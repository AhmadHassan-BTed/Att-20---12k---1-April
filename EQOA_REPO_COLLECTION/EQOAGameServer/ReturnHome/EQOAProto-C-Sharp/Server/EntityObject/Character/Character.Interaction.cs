using System;
using ReturnHome.Utilities;
using ReturnHome.Server.Opcodes.Messages.Server;
using ReturnHome.Server.EntityObject.Items;
namespace ReturnHome.Server.EntityObject.Player
{
    public partial class Character
    {
        //The UpdateQuantity Method will auto run Remove item if stack <= 0
        public void DestroyItem(byte itemToDestroy, int quantityToDestroy) => Inventory.UpdateQuantity(itemToDestroy, quantityToDestroy);

        //Rearranges item inventory for player, move item 1 to slot of item2 and reorder
        public void ArrangeItem(byte itemSlot1, byte itemSlot2) => Inventory.ArrangeItems(itemSlot1, itemSlot2);

        public void AddItem(Item itemToBeAdded) => Inventory.AddItem(itemToBeAdded);

        //Method for withdrawing and depositing bank tunar
        public void BankTunar(uint targetNPC, uint giveOrTake, int transferAmount)
        {
            //deposit transaction
            if (giveOrTake == 0)
            {
                if (transferAmount > Inventory.Tunar)
                {
                    Logger.Err($"Player: {CharName} Account: {characterSession.AccountID} attempted to add {transferAmount} to bank when only {Inventory.Tunar} on hand");
                    return;
                }
                //Remove from Inventory
                Inventory.RemoveTunar(transferAmount);
                //Add to bank
                Bank.AddTunar(transferAmount);
            }
            //withdraw transaction
            else if (giveOrTake == 1)
            {
                if (transferAmount > Bank.Tunar)
                {
                    Logger.Err($"Player: {CharName} Account: {characterSession.AccountID} attempted to remove {transferAmount} from bank when only {Bank.Tunar}");
                    return;
                }
                //remove from bank
                Bank.RemoveTunar(transferAmount);

                //Add To inventory
                Inventory.AddTunar(transferAmount);
            }
        }

        public void TransferItem(byte giveOrTake, byte itemToTransfer, int qtyToTransfer)
        {
            //Deposit Item to bank
            if (giveOrTake == 0)
            {
                //Remove item from Inventory
                if (Inventory.TryRetrieveItem(itemToTransfer, out Item item, out byte clientIndex))
                {
                    Inventory.RemoveItem(itemToTransfer, true);
                    //unequip item
                    equippedGear.Remove(item);
                    //Deposit into bank
                    Bank.AddItem(item, false, true);
                }
            }
            //Pull from bank
            else if (giveOrTake == 1)
            {
                //Remove item from bank
                if (Bank.TryRetrieveItem(itemToTransfer, out Item item, out byte clientIndex))
                {
                    Bank.RemoveItem(itemToTransfer, true);
                    //Deposit into inventory
                    Inventory.AddItem(item, false, true);
                }
            }
        }

        //TODO: Flawed logic involved with stackable items and rearranging inventory, fix
        public void SellItem(byte itemSlot, int itemQty, uint targetNPC)
        {
            if (Inventory.TryRetrieveItem(itemSlot, out Item item, out byte index))
            {
                Inventory.UpdateQuantity(itemSlot, itemQty);
                //TODO: Flawed Tunar logic? Seem to be getting less then we spent back
                Inventory.AddTunar((int)(item.Pattern.Maxhp == 0 ? item.Pattern.ItemCost * itemQty : item.Pattern.ItemCost * (item.RemainingHP / item.Pattern.Maxhp) * itemQty));
            }
        }
    }
}
