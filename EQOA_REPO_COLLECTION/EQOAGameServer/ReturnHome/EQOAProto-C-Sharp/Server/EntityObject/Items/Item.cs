using System.Text;
using ReturnHome.Utilities;

namespace ReturnHome.Server.EntityObject.Items
{
    public class Item
    {
        //Should these be public? How would we handle adding stats to character overall stats without 100 methods?
        public byte ClientIndex;
        public int StackLeft { get; set; }
        public int RemainingHP { get; set; }
        public int Charges { get; set; }
        public EquipSlot EquipLocation { get; set; }
        public ItemLocation Location { get; set; } //inventory, bank auction etc
        public byte ServerKey { get; set; } //Location in inventory, would location in List suffice for this?

        public int ID { get; set; }

        public ItemPattern Pattern { get; private set; }

        //Constructor object for armour and weapons
        //Alot of this could be managed by scripting as there is a huge portion that is static
        //Varis: int thisStacksLeft, int thisRemainingHP, int thisCharges, int thisEquipLocation, byte thisLocation, int thisInventoryNumber, int thisItemID<- use this in scripting to get right gear?
        public Item(int thisStacksLeft, int thisRemainingHP, int thisCharges, int thisEquipLocation, ItemLocation thisLocation, byte thisInventoryNumber, ItemPattern itemPattern, int itemID)
        {
            StackLeft = thisStacksLeft;
            Charges = thisCharges;
            Location = thisLocation;
            ServerKey = thisInventoryNumber;

            //Null this for outgoing packet
            RemainingHP = thisRemainingHP;
            EquipLocation = (EquipSlot)thisEquipLocation;
            Pattern = itemPattern;
            ID = itemID;
        }

        public Item AcquireItem(int qty)
        {
            Item item = (Item)MemberwiseClone();
            item.StackLeft = qty;

            return item;
        }

        public void DumpItem(ref BufferWriter writer, int key)
        {
            //Start adding attributes to list for this item
            writer.Write7BitEncodedInt64(StackLeft);
            writer.Write7BitEncodedInt64(RemainingHP);
            writer.Write7BitEncodedInt64(Charges);
            writer.Write7BitEncodedInt64((sbyte)EquipLocation);
            writer.Write7BitEncodedInt64((sbyte)Location);
            writer.Write(key);
            writer.Write7BitEncodedInt64(Pattern.ItemID);
            writer.Write7BitEncodedInt64((int)Pattern.ItemCost);
            writer.Write7BitEncodedInt64(Pattern.Unk1);
            writer.Write7BitEncodedInt64(Pattern.ItemIcon);
            writer.Write7BitEncodedInt64(Pattern.Unk2);
            writer.Write7BitEncodedInt64((sbyte)Pattern.itemSlot);
            writer.Write7BitEncodedInt64(Pattern.Unk3);
            writer.Write7BitEncodedInt64(Pattern.IsNoTrade ? 1 : 0);
            writer.Write7BitEncodedInt64(Pattern.IsNoRent ? 0 : 1);
            writer.Write7BitEncodedInt64(Pattern.Unk4);
            writer.Write7BitEncodedInt64(Pattern.Attacktype);
            writer.Write7BitEncodedInt64(Pattern.Weapondamage);
            writer.Write7BitEncodedInt64(Pattern.Unk5);
            writer.Write7BitEncodedInt64(Pattern.Levelreq);
            writer.Write7BitEncodedInt64(Pattern.Maxstack);
            writer.Write7BitEncodedInt64(Pattern.Maxhp);
            writer.Write7BitEncodedInt64(Pattern.Duration);
            writer.Write7BitEncodedInt64(Pattern.Classuse);
            writer.Write7BitEncodedInt64(Pattern.Raceuse);
            writer.Write7BitEncodedInt64(Pattern.Procanim);
            writer.Write7BitEncodedInt64(Pattern.IsLore ? 1 : 0);
            writer.Write7BitEncodedInt64(Pattern.Unk6);
            writer.Write7BitEncodedInt64(Pattern.IsCraft ? 1 : 0);
            writer.WriteString(Encoding.Unicode, Pattern.ItemName);
            writer.WriteString(Encoding.Unicode, Pattern.ItemDesc);
            writer.Write7BitEncodedInt64(Pattern.StatSize);
            for(int i = 0; i < Pattern.Stats.Length; ++i)
                if(Pattern.Stats[i] != 0)
                {
                    writer.Write7BitEncodedInt64(i);
                    writer.Write7BitEncodedInt64(Pattern.Stats[i]);
                }
        }
    }
}
