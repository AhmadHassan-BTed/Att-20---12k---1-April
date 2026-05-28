using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using ReturnHome.Server.EntityObject.Stats;

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
        public byte Location { get; set; } //inventory, bank auction etc
        public int ServerKey { get; set; } //Location in inventory, would location in List suffice for this?

        public int ItemID { get; private set; }
        public uint ItemCost { get; private set; }
        public int Unk1 { get; private set; }
        public int ItemIcon { get; private set; }
        public int Unk2 { get; private set; }
        public ItemSlot itemSlot { get; private set; }
        public int Unk3 { get; private set; }
        public int Trade { get; private set; }
        public int Rent { get; private set; }
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
        public int Lore { get; private set; }
        public int Unk6 { get; private set; }
        public int Craft { get; private set; }
        public string ItemName { get; private set; }
        public string ItemDesc { get; private set; }
        public int Model { get; private set; }
        public uint Color { get; private set; }
        

        //Gear stats
        public Dictionary<StatModifiers, int> Stats = new Dictionary<StatModifiers, int>();

        //default constructor
        public Item()
        { }

        //This will instantiate an inventory object
        //Should we be able to instantiate a normal item and gear seperately? Seems the better choice
        //This is to instantiate 
        public Item(int thisStacksLeft, int thisCharges, byte thisLocation, byte thisInventoryNumber, int thisItemID, uint thisItemCost, int thisItemIcon, int thisTrade, int thisRent, int thisCraft, int thisLore, int thisLevelreq, int thisMaxStack, string thisItemName, string thisItemDesc,)

        {
            StackLeft = thisStacksLeft;
            Charges = thisCharges;
            Location = thisLocation;
            ServerKey = thisInventoryNumber;
            ItemID = thisItemID;
            ItemCost = thisItemCost;
            ItemIcon = thisItemIcon;
            Trade = thisTrade;
            Rent = thisRent;
            Craft = thisCraft;
            Lore = thisLore;
            Levelreq = thisLevelreq;
            Maxstack = thisMaxStack;
            ItemName = thisItemName;
            ItemDesc = thisItemDesc;
            

        }

        //Constructor object for armour and weapons
        //Alot of this could be managed by scripting as there is a huge portion that is static
        //Varis: int thisStacksLeft, int thisRemainingHP, int thisCharges, int thisEquipLocation, byte thisLocation, int thisInventoryNumber, int thisItemID<- use this in scripting to get right gear?
        public Item(int thisStacksLeft, int thisRemainingHP, int thisCharges, int thisEquipLocation, byte thisLocation, byte thisInventoryNumber, int thisItemID, uint thisItemCost, int thisItemIcon, int thisItemSlot, int thisAttackType,
                    int thisWeaponDamage, int thisMaxHP, int thisTrade, int thisRent, int thisCraft, int thisLore, int thisLevelreq, int thisMaxStack, string thisItemName, string thisItemDesc, int thisDuration, int thisClassuse,
                    int thisRaceuse, int thisProcanim, List<KeyValuePair<StatModifiers, int>> temp, int model, uint color)
        {
            StackLeft = thisStacksLeft;
            Charges = thisCharges;
            Location = thisLocation;
            ServerKey = thisInventoryNumber;
            ItemID = thisItemID;
            ItemCost = thisItemCost;
            ItemIcon = thisItemIcon;
            Trade = thisTrade;
            Rent = thisRent;
            Craft = thisCraft;
            Lore = thisLore;
            Levelreq = thisLevelreq;
            Maxstack = thisMaxStack;
            ItemName = thisItemName;
            ItemDesc = thisItemDesc;
            

            //Null this for outgoing packet
            RemainingHP = thisRemainingHP;
            EquipLocation = (EquipSlot)thisEquipLocation;
            itemSlot = (ItemSlot)thisItemSlot;
            Attacktype = thisAttackType;
            Weapondamage = thisWeaponDamage;
            Maxhp = thisMaxHP;
            Duration = thisDuration;
            Classuse = thisClassuse;
            Raceuse = thisRaceuse;
            Procanim = thisProcanim;

            Color = color;
            Model = model;

            foreach(KeyValuePair<StatModifiers, int> t in temp)
            {
                if(t.Value == 0)
                    continue;
                else
                    Stats.Add(t.Key, t.Value);
            }
        }

        public Item AcquireItem(int qty)
        {
            Item item = (Item)MemberwiseClone();
            item.StackLeft = qty;
            return item;
        }

        public void DumpItem(ref BufferWriter writer)
        {
            //Start adding attributes to list for this item
            writer.Write7BitEncodedInt64(StackLeft);
            writer.Write7BitEncodedInt64(RemainingHP);
            writer.Write7BitEncodedInt64(Charges);
            writer.Write7BitEncodedInt64((sbyte)EquipLocation);
            writer.Write(Location);
            writer.Write(ServerKey);
            writer.Write7BitEncodedInt64(ItemID);
            writer.Write7BitEncodedUInt64(ItemCost);
            writer.Write7BitEncodedInt64(Unk1);
            writer.Write7BitEncodedInt64(ItemIcon);
            writer.Write7BitEncodedInt64(Unk2);
            writer.Write7BitEncodedInt64((sbyte)itemSlot);
            writer.Write7BitEncodedInt64(Unk3);
            writer.Write7BitEncodedInt64(Trade);
            writer.Write7BitEncodedInt64(Rent);
            writer.Write7BitEncodedInt64(Unk4);
            writer.Write7BitEncodedInt64(Attacktype);
            writer.Write7BitEncodedInt64(Weapondamage);
            writer.Write7BitEncodedInt64(Unk5);
            writer.Write7BitEncodedInt64(Levelreq);
            writer.Write7BitEncodedInt64(Maxstack);
            writer.Write7BitEncodedInt64(Maxhp);
            writer.Write7BitEncodedInt64(Duration);
            writer.Write7BitEncodedInt64(Classuse);
            writer.Write7BitEncodedInt64(Raceuse);
            writer.Write7BitEncodedInt64(Procanim);
            writer.Write7BitEncodedInt64(Lore);
            writer.Write7BitEncodedInt64(Unk6);
            writer.Write7BitEncodedInt64(Craft);
            writer.WriteString(Encoding.Unicode, ItemName);
            writer.WriteString(Encoding.Unicode, ItemDesc);
            PullStats(ref writer);
        }

        private void PullStats(ref BufferWriter writer)
        {
            writer.Write7BitEncodedInt64((byte)Stats.Count);

            foreach (KeyValuePair<StatModifiers, int> t in Stats)
            {
                if (t.Value == 0)
                    continue;

                writer.Write7BitEncodedInt64((byte)t.Key);
                writer.Write7BitEncodedInt64(t.Value);
            }
        }
    }
}
