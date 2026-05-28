using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Buffers;

using ReturnHome.Database.SQL;
using ReturnHome.Server.Opcodes;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.Managers;
using ReturnHome.Server.Network.Managers;
using ReturnHome.Utilities;
using ReturnHome.Server.Opcodes.Messages.Server;

namespace ReturnHome.Server.Network
{
    /// <summary>
    /// This is user to start processing incoming and outgoing packets
    /// </summary>
    public class RdpCommIn
    {
        public readonly SessionConnectionData connectionData;

        //private readonly List<Message> _outOfOrderMessages = new();

        private readonly Session _session;
        private readonly SessionQueue _sessionQueue;

        public ushort clientID { get; private set; }
        public ushort serverID { get; private set; }
        public long TimeoutTick { get; set; }

        public RdpCommIn(Session session, ushort ClientID, ushort ServerID)
        {
            _session = session;
            _sessionQueue = new(_session);
            clientID = ClientID;
            serverID = ServerID;
            connectionData = new(_session);

            // New network auth session timeouts will always be high.
            //For now hardcode 30 seconds, once we enter world it needs to be like... 2 seconds to ping clients, maybe 60 seconds to disconnect
            //Maybe this would get set by the session location. Pre-memory dump = 30 seconds, memory dump > is 2 seconds
            if (_session.inGame)
                TimeoutTick = DateTime.UtcNow.AddSeconds(2).Ticks;
            else
                TimeoutTick = DateTime.UtcNow.AddSeconds(45).Ticks;
        }

        public void ProcessPacket(ClientPacket packet)
        {
            if (_session.inGame)
                TimeoutTick = DateTime.UtcNow.AddSeconds(2).Ticks;
            else
                TimeoutTick = DateTime.UtcNow.AddSeconds(45).Ticks;
            _session.ResetPing();
            //Let's make sure this isn't a delayed packet etc.
            if (packet.segmentBody.arrival_number <= connectionData.lastReceivedPacketSequence)
                //If this bundle number is 0x0000, we are just recycling through the counter, so don't return
                if (!(packet.segmentBody.flush_ack == 0x0000))
                    return;

            connectionData.lastReceivedPacketSequence = packet.segmentBody.arrival_number;

            if ((packet.segmentBody.bodyFlags & SegmentBodyFlags.guaranted_ack) != 0)
                ProcessRdpReport(packet);

            //Always process messages, even if there is none we will just flow over the code
            ProcessMessageBundle(packet);

            if ((packet.segmentBody.bodyFlags & SegmentBodyFlags.sessionAck) != 0)
                ProcessSessionAck(packet);
        }

        public void ProcessMessageBundle(ClientPacket packet)
        {
            foreach (Message m in packet.segmentBody.messages)
            {
                if ((m.Messagetype == MessageType.PingMessage) || (m.Messagetype == MessageType.ReliableMessage) || (m.Messagetype == MessageType.SegmentReliableMessage))
                {
                    if (m.Sequence != (connectionData.lastReceivedMessageSequence + 1))
                    {
                        //Just ignore out of order reliables for now
                        //_outOfOrderMessages.Add(m);
                        Message.Return(m);
                        continue;
                    }

                    connectionData.lastReceivedMessageSequence++;
                }

                Logger.Info($"Working with message {m.Sequence}");

                switch (m.Messagetype)
                {
                    case MessageType.PingMessage:
                        ProcessOpcode.ProcessPingRequest(_session, m);
                        break;

                    case MessageType.ReliableMessage:
                        ProcessOpcode.ProcessOpcodes(_session, m);
                        break;

                    case MessageType.UnreliableMessage:
                        Logger.Info("!!!Received FC type message from client!!!");
                        break;

                    default:
                        Logger.Info("Processing State Channel from Client");
                        ProcessUnreliable.ProcessUnreliables(_session, m);
                        break;
                }

                Message.Return(m);
            }


            ///Should we just initiate responses to clients through here for now?
            ///Ultimately we want to have a seperate thread with a server tick, 
            ///that may handle initiating sending messages at timed intervals, and initiating data collection such as C9's*/
        }

