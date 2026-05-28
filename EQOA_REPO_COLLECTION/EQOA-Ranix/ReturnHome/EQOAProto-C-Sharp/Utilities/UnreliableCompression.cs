using System;
using System.Runtime.InteropServices;

namespace ReturnHome.Utilities
{
    
    class Compression
    {
        public static void runLengthEncode(ref BufferWriter writer, Span<byte> span)
        {
            int length = span.Length;
            int thisReal = 0;
            int thisCompress = 0;

            //Cycle through every byte of update message
            for (int i = 0; i < length; i++)
            {
                //Check if null byte
                if (span[i] == 0)
                {
                    //Check if prior bytes were not null
                    if (thisReal > 0)
                    {
                        writer.Write((byte)(thisReal | 0x80));
                        writer.Write((byte)thisCompress);
                        writer.Write(span.Slice((i - thisReal), thisReal));

                        //Reset counters
                        thisCompress = 0;
                        thisReal = 0;
                    }

                    thisCompress += 1;
                    continue;
                }

                //Real byte
                thisReal += 1;
            }

            writer.Write((byte)(thisReal | 0x80));
            writer.Write((byte)thisCompress);
            if (thisReal != 0)
                writer.Write(span.Slice((length - thisReal), thisReal));

            //Ensure any compression is terminated by a 0x00
            writer.Write((byte)0);
        }

        //Pass in Unreliable message and expected unreliable length
        public static unsafe Memory<byte> Run_length_decode(ReadOnlySpan<byte> arg1, int length)
        {
            int offset = 0;
            byte[] messageBuf = new byte[length];

            fixed (byte* tempBuf = &MemoryMarshal.GetReference(arg1))
            {
                byte* buf = tempBuf;
                byte local_70;
                byte counter = 0;

                //real bytes
                uint len_00;

                //null byte count
                uint uVar2;
                int uVar1;

                while (true)
                {
                    local_70 = buf[offset++];
                    if (local_70 == 0) break;
                    len_00 = (uint)local_70 & 0x7f;
                    if ((local_70 & 0x80) == 0)
                    {
                        len_00 = (uint)(local_70 >> 4);
                        uVar2 = (uint)local_70 & 0xf;
                    }

                    else
                    {
                        local_70 = buf[offset++];
                        uVar2 = local_70;
                    }

                    uVar1 = 0;
                    if (uVar2 != 0)
                    {
                        do
                        {
                            messageBuf[counter] = 0;
                            counter += 1;
                            uVar1 += 1;
                        } while (uVar1 < uVar2);
                    }

                    if (len_00 != 0)
                    {
                        for (int i = 0; i < len_00; i++)
                        {
                            messageBuf[counter] = buf[offset++];
                            counter += 1;
                        }
                    }
                }
            }
            return messageBuf;
        }
    }
}
