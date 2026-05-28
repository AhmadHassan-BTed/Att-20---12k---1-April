using System;

namespace ReturnHome.Server.Network
{
    [Flags]
    public enum SegmentBodyFlags : byte
    {
        flushAck            = 0x01,
        guaranted_ack       = 0x02,
        unknown_packed      = 0x04,
        guaranteed_ack_mask = 0x08,
        clientUpdateAck     = 0x10,
        rdpMessage          = 0x20,
        sessionAck          = 0x40,
    }
}
