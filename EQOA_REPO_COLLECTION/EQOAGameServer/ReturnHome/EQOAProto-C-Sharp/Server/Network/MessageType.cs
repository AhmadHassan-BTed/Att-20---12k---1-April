
namespace ReturnHome.Server.Network
{
    public enum MessageType : byte
    {
        SegmentReliableMessage = 0xFA,
        ReliableMessage        = 0xFB,
        UnreliableMessage      = 0xFC,
        PingMessage            = 0xF9,
        ObjectUpdateChannel0   = 0x00,
        ObjectUpdateChannel1   = 0x01,
        ObjectUpdateChannel2   = 0x02,
        ObjectUpdateChannel3   = 0x03,
        ObjectUpdateChannel4   = 0x04,
        ObjectUpdateChannel5   = 0x05,
        ObjectUpdateChannel6   = 0x06,
        ObjectUpdateChannel7   = 0x07,
        ObjectUpdateChannel8   = 0x08,
        ObjectUpdateChannel9   = 0x09,
        ObjectUpdateChannel10  = 0x0A,
        ObjectUpdateChannel11  = 0x0B,
        ObjectUpdateChannel12  = 0x0C,
        ObjectUpdateChannel13  = 0x0D,
        ObjectUpdateChannel14  = 0x0E,
        ObjectUpdateChannel15  = 0x0F,
        ObjectUpdateChannel16  = 0x10,
        ObjectUpdateChannel17  = 0x11,
        ObjectUpdateChannel18  = 0x12,
        ObjectUpdateChannel19  = 0x13,
        ObjectUpdateChannel20  = 0x14,
        ObjectUpdateChannel21  = 0x15,
        ObjectUpdateChannel22  = 0x16,
        ObjectUpdateChannel23  = 0x17,
        ClientUpdate           = 0x40, // Client unreliable that updates user inputs xyz, camera etc
        GroupUpdate            = 0x40, //?
        StatUpdate             = 0x42, //?
        BuffUpdate             = 0x43, //?
    }
}
