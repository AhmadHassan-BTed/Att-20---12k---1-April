using System;

namespace ReturnHome.Server.Network
{
    //Contains bundle header flags along with BundleTypeFlags
    [Flags]
    public enum SegmentHeaderFlags : uint  
    {
        NewInstance             = 0x00080000,
        HasInstance             = 0x00002000,
        IsRemote                = 0x00000800,
        ResetConnection         = 0x00010000,
        data_0x40000            = 0x00040000,
    }
}
