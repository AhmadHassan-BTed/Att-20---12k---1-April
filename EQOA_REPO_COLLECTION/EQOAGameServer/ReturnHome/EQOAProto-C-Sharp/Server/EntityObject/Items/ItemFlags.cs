using System;

namespace ReturnHome.Server.EntityObject.Items
{
    [Flags]
    public enum ItemFlags : byte
    {
        NoTrade = 0x01,
        Lore = 0x02,
        NoRent = 0x04,
        Craft = 0x08
    }
}
