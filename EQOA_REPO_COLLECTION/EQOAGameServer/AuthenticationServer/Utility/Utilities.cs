using System.Text;
using AuthServer.Server;

namespace AuthServer.Utility
{
	static public class Utilities
	{
        public static string ReturnString(EQOAClient Client, int Count)
        {
            try
            { return Encoding.Default.GetString(Client.ClientPacket.Span.Slice(Client.offset, Count)); }

            finally
            { Client.offset += Count; }
        }
	}

    public class ByteSwaps
    {
        public static ushort SwapBytes(ushort x)
        {
            return (ushort)((ushort)((x & 0xff) << 8) | ((x >> 8) & 0xff));
        }

        public static uint SwapBytes(uint x)
        {
            return (x & 0x000000FFU) << 24 |
                   (x & 0x0000FF00U) << 8 |
                   (x & 0x00FF0000U) >> 8 |
                   (x & 0xFF000000U) >> 24;
        }


        public static ulong SwapBytes(ulong value)
        {
            ulong uvalue = value;
            ulong swapped =
                 ((0x00000000000000FF) & (uvalue >> 56)
                 | (0x000000000000FF00) & (uvalue >> 40)
                 | (0x0000000000FF0000) & (uvalue >> 24)
                 | (0x00000000FF000000) & (uvalue >> 8)
                 | (0x000000FF00000000) & (uvalue << 8)
                 | (0x0000FF0000000000) & (uvalue << 24)
                 | (0x00FF000000000000) & (uvalue << 40)
                 | (0xFF00000000000000) & (uvalue << 56));
            return swapped;
        }
    }
}
