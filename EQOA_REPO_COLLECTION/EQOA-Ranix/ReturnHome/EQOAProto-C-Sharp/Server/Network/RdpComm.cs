using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Net.Sockets;

using ReturnHome.Database.SQL;
using ReturnHome.Server.Opcodes;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.Managers;
using ReturnHome.Server.Network.Managers;
using ReturnHome.Utilities;
using ReturnHome.Server.Opcodes.Messages.Server;
using System.Buffers;

namespace ReturnHome.Server.Network
{
    /// <summary>
    /// This is user to start processing incoming and outgoing packets
    /// </summary>
    public class RdpCommIn
    {
        public readonly SessionConnectionData connectionData;

        private readonly ConcurrentDictionary<ushort, ReadOnlyMemory<byte>> _outOfOrderMessages = new();

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
            if (packet.Header.ClientBundleNumber <= connectionData.lastReceivedPacketSequence)
                //If this bundle number is 0x0000, we are just recycling through the counter, so don't return
                if (!(packet.Header.ClientBundleNumber == 0x0000))
                    return;


            connectionData.lastReceivedPacketSequence = packet.Header.ClientBundleNumber;

            if (packet.Header.RDPReport)
                ProcessRdpReport(packet);

            if (packet.Header.ProcessMessage)
                ProcessMessageBundle(packet);

            if (packet.Header.SessionAck)
                ProcessSessionAck(packet);
        }

        public void ProcessMessageBundle(ClientPacket packet)
        {
            //Check to see if packet contains next message #, if it doesn't... add them to out of order?
            //Will need to check against unreliables first
            if (packet.Messages.ContainsKey((ushort)(connectionData.lastReceivedMessageSequence + 1)))
            {
                while (true)
                {
                    if (packet.Messages.TryRemove((ushort)(connectionData.lastReceivedMessageSequence + 1), out PacketMessage message))
                    {
                        connectionData.lastReceivedMessageSequence++;
                        Logger.Info($"Working with message {message.Header.MessageNumber}");

                        switch (message.Header.messageType)
                        {
                            case (byte)MessageType.PingMessage:
                                ProcessOpcode.ProcessPingRequest(_session, message);
                                break;

                            case (byte)MessageType.ReliableMessage:
                                ProcessOpcode.ProcessOpcodes(_session, message);
                                break;

                            case (byte)MessageType.UnreliableMessage:
                                Logger.Info("!!!Received FC type message from client!!!");
                                break;

                            default:
                                Logger.Info("Processing Object update messages");
                                ProcessUnreliable.ProcessUnreliables(_session, message);
                                //Do stuff to process 0x40 type messages from client
                                break;
                        }
                    }

                    //Add remaining messages to out of order since
                    else
                    {
                        foreach (var i in packet.Messages)
                            _outOfOrderMessages.TryAdd(i.Key, i.Value.Data);

                        //Should be done working with messages now
                        break;
                    }

                    if (packet.Messages.Count == 0)
                        break;
                }
            }
            //Add all of these messages to out of order list
            else
            {
                foreach (var i in packet.Messages)
                    _outOfOrderMessages.TryAdd(i.Key, i.Value.Data);
            }

            if (packet.Header.ChannelAcks != null)
            {
                byte temp2;
                Span<ServerObjectUpdate> temp = connectionData.serverObjects.Span;
                foreach (KeyValuePair<byte, ushort> ack in packet.Header.ChannelAcks)
                {
                    temp2 = ack.Key;

                    if (temp2 >= 0 && temp2 <= 0x17)
                        temp[temp2].UpdateBaseXor(ack.Value);

                    else if (temp2 == (byte)MessageType.StatUpdate)
                        connectionData.clientStatUpdate.UpdateBaseXor(ack.Value);
                    else
                        Console.WriteLine($"Received Channel {temp2}, not implemented yet or processing error on channel processing");
                }
            }

            //Check client update message
            if (packet.clientUpdate != null)
            {
                Logger.Info("Processing Object update messages");
                ProcessUnreliable.ProcessUnreliables(_session, packet.clientUpdate);
            }
            Logger.Info($"{_session.ClientEndpoint.ToString("X")}: Done processing messages in packet");
            ///Should we just initiate responses to clients through here for now?
            ///Ultimately we want to have a seperate thread with a server tick, 
            ///that may handle initiating sending messages at timed intervals, and initiating data collection such as C9's
        }

