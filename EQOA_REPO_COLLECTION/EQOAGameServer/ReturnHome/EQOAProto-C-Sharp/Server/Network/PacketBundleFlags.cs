using System;

namespace ReturnHome.Server.Network
{
	//Contains bundle header flags along with BundleTypeFlags
    public enum PacketBundleFlags  : byte
    {
        NewProcessMessages        = 0x00,
		RDPReport                 = 0x03,
		ProcessUnreliableAcks     = 0x10,
        UnknownSingleByte         = 0x08,
        ProcessMessages           = 0x20,
		ProcessAck                = 0x40
    }
}
