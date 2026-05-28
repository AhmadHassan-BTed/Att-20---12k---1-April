using System;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.Opcodes.Messages.Server;

namespace ReturnHome.Server.EntityObject.Items
{
    public static class ItemInteraction
    {
        //Needs work here for equipping/unequipping gear/using items and checks against it
        public static bool EquipItem(Entity e, Item i)
        {
            if (e.isPlayer)
            {
                if ((((i.Classuse >> (byte)e.EntityClass) & 1) == 1) && (((i.Raceuse >> (byte)e.EntityRace) & 1) == 1) && e.Level >= i.Levelreq)
                {
                    e.equippedGear.Add(i);
                    if (e.isPlayer)
                        ServerEquipItem.ProcessServerEquipItem(((Character)e).characterSession, i);
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

        public static void InteractItem(Entity e, uint index, byte interactionType)
        {
            if (interactionType != 0)
            {
                Console.WriteLine("Received a non-zero interaction type!!!! Look into this");
                return;
            }

            e.Inventory.RetrieveItem((byte)index, out Item i);

            //unequip item if true
            if (i.EquipLocation != EquipSlot.NotEquipped && i.itemSlot != ItemSlot.NotEquipped)
                UnequipItem(e, i);

            //Equip the item if true
            else if (i.EquipLocation == EquipSlot.NotEquipped && i.itemSlot != ItemSlot.NotEquipped)
                EquipItem(e, i);

            //If we are then interacting with an item that is not equipable, must be a consumable item?
            else
                Console.WriteLine("Interacting with a consumable item?");
        }
    }
}
