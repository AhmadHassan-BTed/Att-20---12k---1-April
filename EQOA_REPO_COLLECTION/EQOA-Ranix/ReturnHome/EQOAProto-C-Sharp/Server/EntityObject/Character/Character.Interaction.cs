// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ReturnHome.Server.Opcodes;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;
using ReturnHome.Server.Opcodes.Messages.Server;
using ReturnHome.Server.EntityObject.Items;

namespace ReturnHome.Server.EntityObject.Player
{
    public partial class Character
    {

        public void DestroyItem(byte itemToDestroy, int quantityToDestroy)
        {
            //If this returns true, use quantityToDestroy
            if(Inventory.UpdateQuantity(itemToDestroy, quantityToDestroy, out Item item))
            {
                if (item.StackLeft <= 0)
                {
                    if (Inventory.RemoveItem(itemToDestroy, out Item item2, out byte clientIndex2))
                    {
                        ServerRemoveInventoryItemQuantity.RemoveInventoryItemQuantity(characterSession, quantityToDestroy, clientIndex2);
                    }
                    return;
                }

                ServerRemoveInventoryItemQuantity.RemoveInventoryItemQuantity(characterSession, quantityToDestroy, item.ClientIndex);
            }
        }

        //Rearranges item inventory for player, move item 1 to slot of item2 and reorder
        public void ArrangeItem(byte itemSlot1, byte itemSlot2)
        {
            if (Inventory.ArrangeItems(itemSlot1, itemSlot2, out byte clientItem1, out byte clientItem2))
            {
                ServerInventoryItemArrange.InventoryItemArrange(characterSession, clientItem1, clientItem2);
            }
        }

        public void AddItem(Item itemToBeAdded)
        {
            if (Inventory.AddItem(itemToBeAdded))
            {
                Console.WriteLine($"{itemToBeAdded.ItemName} added.");
            }
        }

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

            ServerUpdateBankTunar.UpdateBankTunar(characterSession, Bank.Tunar);
            ServerUpdatePlayerTunar.UpdatePlayerTunar(characterSession, Inventory.Tunar);
        }

        public void TransferItem(byte giveOrTake, byte itemToTransfer, int qtyToTransfer)
        {
            //Deposit Item to bank
            if (giveOrTake == 0)
            {
                //Remove item from Inventory
                if (Inventory.RemoveItem(itemToTransfer, out Item item, out byte clientIndex))
                {
                    ServerRemoveInventoryItemQuantity.RemoveInventoryItemQuantity(characterSession, qtyToTransfer, clientIndex);

                    //unequip item
                    equippedGear.Remove(item);

                    //Deposit into bank
                    Bank.AddItem(item);

                    ServerAddBankItemQuantity.AddBankItemQuantity(characterSession, item);
                }
            }

            //Pull from bank
            else if (giveOrTake == 1)
            {
                //Remove item from bank
                if (Bank.RemoveItem(itemToTransfer, out Item item, out byte clientIndex))
                {
                    ServerRemoveBankItemQuantity.RemoveBankItemQuantity(characterSession, item, clientIndex);

                    //Deposit into inventory
                    Inventory.AddItem(item);

                    ServerAddInventoryItemQuantity.AddInventoryItemQuantity(characterSession, item);
                }
            }
        }

        //TODO: Flawed logic involved with stackable items and rearranging inventory, fix
        public void SellItem(byte itemSlot, int itemQty, uint targetNPC)
        {
            if(Inventory.Exists(itemSlot))
            {
                if(Inventory.UpdateQuantity(itemSlot, itemQty, out Item item))
                {
                    //TODO: Flawed Tunar logic? Seem to be getting less then we spent back
                    Inventory.AddTunar((int)(item.Maxhp == 0 ? item.ItemCost * itemQty : item.ItemCost * (item.RemainingHP / item.Maxhp) * itemQty));

                    ServerUpdatePlayerTunar.UpdatePlayerTunar(characterSession, Inventory.Tunar);

                    if (item.StackLeft <= 0)
                    {
                        if (Inventory.RemoveItem(itemSlot, out Item item2, out byte clientIndex))
                            ServerRemoveInventoryItemQuantity.RemoveInventoryItemQuantity(characterSession, itemQty, clientIndex);

                        return;
                    }

                    ServerRemoveInventoryItemQuantity.RemoveInventoryItemQuantity(characterSession, itemQty, item.ClientIndex);
                }
            }
        }

        // This is buying item code but should be close to -- if not enough money, else remove money.
         
        public void RepairItem(byte itemSlot, int itemQty, uint targetNPC)
        {
            if (Inventory.Exists(itemSlot))
            {
                if (Inventory.UpdateQuantity(itemSlot, itemQty, out Item item))
                
                    {
                        if (item.RemainingHP < item.Maxhp)
                        {
                            if (Inventory.Tunar < (item.ItemCost * (1 - (item.RemainingHP / item.Maxhp))))
                            {
                                ChatMessage.DistributeSpecificMessageAndColor(((Character)this).characterSession, $"You can't afford that.", new byte[] { 0xFF, 0x00, 0x00, 0x00 });
                            }
                            else
                            {
                                Inventory.RemoveTunar((int)(item.ItemCost * (1 - (item.RemainingHP / item.Maxhp))));

                                //Adjust player tunar
                                ServerUpdatePlayerTunar.UpdatePlayerTunar(((Character)this).characterSession, Inventory.Tunar);

                                item.RemainingHP = item.Maxhp;

                                return;
                            }
                        }
                    }
                
            }
            
        }

        public static void AddQuestLog(Session session, uint questNumber, string questText)
        {
            ServerAddQuestLog.AddQuestLog(session, questNumber, questText);
        }

        public static void DeleteQuest(Session session, byte questNumber)
        {
            ServerDeleteQuest.DeleteQuest(session, questNumber);
        }
    }
}