        private void ProcessRdpReport(ClientPacket packet)
        {
            //Update our connection data object
            //Let's make sure the ack > current saved ack
            if (connectionData.clientLastReceivedMessage <= packet.segmentBody.guaranteed_ack)
                connectionData.clientLastReceivedMessage = packet.segmentBody.guaranteed_ack;

            //Logging an old ack that was received here... should something else happen?
            else
            {
                Logger.Info($"Received an old ack, Expected: {connectionData.clientLastReceivedMessage} Received: {packet.segmentBody.guaranteed_ack}");
                return;
            }

            //Check reliable resend queue
            _sessionQueue.RemoveReliables();
            
            if ((packet.segmentBody.bodyFlags & SegmentBodyFlags.clientUpdateAck) != 0)
            {
                Span<ServerObjectUpdate> temp = connectionData.serverObjects.Span;
                foreach (StateAcks s in packet.segmentBody.stateAcks)
                {
                    if (s.Channel >= 0 && s.Channel <= 0x17)
                        temp[s.Channel].UpdateBaseXor(s.Ack);

                    else if (s.Channel == (byte)MessageType.StatUpdate)
                        connectionData.clientStatUpdate.UpdateBaseXor(s.Ack);

                    else if (s.Channel == (byte)MessageType.GroupUpdate)
                        connectionData.serverGroupUpdate.UpdateBaseXor(s.Ack);
                    else
                        Console.WriteLine($"Received Channel {s.Channel}, not implemented yet or processing error on channel processing");
                }
            }

            ///Triggers Character select, after client soft ack's the 2nd message we sent.
            if ((connectionData.clientLastReceivedMessage == 0x02) && _session.didServerInitiate)
            {
                CharacterSQL charList = new();
                Logger.Info($"{_session.rdpCommIn.clientID.ToString("X")}: Generating Character Select");
                List<Character> MyCharacterList = charList.AccountCharacters(_session);


                //May need to be fixed up
                ServerCreateCharacterList.CreateCharacterList(MyCharacterList, _session);
            }

            ///Triggers creating master session
            else if ((connectionData.lastReceivedMessageSequence == 0x02) && (clientID == ((_session.InstanceID - 1) & 0xFFFF)))
            {
                Logger.Info($"Creating master session for {_session.SessionID}");
                SessionManager.CreateMasterSession(_session);
            }

            ///Trigger Server Select with this?
            else if ((connectionData.lastReceivedMessageSequence == 0x02) && (clientID == (_session.InstanceID & 0xFFFF)))
                ServerListManager.AddSession(_session);
        }


        private void ProcessSessionAck(ClientPacket packet)
        {
            if (_session.InstanceID == packet.segmentBody.instance_local)
                //Session has been ack'd and no longer new
                _session.Instance = false;

            //Doesn't seem to really matter,we could use this as a check and balance and deny the session if this check fails?
            return;
        }
    }

    public class RdpCommOut
    {
        private static MemoryPool<byte> _memoryPool = MemoryPool<byte>.Shared;
        private readonly Session _session;
        public readonly ServerListener _listener;
        public static int maxSize = 0x530;

        public RdpCommOut(Session session, ServerListener listener)
        {
            _session = session;
            _listener = listener;
        }

