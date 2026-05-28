using System.Net;
using System.Threading.Tasks;
using System.Linq;

using ReturnHome.Utilities;
using ReturnHome.Server.Managers;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.Opcodes.Messages.Server;

namespace ReturnHome.Server.Network.Managers
{
    public static class SessionManager
    {
        ///Our IDUP Starter for now
        private static uint InstanceIDUpStarter = 220760;

        ///This is our sessionList
        public static readonly ConcurrentHashSet<Session> SessionHash = new ConcurrentHashSet<Session>();

        /// <summary>
        /// Handles packets from clients, creating/removing sessions, or sending established connections data to process
        /// </summary>
        public static void ProcessPacket(ServerListener listener, ClientPacket packet, IPEndPoint ClientIPEndPoint)
        ///public static void ProcessSession(List<byte> myPacket, bool NewSession)
        {
			Session ClientSession;

            //Remove session
            if ((packet.segmentHeader.flags & SegmentHeaderFlags.ResetConnection) != 0)
            {
                //Add a method here to save session data and character etc, whatever is applicable.
                //Attempt to remove from serverlisting for now
                //Probably be more accurate to track account/connection state to dictate what needs to happen here
                findSession(ClientIPEndPoint, packet.segmentHeader.instance, out ClientSession);
                if (ClientSession == null)
                    return;
                ServerListManager.RemoveSession(ClientSession);
                ClientSession.DropSession(true);

                if (SessionHash.TryRemove(ClientSession))
                    Logger.Info("Session Successfully removed from Session List");
                return;
            }

            //Create a new session
            if ((packet.segmentHeader.flags & SegmentHeaderFlags.NewInstance) != 0)
            {
                // Create New Session
                ClientSession = new Session(listener, ClientIPEndPoint, packet.segmentHeader.instance, packet.segmentHeader.remote_endpoint, packet.Local_Endpoint, listener.serverEndPoint, false);

                //Try adding session to hashset
                if (SessionHash.TryAdd(ClientSession))
                {
                    Logger.Info($"{ClientSession.ClientEndpoint.ToString("X")}: Processing new session");
                    ClientSession.segmentBodyFlags |= SegmentBodyFlags.sessionAck;
                    //Success, keep processing data
                    ClientSession.rdpCommIn.ProcessPacket(packet);
                }
			}

            else
            {
				//Find session, if it returns true, outputs session
                if (findSession(ClientIPEndPoint, out ClientSession))
                {
					//Checks if IP/Port matches expected session to incoming packet
					//This might not be needed?
                    if (ClientSession.MyIPEndPoint.Equals(ClientIPEndPoint))
                        ClientSession.rdpCommIn.ProcessPacket(packet);

                    else
                    {
                        //Somehow got the wrong session? Def. Needs a log to notate this
                        ClientSession = null;
                        Logger.Err($"Session for Id {packet.Local_Endpoint} has IP {ClientSession.MyIPEndPoint} but packet has IP {ClientIPEndPoint}");
                    }
                }

                else
                {
                    Logger.Info($"Unsolicited Packet from {ClientIPEndPoint} with Id { packet.Local_Endpoint}");
                }
            }
        }

        /// <summary>
        /// Finds a session and returns it
        /// </summary>
        public static bool findSession(IPEndPoint ClientIPEndPoint, uint InstanceID, out Session actualSession)
        {
            foreach (Session ClientSession in SessionHash)
            {
                if (ClientSession.MyIPEndPoint.Equals(ClientIPEndPoint) && ClientSession.InstanceID == InstanceID)
                {
                    actualSession = ClientSession;
                    return true;
                }
            }

            //Need logging to indicate actual session was not found
            actualSession = default;
            return false;
        }

        //Have two seperate FindSession functions to handle the times instanceID is not in header, may be a better way to do this.
        public static bool findSession(IPEndPoint ClientIPEndPoint, out Session actualSession)
        {
            foreach (Session ClientSession in SessionHash)
            {
                if (ClientSession.MyIPEndPoint.Equals(ClientIPEndPoint))
                {
                    actualSession = ClientSession;
                    return true;
                }
            }

            //Need logging to indicate actual session was not found
            actualSession = default;
            return false;
        }

        public static uint ObtainIDUp()
        {
            ///Eventually would need some checks to make sure it isn't taken in bigger scale
            uint NewID = InstanceIDUpStarter;
            ///Increment by 2 every pull
            ///Client will Transition into InstanceIDUpStarter + 1
            InstanceIDUpStarter += 2;

            return NewID;
        }

        public static void CreateMasterSession(Session session)
        {
            Session NewMasterSession = new Session(session.rdpCommOut._listener, session.MyIPEndPoint, DNP3Creation.DNP3Session(), ObtainIDUp(), session.rdpCommIn.clientID, session.rdpCommIn.serverID, true);
            NewMasterSession.AccountID = session.AccountID;
            NewMasterSession.Instance = true;

            if (SessionHash.TryAdd(NewMasterSession))
            {
                //Start client contact here
                GenerateClientContact(NewMasterSession);
            }

        }

        public static void CreateMemoryDumpSession(Session session, Character MyCharacter)
        {
            //Start new session 
            Session NewMasterSession = new Session(session.rdpCommOut._listener, session.MyIPEndPoint, DNP3Creation.DNP3Session(), session.SessionID + 1, session.rdpCommIn.clientID, session.rdpCommIn.serverID, true);
            NewMasterSession.Instance = true;
            NewMasterSession.AccountID = session.AccountID;
            NewMasterSession.MyCharacter = MyCharacter;
            NewMasterSession.MyCharacter.characterSession = NewMasterSession;
            NewMasterSession.MyCharacter.ObjectID = NewMasterSession.SessionID;
            EntityManager.AddEntity(MyCharacter);

            if (SessionHash.TryAdd(NewMasterSession))
            {
                Logger.Info($"Session {NewMasterSession.SessionID} starting Memory Dump");
                ServerMemoryDump.MemoryDump(NewMasterSession);
            }
        }

        ///Used when starting a master session with client.
        public static void GenerateClientContact(Session session)
        {
            ServerOpcode0x07D1.Opcode0x07D1(session);
            ServerOpcode0x07F5.Opcode0x07F5(session);
        }
		
		/// <summary>
        /// Dispatches all outgoing messages.<para />
        /// Removes dead sessions.
        /// </summary>
        public static int BeginSessionWork()
        {
            int sessionCount = 0;

            // Removes sessions in the NetworkTimeout state, including sessions that have reached a timeout limit. These should be dropped before processing anything
            foreach (var session in SessionHash.Where(k => !Equals(null, k)))
            {
                if (session.PendingTermination)
                {
                    session.DropSession();
                    continue;
                }
                sessionCount++;
            }

            MapManager.UpdateMaps();
            GroupManager.DistributeGroupUpdates();
            // The session tick outbound processes pending actions and handles outgoing messages
            Parallel.ForEach(SessionHash, s => s?.TickOutbound());
			
            return sessionCount;
        }
    }
}
