namespace ReturnHome.Server.EntityObject.Items
{
    public class ItemPattern
    {
        public int ItemID { get; private set; }
        public uint ItemCost { get; private set; }
        public int Unk1 { get; private set; }
        public int ItemIcon { get; private set; }
        public int Unk2 { get; private set; }
        public ItemSlot itemSlot { get; private set; }
        public int Unk3 { get; private set; }
        public ItemFlags Flags { get; private set; }
        public int Unk4 { get; private set; }
        public int Attacktype { get; private set; }
        public int Weapondamage { get; private set; }
        public int Unk5 { get; private set; }
        public int Levelreq { get; private set; }
        public int Maxstack { get; private set; }
        public int Maxhp { get; private set; }
        public int Duration { get; private set; }
        public int Classuse { get; private set; }
        public int Raceuse { get; private set; }
        public int Procanim { get; private set; }
        public int Unk6 { get; private set; }
        public string ItemName { get; private set; }
        public string ItemDesc { get; private set; }
        public int Model { get; private set; }
        public uint Color { get; private set; }
        public int[] Stats;
        public int StatSize { get; private set; }

        public bool IsLore => (Flags & ItemFlags.Lore) == ItemFlags.Lore;
        public bool IsNoRent => (Flags & ItemFlags.NoRent) == ItemFlags.NoRent;
        public bool IsCraft => (Flags & ItemFlags.Craft) == ItemFlags.Craft;
        public bool IsNoTrade => (Flags & ItemFlags.NoTrade) == ItemFlags.NoTrade;

        public ItemPattern(int thisItemID, uint thisItemCost, int thisItemIcon, int thisItemSlot, int thisAttackType,
                    int thisWeaponDamage, int thisMaxHP, int thisLevelreq, int thisMaxStack, string thisItemName, string thisItemDesc, int thisDuration, int thisClassuse,
                    int thisRaceuse, int thisProcanim, int[] stats, int model, uint color, ItemFlags flags)
        {
            ItemID = thisItemID;
            ItemCost = thisItemCost;
            ItemIcon = thisItemIcon;
            itemSlot = (ItemSlot)thisItemSlot;
            Attacktype = thisAttackType;
            Weapondamage = thisWeaponDamage;
            Maxhp = thisMaxHP;
            Levelreq = thisLevelreq;
            Maxstack = thisMaxStack;
            ItemName = thisItemName;
            ItemDesc = thisItemDesc;
            Duration = thisDuration;
            Classuse = thisClassuse;
            Raceuse = thisRaceuse;
            Procanim = thisProcanim;
            Model = model;
            Color = color;
            Flags = flags;

            Stats = stats;

            for (int i = 0; i < Stats.Length; ++i)
                if (Stats[i] != 0)
                    StatSize += 1;
        }
    }
}
