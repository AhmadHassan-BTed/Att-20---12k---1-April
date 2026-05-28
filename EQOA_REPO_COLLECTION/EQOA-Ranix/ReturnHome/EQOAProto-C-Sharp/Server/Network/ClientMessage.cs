using System;

using ReturnHome.Utilities;

namespace ReturnHome.Server.Network
{
    public class ClientMessage
    {
        public ReadOnlyMemory<byte> Payload;

        public ushort Opcode { get; }

        public int offset = 0;

        public ClientMessage(ReadOnlyMemory<byte> payload)
        {
            //Payload = payload;
			//offset = 0;
            //Opcode = payload.Span.GetLEUShort(ref offset);
        }
    }
}
