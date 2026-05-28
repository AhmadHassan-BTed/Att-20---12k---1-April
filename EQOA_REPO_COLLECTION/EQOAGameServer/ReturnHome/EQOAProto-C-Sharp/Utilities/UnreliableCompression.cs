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
        public static Memory<byte> Run_length_decode(ref BufferReader reader, int length)
        {
            Memory<byte> messageBuf = new Memory<byte>(new byte[length]);
            Span<byte> temp = messageBuf.Span;

            byte local_70;
            int counter = 0;

            //real bytes
            uint len_00;

            //null byte count
            int uVar2;

            while (true)
            {
                local_70 = reader.Read<byte>();
                if (local_70 == 0) break;
                len_00 = (uint)local_70 & 0x7f;
                if ((local_70 & 0x80) == 0)
                {
                    len_00 = (uint)(local_70 >> 4);
                    uVar2 = local_70 & 0xf;
                }

                else
                    uVar2 = reader.Read<byte>();

                if (len_00 != 0)
                {
                    counter += uVar2;
                    for (int i = 0; i < len_00; i++)
                    {
                        temp[counter] = reader.Read<byte>();
                        counter += 1;
                    }
                }
            }
            return messageBuf;
        }
    }
}
