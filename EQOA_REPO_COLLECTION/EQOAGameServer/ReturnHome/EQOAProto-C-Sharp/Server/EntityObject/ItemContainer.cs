using System.Collections.Generic;
using ReturnHome.Server.EntityObject.Items;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.Opcodes;
using ReturnHome.Server.Opcodes.Messages.Server;
using ReturnHome.Database.SQL;
using ReturnHome.Server.Managers;
using System;
using ReturnHome.Server.Opcodes.Chat;
using Microsoft.EntityFrameworkCore.Metadata.Internal;

namespace ReturnHome.Server.EntityObject
{
    //TODO: Rewrite ItemContainer class to handle sending messages if a player
    public class ItemContainer
    {
        private Entity _e;
        private List<ClientItemWrapper> _itemContainer;
        private byte _counter = 0;
        private int _tunar;

        //Indicates if inventory or bank
        public bool Inventory;

        private ItemLocation _type;

        public int Tunar
        {
            get { return _tunar; }
            private set
            {
                _tunar = value;
                if (_e.isPlayer)
                    ServerUpdateTunar.UpdateTunar(((Character)_e).characterSession, Inventory ? GameOpcode.PlayerTunar : GameOpcode.ConfirmBankTunar, Tunar);
            }
        }

        public int Count => _itemContainer.Count;

        public List<ClientItemWrapper> itemContainer => _itemContainer;

        public ItemContainer(int tunar, Entity e, bool inventory = true)
        {
            _e = e;
            _tunar = tunar;
            _itemContainer = new();
            Inventory = inventory;
            if (inventory)
                _type = ItemLocation.Inventory;
            else
                _type = ItemLocation.Bank;
        }

        public void AddTunar(int tunar) => Tunar += tunar;

        public void RemoveTunar(int tunar) => Tunar -= tunar;

        public bool Exists(byte key)
        {
            for (int i = 0; i < _itemContainer.Count; ++i)
                if (_itemContainer[i].key == key)
                    return true;
            return false;
        }

        //TODO: Need to add check's in place for item's like "Lore", where we can only have 1 in our inventory at a time.
        public bool AddItem(Item itemToBeAdded, bool loot = false, bool transfer = false)
        {
            if (_itemContainer.Count < 40)
            {
                //TODO fix item stacking, specifically stacking of overflow stacks
                //if(itemToBeAdded.Lore == true)
                if (_e.isPlayer)
                {
                    //Make sure this isn't a freshly created item, if it is, update the itemID before adding to inventory and save it to the players inventory
                    if (itemToBeAdded.ID == 0)
                    {
                        bool itemExist = false;
                        foreach (ClientItemWrapper item in ((Character)_e).characterSession.MyCharacter.Inventory.itemContainer)
                        {
                            if (item.item.Pattern.ItemID == itemToBeAdded.Pattern.ItemID)
                            {
                                if ((itemToBeAdded.StackLeft + item.item.StackLeft) <= itemToBeAdded.Pattern.Maxstack)
                                {
                                    //Update internally on an already held item's stack count, we should be adding here
                                    UpdateQuantity(item.key, itemToBeAdded.StackLeft, loot, true);
                                    itemToBeAdded.StackLeft = 0;
                                }
                                itemExist = true;
                                /* TODO: Need to figure out how to properly showing multiple stacks of items client side, for time being we only allow 1 maxed stack of any item
                                else if(item.item.StackLeft != item.item.Pattern.Maxstack)
                                {
                                    int qtyToAdd = itemToBeAdded.Pattern.Maxstack - item.item.StackLeft;
                                    Console.WriteLine($"Would be overflow stack, adding {qtyToAdd} to existing stack for a total of {qtyToAdd + itemToBeAdded.StackLeft}.");

                                    //Update internally on an already held item's stack count, we should be adding here
                                    UpdateQuantity(item.key, qtyToAdd, true);

                                    //Subject the quantity added to currently held stack from new item
                                    itemToBeAdded.StackLeft -= qtyToAdd;
                                }*/
                            }
                            //If at the end of this loop the new items stackleft is 0, let it GC and return
                            if (itemToBeAdded.StackLeft == 0)
                                return true;
                        }

                        if (itemExist)
                        {
                            //Send a message to client saying you cannot buy/loot anymore of this item due to max stack.
                            ChatMessage.ClientErrorMessage(((Character)_e).characterSession, $"You cannot buy/loot anymore of this item due to max stack");
                            ServerInventoryFull.InventoryFull(((Character)_e).characterSession);
                            return false;
                        }
                        //Finished loop, if we still have an item with stacks, give it an ID
                        if (itemToBeAdded.StackLeft > 0)
                        {
                            itemToBeAdded.ID = ItemManager.nextItemID;
                            ItemManager.nextItemID++;
                        }
                    }
                }
                _itemContainer.Add(new ClientItemWrapper(itemToBeAdded, _counter++));
                itemToBeAdded.Location = _type;
                itemToBeAdded.ClientIndex = (byte)_itemContainer.Count;
                if (_e.isPlayer)
                {
                    CharacterSQL sql = new CharacterSQL();
                    if (((Character)_e).characterSession.inGame && !transfer)
                        sql.AddPlayerItem(((Character)_e).characterSession.MyCharacter, itemToBeAdded);

                    else if(((Character)_e).characterSession.inGame && transfer)
                        sql.UpdatePlayerItem(((Character)_e).characterSession.MyCharacter, itemToBeAdded);

                    if (((Character)_e).characterSession.inGame && !loot)
                        ServerAddItemOrQuantity.AddItemOrQuantity(((Character)_e).characterSession, Inventory ? GameOpcode.AddInvItem : GameOpcode.AddBankItem, itemToBeAdded, itemToBeAdded.StackLeft);
                    sql.CloseConnection();
                }
                return true;
            }
            if (_e.isPlayer)
                ServerInventoryFull.InventoryFull(((Character)_e).characterSession);

            return false;
        }

