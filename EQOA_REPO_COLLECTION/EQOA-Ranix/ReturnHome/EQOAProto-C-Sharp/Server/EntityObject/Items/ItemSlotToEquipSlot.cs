using System.Collections.Generic;
namespace ReturnHome.Server.EntityObject.Items
{
    public class ItemSlotToEquipSlot
    {
        public static Dictionary<ItemSlot, EquipSlot> ItemSlotToEquipSlots = new Dictionary<ItemSlot, EquipSlot>()
        {
            { ItemSlot.NotEquipped, EquipSlot.NotEquipped },
            { ItemSlot.Head, EquipSlot.Head },
            { ItemSlot.Robe, EquipSlot.Robe },
            { ItemSlot.Earring, EquipSlot.RightEar},
            { ItemSlot.Neck, EquipSlot.Neck },
            { ItemSlot.Chest, EquipSlot.Chest },
            { ItemSlot.Forearm, EquipSlot.RightBracelet},
            { ItemSlot.TwoForearm, EquipSlot.Bracers },
            { ItemSlot.Ring, EquipSlot.RightRing },
            { ItemSlot.Belt, EquipSlot.Belt },
            { ItemSlot.Leg, EquipSlot.Leg },
            { ItemSlot.Feet, EquipSlot.Feet },
            { ItemSlot.Primary, EquipSlot.Primary },
            { ItemSlot.Shield, EquipSlot.Shield },
            { ItemSlot.Secondary, EquipSlot.Secondary },
            { ItemSlot.TwoHand, EquipSlot.TwoHand },
            { ItemSlot.Bow, EquipSlot.Thrown },
            { ItemSlot.Thrown, EquipSlot.Thrown },
            { ItemSlot.Held, EquipSlot.Held },
            { ItemSlot.Fishing, EquipSlot.Primary }, //Is this technically correct?
            { ItemSlot.Bait, EquipSlot.Secondary }, //Is this technically correct?
        };
    }
}
