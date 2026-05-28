using ReturnHome.Utilities;

namespace ReturnHome.Server.Network
{
    public class SegmentHeader
    {
        public SegmentHeaderFlags flags;
        public int size;
        public uint instance;
        public int data_0x40000;
        public uint src_addr;
        public long local_endpoint;
        public uint remote_endpoint;
        public SegmentHeader()
        {
        }

        public void Read(ref BufferReader reader, uint source_addr, uint dst_addr)
        {
            ulong flags_len = reader.Read7BitEncodedUInt64();
            flags = (SegmentHeaderFlags)(flags_len & 0xfffff800);
            size = (int)(flags_len & 0x7ff);
            if ((flags & SegmentHeaderFlags.HasInstance) != 0) instance = reader.Read<uint>();
            //if ((flags & SegmentHeaderFlags.data_0x40000) != 0) data_0x40000 = reader.Read<int>();
            //if ((flags & SegmentHeaderFlags.NewInstance) != 0) src_addr = reader.Read<uint>();
            if ((flags & SegmentHeaderFlags.ResetConnection) != 0) local_endpoint = reader.Read<uint>();
            if ((flags & SegmentHeaderFlags.IsRemote) == 0) remote_endpoint = dst_addr;
            else remote_endpoint = (uint)reader.Read7BitEncodedInt64();
        }
    }
}