        public void PrepPackets()
        {
            if (_session.sessionQueue.CheckQueue() || (_session.segmentBodyFlags & SegmentBodyFlags.guaranted_ack) != 0 || (_session.segmentBodyFlags & SegmentBodyFlags.clientUpdateAck) != 0)
            {
                //Take this instance for processing, probablky need to introduce some locking mechanism on this
                SegmentBodyFlags segmentBodyFlags = _session.segmentBodyFlags | SegmentBodyFlags.rdpMessage;

                //Replace with a new instance for next packet
                _session.segmentBodyFlags = new();

                segmentBodyFlags |= (SegmentBodyFlags.guaranted_ack & segmentBodyFlags) != 0 ? SegmentBodyFlags.flushAck : 0;
                Memory<byte> packetBody = _memoryPool.Rent(0x0530).Memory;
                BufferWriter writerBody = new(packetBody.Span);

                Memory<byte> packetHeader = _memoryPool.Rent(0x530).Memory;
                BufferWriter writerHeader = new(packetHeader.Span);

                //Add bundle type first
                AddBundleType(ref writerBody, segmentBodyFlags);

                if ((segmentBodyFlags & SegmentBodyFlags.sessionAck) != 0)
                    ///To ack session, we just repeat session information as an ack
                    AddSessionAck(ref writerBody);

                AddRDPReport(ref writerBody, segmentBodyFlags);

                _session.sessionQueue.WriteMessages(ref writerBody, segmentBodyFlags);

                //Add session header data
                AddSessionHeader(ref writerHeader, writerBody.Position);

                ///Add Session Information
                AddSession(ref writerHeader);

                //Merge Memory's together
                writerHeader.Write(packetBody.Span[0.. writerBody.Position]);

                //Add CRC
                writerHeader.Write(CRC.calculateCRC(writerHeader.Span[0..writerHeader.Position]));

                SocketAsyncEventArgs args = new();
                args.RemoteEndPoint = _session.MyIPEndPoint;
                args.SetBuffer(packetHeader.Slice(0, writerHeader.Position));

                //Send Packet
                _listener.socket.SendToAsync(args);

                _session.Instance = false;
            }
        }

        ///Identifies if full RDPReport is needed or just the current bundle #
        private void AddRDPReport(ref BufferWriter packet, SegmentBodyFlags segmentBodyFlags)
        {
            packet.Write(_session.rdpCommIn.connectionData.lastSentPacketSequence++);
            ///If RDP Report == True, Current bundle #, Last Bundle received # and Last message received #
            if ((segmentBodyFlags & SegmentBodyFlags.guaranted_ack) != 0)
            {
                Logger.Info("Full RDP Report");
                packet.Write(_session.rdpCommIn.connectionData.lastReceivedPacketSequence);
                packet.Write(_session.rdpCommIn.connectionData.lastReceivedMessageSequence);

            }

            //include 4029 ack's as needed
            if ((segmentBodyFlags & SegmentBodyFlags.clientUpdateAck) != 0)
            {
                packet.Write((byte)0x40);
                packet.Write(_session.rdpCommIn.connectionData.client.SeqNum);
                packet.Write((byte)0xF8);
            }
        }

        ///Add our bundle type
        ///Consideration for in world or "certain packet" # is needed during conversion. For now something basic will work
        private void AddBundleType(ref BufferWriter packet, SegmentBodyFlags segmentBodyFlags) => packet.Write((byte)segmentBodyFlags);

        ///Add a session ack to send to client
        private void AddSession(ref BufferWriter packet)
        {
            Logger.Info($"{_session.ClientEndpoint.ToString("X")}: Adding Session Data");
            if (_session.hasInstance)
                packet.Write(_session.InstanceID);

            if (_session.didServerInitiate)
                packet.Write7BitEncodedUInt64(_session.SessionID);
        }

        private void AddSessionAck(ref BufferWriter packet) => packet.Write(_session.InstanceID);

        private void AddSessionHeader(ref BufferWriter packet, int size)
        {
            Logger.Info($"{_session.ClientEndpoint.ToString("X")}: Adding Session Header");
            int value = 0;
            packet.Write(_session.rdpCommIn.serverID);
            packet.Write(_session.rdpCommIn.clientID);

            if (_session.Instance) //When server initiates instance with the client, it will use this
                value |= 0x80000;

            if (_session.didServerInitiate) // Purely a guess.... Something is 0x4000 in this and seems to correspond the initator of the session
                value |= 0x04000;

            else // Server is not master, seems to always have this when not in control
                value |= 0x01000;

            if (_session.hasInstance) // Server always has instance ID, atleast untill we are in world awhile, then it can drop this and the 4 byte instance ID
                value |= 0x02000;

            //Subtract CRC length from total count also
            packet.Write7BitEncodedUInt64((uint)(value + size));
        }
    }
}
