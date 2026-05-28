using System.Net;
using System;

using ReturnHome.Utilities;
using ReturnHome.Server.Opcodes.Chat;
using ReturnHome.Server.Managers;
using ReturnHome.Server.Network.Managers;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.Opcodes;

namespace ReturnHome.Server.Network
{
    /// This is individual client session object
    public class Session
    {
        //Message List
		//New data to Session...
		public RdpCommIn rdpCommIn { get; private set; } 
		public RdpCommOut rdpCommOut { get; private set; }
        public SessionQueue sessionQueue { get; private set; }
        //public ServerListener listener { get; private set; }
		
		
        private short _pingCount = 0;
        public uint InstanceID;
        public uint SessionID;
		public ushort ClientEndpoint;
		
		//Helps to identify master of session
		public bool didServerInitiate { get; private set; }
        public bool PendingTermination { get; set; } = false;
		
		///Client IPEndPoint
        public IPEndPoint MyIPEndPoint { get; private set; }
		
		//End

        ///SessionList Objects, probably need bundle information here too?
        public ushort ActorUpdatMessageCount = 1;

        public bool hasInstance = true;
        public bool Instance = false;

        ///BundleType Transition
        public bool BundleTypeTransition = false;

        public bool serverSelect;
        public SegmentBodyFlags PacketBodyFlags = new();
        public bool characterInWorld = false;
        public bool inGame = false;
        public bool objectUpdate = false;
        public bool unkOpcode = true;
        public bool OIDUpdate = true;
        ///Once we receive account ID, this should never change...
        public int AccountID;

        //Character 
        public Character MyCharacter { get; set; }

        /// <summary>
        /// Create a Simple Client Session
        /// </summary>
        /// <param name="MyIPEndPoint"></param>
        public Session(ServerListener listener, IPEndPoint myIPEndPoint, uint instanceID, uint sessionID, ushort clientID, ushort serverID, bool DidServerInitiate)
        {
            didServerInitiate = DidServerInitiate;
            SessionID = sessionID;
            MyIPEndPoint = myIPEndPoint;
            InstanceID = instanceID;
            rdpCommIn = new(this, clientID, serverID);
			rdpCommOut = new(this, listener);
			sessionQueue = new(this);
        }

        public void UnreliablePing()
        {
            Message message = Message.Create(MessageType.UnreliableMessage, GameOpcode.BestEffortPing);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            message.Size = writer.Position;
            sessionQueue.Add(message);
            _pingCount++;
        }

        //TODO This should get built into somewhere else, eventually
        public void CoordinateUpdate()
        {
            string message = $"Coordinates: X: {MyCharacter.x} Y: {MyCharacter.y} Z: {MyCharacter.z} F: {MyCharacter.FacingF}";

            ChatMessage.GenerateClientSpecificChat(this, message);
        }

        //TODO Put this somewhere else
        public void TargetUpdate()
        {
            string message = $"Targeting ObjectID: {MyCharacter.Target}";
            ChatMessage.GenerateClientSpecificChat(this, message);
        }

        public void ResetPing()
        {
            _pingCount = 0;
        }

        //Override the GetHashcodeMethod so that Hashset works properly as our SessionHolder
        public override int GetHashCode()
        {
            unchecked
            {
                int hash = (int)2166136261;
                hash = (hash ^ MyIPEndPoint.GetHashCode()) * 16777619;
                hash = (hash ^ InstanceID.GetHashCode()) * 16777619;
                return hash;
            }
        }

        /// <summary>
        /// This will send outgoing packets as well as the final logoff message.
        /// </summary>
        public void TickOutbound()
        {
            //Add some state stuff? To identify some stuff
            // Checks if the session has stopped responding.
            if (DateTime.UtcNow.Ticks >= rdpCommIn.TimeoutTick)
            {
                UnreliablePing();
                if (inGame)
                    rdpCommIn.TimeoutTick = DateTime.UtcNow.AddSeconds(2).Ticks;
                else
                    rdpCommIn.TimeoutTick = DateTime.UtcNow.AddSeconds(45).Ticks;
            }

            if (characterInWorld)
            {
                //Check for updates on current objects
                foreach (ServerObjectUpdate i in rdpCommIn.connectionData.serverObjects.Span)
                    i.GenerateUpdate();

                rdpCommIn.connectionData.clientStatUpdate.GenerateUpdate();
            }

            PendingTermination = inGame ? _pingCount >= 50 ? true : false : _pingCount >= 10 ? true : false;
                //Send a disconnect from server to client, then remove the session
                //For now just remove the session

            rdpCommOut.PrepPackets();
        }

        //Optional override for dropping the session, currently simplifies for removing a character when they log out
        public void DropSession(bool Override = false)
        {
            if (!PendingTermination) return;

            MapManager.RemoveObject(MyCharacter);
            PlayerManager.RemovePlayer(MyCharacter);
            EntityManager.RemoveEntity(MyCharacter);
            SessionManager.SessionHash.TryRemove(this);
        }
    }
}
