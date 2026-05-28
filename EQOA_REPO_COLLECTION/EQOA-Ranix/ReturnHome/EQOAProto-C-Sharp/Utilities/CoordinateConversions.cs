using System;
using System.Runtime.InteropServices;

namespace ReturnHome.Utilities
{
    class CoordinateConversions
    {
        public static float ConvertXZToFloat(uint Value)
        { return (32000.0f + 4000.0f) * (int)Value / 0xffffff - 4000.0f; }

        public static float ConvertYToFloat(uint Value)
        { return (1000.0f + 1000.0f) * (int)Value / 0xffffff - 1000.0f; }

        public static float ConvertFacing(byte Value)
        { return ((sbyte)Value * 0.0245433693f); }

        public static void Xor_data(Memory<byte> dst, ReadOnlyMemory<byte> src, int len)
        {
            Span<byte> dstTemp = dst.Span;
            ReadOnlySpan<byte> srcTemp = src.Span;

            if(dst.Length == src.Length)
            {
                for (int i = 0; i < len; i++)
                    dstTemp[i] = (byte)(dstTemp[i] ^ srcTemp[i]);
            }
        }

        public static unsafe void Xor_data(Memory<byte> returnMem, Memory<byte> dst, ReadOnlyMemory<byte> src, int len)
        {

            fixed (byte* srctempBuf = &MemoryMarshal.GetReference(src.Span))
            fixed (byte* dsttempBuf = &MemoryMarshal.GetReference(dst.Span))
            fixed (byte* rtntempBuf = &MemoryMarshal.GetReference(returnMem.Span))
            {
                byte* ptr;
                byte* rtnBuf = rtntempBuf;
                byte* srcBuf = srctempBuf;
                byte* dstBuf = dsttempBuf;
                int i = 0;

                if (0 < len)
                {
                    do
                    {
                        ptr = srcBuf + i;
                        i = i + 1;
                        *rtnBuf = (byte)(*dstBuf ^ *ptr);
                        rtnBuf = rtnBuf + 1;
                        dstBuf = dstBuf + 1;
                    } while (i < len);
                }
                return;
            }
        }
    }
}