        public bool RemoveItem(byte key, bool transfer = false)
        {
            for (int i = 0; i < _itemContainer.Count; ++i)
            {
                if (_itemContainer[i].key == key)
                {
                    Item item = _itemContainer[i].item;
                    if (item.EquipLocation != EquipSlot.NotEquipped)
                        _e.equippedGear.Remove(item);

                    _itemContainer.RemoveAt(i);
                    if (_e.isPlayer)
                        if (((Character)_e).characterSession.inGame)
                        {
                            if (!transfer)
                            {
                                CharacterSQL sql = new CharacterSQL();
                                sql.DeletePlayerItem(item.ID);//Should this be moved to RemoveItem? Other classes/Methods call remove Item outside of UpdateQuantity
                                sql.CloseConnection();
                            }

                            ServerRemoveItemOrUpdateQuantity.RemoveItemOrUpdateQuantity(((Character)_e).characterSession, Inventory ? GameOpcode.RemoveInvItem : GameOpcode.RemoveBankItem, item.StackLeft, (byte)i);
                        }
                    return true;
                }
            }

            return false;
        }

        /// <summary>
        /// <paramref name="key"/> is specific to client unique item keys.
        /// Ensure <paramref name="quantity"/> is negative if removing stacks/item, or positive if adding.
        /// </summary>
        /// 
        /// //TODO: Update references to this method to make sure they are appropriately passing a positive or negative value
        public void UpdateQuantity(byte key, int quantity, bool loot = false, bool Add = false)
        {
            for (int i = 0; i < _itemContainer.Count; ++i)
            {
                if (_itemContainer[i].key == key)
                {
                    if (!Add && ((_itemContainer[i].item.StackLeft - quantity) == 0))
                        RemoveItem(key);

                    else
                    {
                        if (_e.isPlayer)
                            if (((Character)_e).characterSession.inGame)
                            {
                                CharacterSQL sql = new CharacterSQL();
                                _itemContainer[i].item.StackLeft += Add ? quantity : -1 * quantity;
                                sql.UpdatePlayerItem(((Character)_e).characterSession.MyCharacter, _itemContainer[i].item);
                                sql.CloseConnection();
                                if (!loot)
                                    //If adding to a stack, we need to * -1, otherwise left the value go through as is for subtraction
                                    ServerRemoveItemOrUpdateQuantity.RemoveItemOrUpdateQuantity(((Character)_e).characterSession, Inventory ? GameOpcode.RemoveInvItem : GameOpcode.RemoveBankItem, Add ? -1 * quantity : quantity, (byte)i);
                            }
                    }
                }
            }
        }

        public bool ArrangeItems(byte key1, byte key2)
        {
            //If both item's exist...
            for (byte i = 0; i < _itemContainer.Count; ++i)
            {
                //Get both client index's
                if (_itemContainer[i].key == key1)
                {
                    for (byte j = 0; j < _itemContainer.Count; ++j)
                    {
                        if (_itemContainer[j].key == key2)
                        {
                            ClientItemWrapper temp = _itemContainer[i];
                            _itemContainer.RemoveAt(i);
                            _itemContainer.Insert(j, temp);

                            if (_e.isPlayer)
                                if (((Character)_e).characterSession.inGame)
                                    ServerInventoryItemArrange.InventoryItemArrange(((Character)_e).characterSession, i, j);

                            return true;
                        }
                    }
                    //if we found one item, but not the other break out and return false?
                    break;
                }
            }

            return false;
        }

        //Retrieve's our object reference
        public bool TryRetrieveItem(byte key, out Item item, out byte index)
        {
            for (byte i = 0; i < _itemContainer.Count; ++i)
            {
                if (_itemContainer[i].key == key)
                {
                    item = _itemContainer[i].item;
                    index = i;
                    return true;
                }
            }

            item = default;
            index = 0xFF;
            return false;
        }

        //TODO: Figure out how pulling out items should work. Should we pull the item wrapper out with any item so we can get the relevant key if needed?
        public bool TryRetrieveItem(byte key, out ClientItemWrapper item, out byte index)
        {
            for (byte i = 0; i < _itemContainer.Count; ++i)
            {
                if (_itemContainer[i].key == key)
                {
                    item = _itemContainer[i];
                    index = i;
                    return true;
                }
            }

            item = default;
            index = 0xFF;
            return false;
        }
    }
}
