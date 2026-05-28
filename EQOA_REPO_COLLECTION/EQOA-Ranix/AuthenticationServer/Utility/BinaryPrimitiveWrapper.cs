using System;
using System.Buffers.Binary;
using AuthServer.Server;

namespace AuthServer.Utility
{
    class BinaryPrimitiveWrapper
    {
        public static int GetBEInt(EQOAClient Client)
        {
            Client.offset += 4;
            return BinaryPrimitives.ReadInt32BigEndian(Client.ClientPacket.Span.Slice(Client.offset - 4, 4));
        }

        public static uint GetLEUint(ReadOnlyMemory<byte> mem, ref int offset)
        {
            offset += 4;
            return BinaryPrimitives.ReadUInt32LittleEndian(mem.Span.Slice(offset - 4, 4));
        }
        public static short GetLEShort(ReadOnlyMemory<byte> mem, ref int offset)
        {
            offset += 2;
            return BinaryPrimitives.ReadInt16LittleEndian(mem.Span.Slice(offset - 2, 2));
        }

        public static ushort GetLEUShort(ReadOnlyMemory<byte> mem, ref int offset)
        {
            offset += 2;
            return BinaryPrimitives.ReadUInt16LittleEndian(mem.Span.Slice(offset - 2, 2));
        }

        public static byte GetLEByte(ReadOnlyMemory<byte> mem, ref int offset)
        {
            offset += 1;
            return mem.Span[offset - 1];
        }
    }
}
