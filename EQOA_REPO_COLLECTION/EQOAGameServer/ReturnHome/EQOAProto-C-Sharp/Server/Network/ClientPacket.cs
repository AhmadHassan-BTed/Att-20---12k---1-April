using System;

using ReturnHome.Utilities;

namespace ReturnHome.Server.Network
{
    public class ClientPacket
    {
        public ushort Local_Endpoint;
        public uint Remote_Endpoint;
        public SegmentHeader segmentHeader = new();
        public SegmentBody segmentBody = new();
        public bool ProcessPacket(Memory<byte> buffer)
        {
            BufferReader reader = new BufferReader(buffer.Span);

            //Track memory offset as packet is processed
            reader.Position = reader.Length - 4;
            if(reader.Read<uint>() != CRC.calculateCRC(reader.Buffer.Slice(0, reader.Length - 4)))
            {
                reader.Position = 0;
                Logger.Err($"CRC Failed for endpoint {reader.Read<ushort>():2X}");
            }

            reader.Position = 0;

            Local_Endpoint = reader.Read<ushort>();
            Remote_Endpoint = reader.Read<ushort>(); //TODO: Eventually need to consider scoping for 0xFFFF endpoint for server transfers
                
            segmentHeader.Read(ref reader, Local_Endpoint, Remote_Endpoint);

            //Nothing else to read, need to terminate session
            if((segmentHeader.flags & SegmentHeaderFlags.ResetConnection) != 0)
                return true;

            if (!segmentBody.Read(ref reader, buffer, segmentHeader))
                return false;

            return true;
        }
    }
}
