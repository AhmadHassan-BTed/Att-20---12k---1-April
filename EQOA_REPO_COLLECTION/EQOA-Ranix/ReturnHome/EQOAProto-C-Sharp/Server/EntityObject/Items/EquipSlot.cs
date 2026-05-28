namespace ReturnHome.Server.EntityObject.Items
{
    //Byte indicating where the item is actually equipped.
    public enum EquipSlot : sbyte
    {
        NotEquipped = -1,
        Head        = 0,
        Hand        = 1,
        LeftEar     = 2,
        RightEar    = 3,
        Neck        = 4,
        Chest       = 5,
        RightBracelet = 6,
        LeftBracelet = 7,
        Bracers     = 8,
        LeftRing    = 9,
        RightRing   = 10,
        Belt        = 11,
        Leg         = 12,
        Feet        = 13,
        Primary     = 14,
        Secondary   = 15,
        TwoHand     = 16,
        Shield      = 17,
        Thrown      = 18,
        Held        = 19,
        Held2       = 20,
        Robe        = 21,
    }
}
