namespace ReturnHome.Server.EntityObject.Items
{
    //Byte indicating the location the item should equip too
    public enum ItemSlot : sbyte
    {
        NotEquipped  = -1,
        Head         = 1,
        Robe         = 2,
        Earring      = 3,
        Neck         = 4,
        Chest        = 5,
        Forearm      = 6,
        TwoForearm   = 7,
        Ring         = 8,
        Belt         = 9,
        Leg          = 10,
        Feet         = 11,
        Primary      = 12,
        Shield       = 13,
        Secondary    = 14,
        TwoHand      = 15,
        Bow          = 16,
        Thrown       = 17,
        Held         = 18,
        Hand         = 19,
        Fishing      = 20,
        Bait         = 21,
        WeaponCraft  = 22,
        ArmorCraft   = 23,
        JewlerCraft  = 24,
        Carpentry    = 25,
        Alchemy      = 26
    }
}
