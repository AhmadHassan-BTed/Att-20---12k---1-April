using System;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Network
{
    public class PacketMessageHeader
    {
        public int HeaderSize { get; set; }

        public byte messageType { get; set; }
        public ushort MessageNumber { get; set; }
        public ushort Opcode { get; private set; }
        public ushort Size { get; set; }
        public int Count { get; set; }
        public int Index { get; set; } // If client sends 0xFA types, may be needed to combine message splits
        public bool Split { get; set; } // If client sends 0xFA types, may be needed to combine message splits
        public byte XorByte { get; set; }

        public void Unpack(ref BufferReader reader)
        {
            //Read first byte, if it is FF, read an additional byte (Indicates >255byte message
            messageType = reader.Read<byte>();

            byte sizeCheck = reader.Read<byte>();

            if (sizeCheck == 0xFF)
                Size = reader.Read<ushort>();

            else
                Size = sizeCheck;

            if ((byte)MessageType.UnreliableMessage != messageType)
                MessageNumber = reader.Read<ushort>();

            if ((byte)MessageType.PingMessage == messageType)
                return;

            //unreliables and Reliables have opcodes. No  other opcode type/channel does
            else if ((byte)MessageType.UnreliableMessage == messageType || (byte)MessageType.ReliableMessage == messageType)
            {
                Opcode = reader.Read<ushort>();
                //Subtract 2 from size after reading opcode
                Size -= 2;
            }

            else if (messageType == (byte)MessageType.ClientUpdate)
                XorByte = reader.Read<byte>();

            else
                throw new Exception("Error occured on processing data");

        }
    }
}
