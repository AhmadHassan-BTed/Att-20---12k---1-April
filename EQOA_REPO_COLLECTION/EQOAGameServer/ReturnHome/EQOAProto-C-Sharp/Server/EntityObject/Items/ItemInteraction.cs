using System;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.Opcodes.Messages.Server;
using ReturnHome.Utilities;

namespace ReturnHome.Server.EntityObject.Items
{
    public static class ItemInteraction
    {
        //Needs work here for equipping/unequipping gear/using items and checks against it
        public static bool EquipItem(Entity e, Item item, byte index)
        {
            if (e.isPlayer)
            {
                if ((((item.Pattern.Classuse >> (byte)e.EntityClass) & 1) == 1) && (((item.Pattern.Raceuse >> (byte)e.EntityRace) & 1) == 1) && e.Level >= item.Pattern.Levelreq)
                {
                    e.equippedGear.Add(item);
                    if (e.isPlayer)
                        ServerEquipItem.ProcessServerEquipItem(((Character)e).characterSession, item, index);
                    return true;
                }
                return false;
            }
            //TODO: NPC's should be able to equip with right conditions
            return false;
        }

        public static bool UnequipItem(Entity e, Item i)
        {
            if (e.equippedGear.Exists(i))
            {
                EquipSlot temp = e.equippedGear.Remove(i);
                if(e.isPlayer)
                    ServerUnequipItem.ProcessServerUnequipItem(((Character)e).characterSession, (byte)temp);
                return true;
            }

            return false;
        }

        public static void InteractItem(Entity e, uint key, byte interactionType)
        {
            if (interactionType != 0)
            {
                Logger.Err("Received a non-zero interaction type!!!! Look into this");
                return;
            }

            e.Inventory.TryRetrieveItem((byte)key, out Item item, out byte index);

            //unequip item if true
            if (item.EquipLocation != EquipSlot.NotEquipped && item.Pattern.itemSlot != ItemSlot.NotEquipped)
                UnequipItem(e, item);

            //Equip the item if true
            else if (item.EquipLocation == EquipSlot.NotEquipped && item.Pattern.itemSlot != ItemSlot.NotEquipped)
                EquipItem(e, item, index);

            //If we are then interacting with an item that is not equipable, must be a consumable item?
            else
                Console.WriteLine("Interacting with a consumable item?");
        }
    }
}
