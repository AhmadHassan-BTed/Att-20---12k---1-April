using System;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Network
{
    public class ClientPacketMessage : PacketMessage
    {
        public bool Unpack(ref BufferReader reader, ReadOnlyMemory<byte> buffer)
        {
            Header.Unpack(ref reader);

            //Check if message type is client update
            if (Header.messageType == (byte)MessageType.ClientUpdate)
            {
                Data = Compression.Run_length_decode(reader.Buffer.Slice(reader.Position), Header.Size);
                reader.Position = (reader.Length - 4);
                return true;
            }

            else
            {
                Data = buffer.Slice(reader.Position, Header.Size);
                reader.Position += Header.Size;
                return true;
            }
        }
    }
}
