using System;
using System.Collections.Generic;
using System.Data;
using System.Reflection.PortableExecutable;
using ReturnHome.Server.Opcodes;
using ReturnHome.Utilities;
using static Microsoft.EntityFrameworkCore.DbLoggerCategory.Database;

namespace ReturnHome.Server.Network
{
    public class SegmentBody
    {
        private readonly MessageType _check = (MessageType)0xF7;
        public byte flags;
        public int instance_local;
        public bool instance_local_ok;
        public bool arrival_positive;
        public ushort arrival_number;
        public ushort flush_ack;
        public long unknown_packed;
        public bool guaranteed_ack_present_maybe;
        public ushort guaranteed_ack;
        public long guaranteed_ack_mask;
        public ushort seqnum;
        public short flags2;
        public int unknown3;
        public byte[] unknown2;
        public GameOpcode opcode;

        //Initialize an array to hold the max possible acks, 24 for state channels, 3 for group, stats and buffs
        public List<StateAcks> stateAcks = new List<StateAcks>();

        public List<Message> messages= new List<Message>();
        public SegmentBodyFlags bodyFlags;

        public SegmentBody()
        { }

        public bool Read(ref BufferReader reader, Memory<byte> buffer, SegmentHeader segmentHeader)
        {
            int body_start = reader.Position;
            bodyFlags = (SegmentBodyFlags)reader.Read<byte>();

            if ((bodyFlags & SegmentBodyFlags.sessionAck) != 0) instance_local = reader.Read<int>();

            arrival_positive = true;
            arrival_number = reader.Read<ushort>();

            if ((bodyFlags & SegmentBodyFlags.flushAck) == 0)
            { }
            else
            { 
                flush_ack = reader.Read<ushort>();
                if ((bodyFlags & SegmentBodyFlags.unknown_packed) != 0)
                {
                    unknown_packed = reader.Read7BitEncodedInt64();
                    Logger.Info($"Received an unknown packed value of {unknown_packed}");
                }
                //Should we check against flush_ack here? If flush_ack < most recent received packet#, drop?
            }

            if ((bodyFlags & SegmentBodyFlags.guaranted_ack) == 0)
            { }

            else
            {
                guaranteed_ack = reader.Read<ushort>();

                if ((bodyFlags & SegmentBodyFlags.guaranteed_ack_mask) != 0)
                {
                    guaranteed_ack_mask = reader.Read7BitEncodedInt64();
                }

                //Should we also check against the guaranteed_ack here? Or just check this later on? We don't get our session info quite yet
            }

            if((bodyFlags & SegmentBodyFlags.clientUpdateAck) != 0)
            {
                while(true)
                {
                    byte channel = reader.Read<byte>();

                    //Indicates end of state ack's
                    if (channel == 0xF8)
                        break;

                    if(channel > 0xF8)
                    {
                        Logger.Info("Received an illegal channel, dropping");
                        return false;
                    }

                    stateAcks.Add(new StateAcks(channel, reader.Read<ushort>()));
                    //Do we want to check against channel state acks here? We would likely want to capture if an incoming state ack is less then currently ack'd, and drop the packet? Since that could be corrupted/stale data as a whole

                }

            }
            MessageType type;
            int size;
            while (true)
            {
                while (true)
                {
                    //Consider adding session checks here if we pull that before processing the packet?
                    if (reader.Position - body_start >= segmentHeader.size)
                        return true;

                    type = (MessageType)reader.Read<byte>();
                    size = reader.ReadSize();
                    if (0x07FE < size)
                    {
                        Logger.Err("Size was > Segment Header");
                        return false;
                    }

                    //exit loop as the message type is no longer a reliable
                    if (type != MessageType.ReliableMessage && type != MessageType.PingMessage && type != MessageType.SegmentReliableMessage)
                        break;

                    seqnum = reader.Read<ushort>();

                    if (type != MessageType.PingMessage)
                    {
                        opcode = (GameOpcode)reader.Read<ushort>();
                        size -= 2; //Remove opcode from size if it is not a ping message (no opcode in ping)
                    }

                    Message message = Message.Create(type, opcode);
                    buffer.Span[reader.Position..(reader.Position + size)].CopyTo(message.Span);
                    message.Size = size;
                    message.AddSequence(seqnum);

                    messages.Add(message);
                    //Manually advance reader since we pulled the message straight from the buffer
                    reader.Advance(size);
                }

                if (type == MessageType.UnreliableMessage)
                {
                    seqnum = 0;
                    opcode = (GameOpcode)reader.Read<ushort>();

                    size -= 2; //remove opcode from size

                    Message message = Message.Create(type, opcode);
                    buffer.Span[reader.Position..(reader.Position + size)].CopyTo(message.Span);

                    messages.Add(message);
                    reader.Advance(size);
                }

                else if (_check < type)
                {
                    Logger.Err("Received Invalid Message Type");
                    return false;
                }

                //State message then? We should only get a 0x40 channel so 41 is only size?
                else
                {
                    seqnum = reader.Read<ushort>();
                    byte refnum = reader.Read<byte>();

                    if (refnum >= 0x21)
                    {
                        Logger.Err("Received a bad state reference number; >= 0x21");
                        return false;
                    }

                    Message message = new Message(type, seqnum, refnum, Compression.Run_length_decode(ref reader, size));
                    messages.Add(message);
                }
            }
        }
    }
}