        private void ProcessRdpReport(ClientPacket packet)
        {
            //Update our connection data object
            //Let's make sure the ack > current saved ack
            if (connectionData.clientLastReceivedMessage < packet.Header.ClientMessageAck)
                connectionData.clientLastReceivedMessage = packet.Header.ClientMessageAck;

            //Logging an old ack that was received here... should something else happen?
            else
            {
                Logger.Info($"Received an old ack, Expected: {connectionData.lastReceivedMessageSequence} Received: {packet.Header.ClientMessageAck}");
                return;
            }

            //Check reliable resend queue
            _sessionQueue.RemoveReliables();

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
            {
                ServerListManager.AddSession(_session);
            }
        }


        private void ProcessSessionAck(ClientPacket packet)
        {
            if (_session.InstanceID == packet.Header.SessionAckID)
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
        private List<ReadOnlyMemory<byte>> _messageList = new();
        private int _size = 0;
        private int _crcPosition = 0;
        private int _value = 0;
        public static int maxSize = 0x530;

        public RdpCommOut(Session session, ServerListener listener)
        {
            _session = session;
            _listener = listener;
        }

        public void PrepPackets()
        {
            if (_session.sessionQueue.CheckQueue() || _session.PacketBodyFlags.RdpReport || _session.PacketBodyFlags.clientUpdateAck)
            {
                //Take this instance for processing, probablky need to introduce some locking mechanism on this
                SegmentBodyFlags segmentBodyFlags = _session.PacketBodyFlags;

                //Replace with a new instance for next packet
                _session.PacketBodyFlags = new();

                Memory<byte> packet = _memoryPool.Rent(0x0530).Memory;
                BufferWriter writer = new(packet.Span);
                // 4 bytes for endpoints,  , if has instance + 4, if RemoteMaster true + 3? Depends how we handle sessionID's, for now 3 works 
                writer.Position = GetHeaderLength(segmentBodyFlags);

                _session.sessionQueue.WriteMessages(ref writer, segmentBodyFlags);

                //Reset writer to write session data
                //Actual size...
                _size = writer.Position - _size;
                _crcPosition = writer.Position;
                writer.Position = 0;

                //Add session header data
                AddSessionHeader(ref writer);

                ///Add Session Information
                AddSession(ref writer);

                //Add bundle type first
                AddBundleType(ref writer, segmentBodyFlags);

                ///Add session ack here if it has not been done yet
                ///Lets client know we acknowledge session
                ///Making sure remoteMaster is 1 (client) makes sure we have them ack our session
                if (segmentBodyFlags.SessionAck)
                {
                    //Change this to false then process
                    segmentBodyFlags.SessionAck = false;

                    ///To ack session, we just repeat session information as an ack
                    AddSessionAck(ref writer);
                }

                AddRDPReport(ref writer, segmentBodyFlags);

                writer.Position = _crcPosition;

                //_ = packet.Length - 4 != 0 ? throw new Exception("Error occured with PAcket Length") : true;

                //Add CRC
                writer.Write(CRC.calculateCRC(writer.Span[0..writer.Position]));

                SocketAsyncEventArgs args = new();
                args.RemoteEndPoint = _session.MyIPEndPoint;
                args.SetBuffer(packet.Slice(0, writer.Position));

                //Send Packet
                _listener.socket.SendToAsync(args);

                _messageList.Clear();
                _value = 0;
                _crcPosition = 0;
                _size = 0;
                _session.Instance = false;
            }
        }

        private int GetHeaderLength(SegmentBodyFlags segmentBodyFlags)
        {
            //endpoints
            int headerLength = 4;

            if (_session.Instance) //When server initiates instance with the client, it will use this
                _value |= 0x80000;

            if (_session.didServerInitiate) // Purely a guess.... Something is 0x4000 in this and seems to correspond the initator of the session
            {
                //This needs to be more dynamic, technically the sessionID length can vary when packed but 3 is super common
                headerLength += 3;
                _value |= 0x04000;
            }

            else // Server is not master, seems to always have this when not in control
                _value |= 0x01000;

            if (_session.hasInstance) // Server always has instance ID, atleast untill we are in world awhile, then it can drop this and the 4 byte instance ID
            {
                headerLength += 4;
                _value |= 0x02000;
            }

            //If _value is over 0x3000, means the header info should be 3 bytes. is _value is 0, header will be the packet length using variable length integer.... So will have to rethink this a little bit eventually
            byte temp = Utility_Funcs.VariableUIntLength((ulong)_value);
            headerLength += temp + 3;

            //Utilize this to get size for header info
            _size = headerLength - 3;

            if (segmentBodyFlags.SessionAck)
                headerLength += 4;

            if (segmentBodyFlags.RdpReport)
                headerLength += 4;

            if (segmentBodyFlags.clientUpdateAck)
                headerLength += 4;

            return headerLength;
        }

        private void AddMessages(ref BufferWriter packet, List<ReadOnlyMemory<byte>> messageList)
        {
            foreach (ReadOnlyMemory<byte> message in messageList)
                packet.Write(message.Span);
        }

        ///Identifies if full RDPReport is needed or just the current bundle #
        private void AddRDPReport(ref BufferWriter packet, SegmentBodyFlags segmentFlags)
        {
            packet.Write(_session.rdpCommIn.connectionData.lastSentPacketSequence++);
            ///If RDP Report == True, Current bundle #, Last Bundle received # and Last message received #
            if (segmentFlags.RdpReport)
            {
                Logger.Info("Full RDP Report");
                if (_session.rdpCommIn.connectionData.lastReceivedPacketSequence == 0)
                    throw new Exception("wtf");
                packet.Write(_session.rdpCommIn.connectionData.lastReceivedPacketSequence);
                packet.Write(_session.rdpCommIn.connectionData.lastReceivedMessageSequence);
            }

            //include 4029 ack's as needed
            if (segmentFlags.clientUpdateAck)
            {
                packet.Write((byte)0x40);
                packet.Write(_session.rdpCommIn.connectionData.client.BaseXorMessage);
                packet.Write((byte)0xF8);
            }
        }

        ///Add our bundle type
        ///Consideration for in world or "certain packet" # is needed during conversion. For now something basic will work
        private void AddBundleType(ref BufferWriter packet, SegmentBodyFlags segmentBodyFlags)
        {
            byte segBody = 0;

            //Always has this
            segBody |= 0x20;

            if (segmentBodyFlags.RdpReport)
            {
                segBody |= 0x03;
            }

            if (segmentBodyFlags.clientUpdateAck)
            {
                segBody |= 0x10;
            }

            if (segmentBodyFlags.SessionAck)
            {
                segBody |= 0x40;
            }

            packet.Write(segBody);
        }

        ///Add a session ack to send to client
        private void AddSession(ref BufferWriter packet)
        {
            Logger.Info($"{_session.ClientEndpoint.ToString("X")}: Adding Session Data");
            if (_session.hasInstance)
                packet.Write(_session.InstanceID);

            if (_session.didServerInitiate)
                packet.Write7BitEncodedUInt64(_session.SessionID);
        }

        private void AddSessionAck(ref BufferWriter packet)
        {
            packet.Write(_session.InstanceID);
        }

        private void AddSessionHeader(ref BufferWriter packet)
        {
            Logger.Info($"{_session.ClientEndpoint.ToString("X")}: Adding Session Header");

            packet.Write(_session.rdpCommIn.serverID);
            packet.Write(_session.rdpCommIn.clientID);

            //Subtract CRC length from total count also
            packet.Write7BitEncodedUInt64((uint)(_value + _size));
        }
    }
}
