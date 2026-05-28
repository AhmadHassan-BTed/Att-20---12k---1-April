using ReturnHome.Server.EntityObject.Stats;

using System;
using System.Collections.Generic;


namespace ReturnHome.Server.EntityObject.Items
{
    public sealed class EquippedGear
    {
        private Entity _entity;
        //Used to translate from Itemslot to EquipSlot
        private readonly static Dictionary<ItemSlot, EquipSlot> ItemSlotToEquipSlots = new Dictionary<ItemSlot, EquipSlot>()
        {
            { ItemSlot.NotEquipped, EquipSlot.NotEquipped },
            { ItemSlot.Hand, EquipSlot.Hand },
            { ItemSlot.Head, EquipSlot.Head },
            { ItemSlot.Robe, EquipSlot.Robe },
            { ItemSlot.Earring, EquipSlot.RightEar},
            { ItemSlot.Neck, EquipSlot.Neck },
            { ItemSlot.Chest, EquipSlot.Chest },
            { ItemSlot.Forearm, EquipSlot.RightBracelet},
            { ItemSlot.TwoForearm, EquipSlot.Bracers },
            { ItemSlot.Ring, EquipSlot.RightRing },
            { ItemSlot.Belt, EquipSlot.Belt },
            { ItemSlot.Leg, EquipSlot.Leg },
            { ItemSlot.Feet, EquipSlot.Feet },
            { ItemSlot.Primary, EquipSlot.Primary },
            { ItemSlot.Shield, EquipSlot.Shield },
            { ItemSlot.Secondary, EquipSlot.Secondary },
            { ItemSlot.TwoHand, EquipSlot.TwoHand },
            { ItemSlot.Bow, EquipSlot.Thrown },
            { ItemSlot.Thrown, EquipSlot.Thrown },
            { ItemSlot.Held, EquipSlot.Held },
            { ItemSlot.Fishing, EquipSlot.Primary }, //Is this technically correct?
            { ItemSlot.Bait, EquipSlot.Secondary }, //Is this technically correct?
        };

        private Dictionary<EquipSlot, Item> _equippedItems;

        public EquippedGear(Entity entity)
        {
            _entity = entity;
            _equippedItems = new();

            foreach (EquipSlot e in Enum.GetValues(typeof(EquipSlot)))
                _equippedItems.Add(e, null);
        }

        //Should we find the appropriate slot first, then work from there?
        public void Add(Item item)
        {
            //Find appropriate slot
            EquipSlot equipSlot = ItemSlotToEquipSlots[item.itemSlot];

            //Add To Dictionary/Equip item
            EquipItem(equipSlot, item);
        }

        public EquipSlot Remove(Item item) => UnequipItem(item.EquipLocation, item);

        public bool Exists(Item i)
        {
            if (i == _equippedItems[i.EquipLocation])
                return true;
            return false;
        }
        private void AddStats(Item item)
        {
            try
            {
                foreach (KeyValuePair<StatModifiers, int> s in item.Stats)
                    _entity.CurrentStats.Add(s.Key, s.Value);
            }

            catch
            {
                Console.WriteLine($"Issue adding stats for {_entity.CharName}");
            }
        }



        private void RemoveStats(Item item)
        {
            try
            {
                foreach (KeyValuePair<StatModifiers, int> s in item.Stats)
                    _entity.CurrentStats.Remove(s.Key, s.Value);
            }

            catch
            {
                Console.WriteLine($"Issue removing stats for {_entity.CharName}");
            }
        }

        private EquipSlot UnequipItem(EquipSlot equipSlot, Item item)
        {
            switch (equipSlot)
            {
                case EquipSlot.Robe:
                    if (_equippedItems[equipSlot] != null)
                        RemoveStats(_equippedItems[equipSlot]);
                    _equippedItems[equipSlot] = null;
                    _entity.Robe = -1;
                    _entity.RobeColor = 0xFFFFFFFF;
                    break;

                case EquipSlot.Head:
                    if (_equippedItems[equipSlot] != null)
                        RemoveStats(_equippedItems[equipSlot]);
                    _equippedItems[equipSlot] = null;
                    _entity.Helm = 0;
                    _entity.HelmColor = 0xFFFFFFFF;
                    break;

                case EquipSlot.Chest:
                    if (_equippedItems[equipSlot] != null)
                        RemoveStats(_equippedItems[equipSlot]);
                    _equippedItems[equipSlot] = null;
                    _entity.ChestColor = 0xFFFFFFFF;
                    _entity.Chest = 0;
                    break;

                case EquipSlot.Bracers:
                    if (_equippedItems[equipSlot] != null)
                        RemoveStats(_equippedItems[equipSlot]);
                    _equippedItems[equipSlot] = null;
                    _entity.Bracer = 0;
                    _entity.BracerColor = 0xFFFFFFFF;
                    break;

                //If bracers are equipped, remove them and stats, first bracelet is always right, left always changes.
                case EquipSlot.RightBracelet:
                case EquipSlot.LeftBracelet:
                case EquipSlot.RightRing:
                case EquipSlot.LeftRing:
                case EquipSlot.RightEar:
                case EquipSlot.LeftEar:
                    if (_equippedItems[equipSlot] != null)
                        RemoveStats(_equippedItems[equipSlot]);
                    _equippedItems[equipSlot] = null;
                    break;

                case EquipSlot.Hand:
                    if (_equippedItems[equipSlot] != null)
                        RemoveStats(_equippedItems[equipSlot]);
                    _equippedItems[equipSlot] = null;
                    _entity.Gloves = 0;
                    _entity.GloveColor = 0xFFFFFFFF;
                    break;

                case EquipSlot.Leg:
                    if (_equippedItems[equipSlot] != null)
                        RemoveStats(_equippedItems[equipSlot]);
                    _equippedItems[equipSlot] = null;
                    _entity.LegColor = 0xFFFFFFFF;
                    _entity.Legs = 0;
                    break;

                case EquipSlot.Feet:
                    if (_equippedItems[equipSlot] != null)
                        RemoveStats(_equippedItems[equipSlot]);
                    _equippedItems[equipSlot] = null;
                    _entity.Boots = 0;
                    _entity.BootsColor = 0xFFFFFFFF;
                    break;

                case EquipSlot.Primary:
                case EquipSlot.Held:
                case EquipSlot.TwoHand:
                case EquipSlot.Thrown:
                    if (_equippedItems[equipSlot] != null)
                        RemoveStats(_equippedItems[equipSlot]);
                    _equippedItems[equipSlot] = null;
                    _entity.Primary = 0;
                    break;


                case EquipSlot.Held2:
                case EquipSlot.Secondary:
                    if (_equippedItems[equipSlot] != null)
                        RemoveStats(_equippedItems[equipSlot]);
                    _equippedItems[equipSlot] = null;
                    _entity.Secondary = 0;
                    break;

                case EquipSlot.Shield:
                    if (_equippedItems[equipSlot] != null)
                        RemoveStats(_equippedItems[equipSlot]);
                    _equippedItems[equipSlot] = null;
                    _entity.Shield = 0;
                    break;
            }

            item.EquipLocation = EquipSlot.NotEquipped;
            return equipSlot;
        }

        private void EquipItem(EquipSlot equipSlot,Item item)
        {
            switch (equipSlot)
            {
                case EquipSlot.Robe:
                    if (_equippedItems[equipSlot] != null)
                        Remove(_equippedItems[equipSlot]);
                    _equippedItems[equipSlot] = item;
                    item.EquipLocation = equipSlot;
                    _entity.Robe = item.Model;
                    _entity.RobeColor = item.Color;
                    break;

                case EquipSlot.Head:
                    if (_equippedItems[equipSlot] != null)
                        Remove(_equippedItems[equipSlot]);
                    _equippedItems[equipSlot] = item;
                    item.EquipLocation = equipSlot;
                    _entity.Helm = (byte)item.Model;
                    _entity.HelmColor = item.Color;
                    break;

                case EquipSlot.Chest:
                    if (_equippedItems[equipSlot] != null)
                        Remove(_equippedItems[equipSlot]);
                    _equippedItems[equipSlot] = item;
                    item.EquipLocation = equipSlot;
                    _entity.ChestColor = item.Color;
                    _entity.Chest = (byte)item.Model;
                    break;

                case EquipSlot.Bracers:
                    //If any bracelet's are equipped, unequip them and remove stats
                    if (_equippedItems[EquipSlot.LeftBracelet] != null)
                    {
                        Remove(_equippedItems[EquipSlot.LeftBracelet]);
                        _equippedItems[EquipSlot.LeftBracelet] = null;
                    }

                    if (_equippedItems[EquipSlot.RightBracelet] != null)
                    {
                        Remove(_equippedItems[EquipSlot.RightBracelet]);
                        _equippedItems[EquipSlot.RightBracelet] = null;
                    }

                    _equippedItems[equipSlot] = item;
                    item.EquipLocation = equipSlot;
                    _entity.Bracer = (byte)item.Model;
                    _entity.BracerColor = item.Color;
                    break;

                //If bracers are equipped, remove them and stats, first bracelet is always right, left always changes.
                case EquipSlot.RightBracelet:
                case EquipSlot.LeftBracelet:
                    if (_equippedItems[EquipSlot.Bracers] != null)
                    {
                        Remove(_equippedItems[EquipSlot.Bracers]);
                        _equippedItems[EquipSlot.Bracers] = null;
                    }

                    if (_equippedItems[EquipSlot.RightBracelet] == null)
                    {
                        _equippedItems[EquipSlot.RightBracelet] = item;
                        item.EquipLocation = EquipSlot.RightBracelet;
                    }
                    else
                    {
                        if (_equippedItems[EquipSlot.LeftBracelet] != null)
                            Remove(_equippedItems[EquipSlot.LeftBracelet]);

                        _equippedItems[EquipSlot.LeftBracelet] = item;
                        item.EquipLocation = EquipSlot.LeftBracelet;
                    }

                    _entity.Bracer = 0;
                    _entity.BracerColor = 0;
                    break;

                case EquipSlot.Hand:
                    if (_equippedItems[equipSlot] != null)
                        Remove(_equippedItems[equipSlot]);
                    _equippedItems[equipSlot] = item;
                    item.EquipLocation = equipSlot;
                    _entity.Gloves = (byte)item.Model;
                    _entity.GloveColor = item.Color;
                    break;

                case EquipSlot.Leg:
                    if (_equippedItems[equipSlot] != null)
                        Remove(_equippedItems[equipSlot]);
                    _equippedItems[equipSlot] = item;
                    item.EquipLocation = equipSlot;
                    _entity.LegColor = item.Color;
                    _entity.Legs = (byte)item.Model;
                    break;

                case EquipSlot.Feet:
                    if (_equippedItems[equipSlot] != null)
                        Remove(_equippedItems[equipSlot]);
                    _equippedItems[equipSlot] = item;
                    item.EquipLocation = equipSlot;
                    _entity.Boots = (byte)item.Model;
                    _entity.BootsColor = item.Color;
                    break;

                case EquipSlot.RightRing:
                case EquipSlot.LeftRing:
                    if (_equippedItems[EquipSlot.RightRing] == null)
                    {
                        _equippedItems[EquipSlot.RightRing] = item;
                        item.EquipLocation = EquipSlot.RightRing;
                    }
                    else
                    {
                        if (_equippedItems[EquipSlot.LeftRing] != null)
                            Remove(_equippedItems[EquipSlot.LeftRing]);

                        _equippedItems[EquipSlot.LeftRing] = item;
                        item.EquipLocation = EquipSlot.LeftRing;
                    }
                    break;

                case EquipSlot.RightEar:
                case EquipSlot.LeftEar:
                    if (_equippedItems[EquipSlot.RightEar] == null)
                    {
                        _equippedItems[EquipSlot.RightEar] = item;
                        item.EquipLocation = EquipSlot.RightEar;
                    }
                    else
                    {
                        if (_equippedItems[EquipSlot.LeftEar] != null)
                            Remove(_equippedItems[EquipSlot.LeftEar]);

                        _equippedItems[EquipSlot.LeftEar] = item;
                        item.EquipLocation = EquipSlot.LeftEar;
                    }
                    break;

                case EquipSlot.Neck:
                case EquipSlot.Belt:
                    if (_equippedItems[equipSlot] != null)
                        Remove(_equippedItems[equipSlot]);
                    _equippedItems[equipSlot] = item;
                    item.EquipLocation = equipSlot;
                    break;

                case EquipSlot.Primary:
                case EquipSlot.Secondary:
                case EquipSlot.Shield:
                case EquipSlot.Held:
                case EquipSlot.Held2:
                case EquipSlot.TwoHand:
                case EquipSlot.Thrown:
                    EquipWeapons(equipSlot, item);
                    break;
            }

            AddStats(item);
        }

        private void EquipWeapons(EquipSlot equipSlot, Item item)
        {
            switch(equipSlot)
            {
                case EquipSlot.Primary:
                    //If any of the first 2 if's are equipped, unequip them and remove stats
                    if (_equippedItems[EquipSlot.TwoHand] != null)
                    {
                        Remove(_equippedItems[EquipSlot.TwoHand]);
                        _equippedItems[EquipSlot.TwoHand] = null;
                        _entity.Primary = 0;
                    }

                    if (_equippedItems[EquipSlot.Thrown] != null)
                    {
                        Remove(_equippedItems[EquipSlot.Thrown]);
                        _equippedItems[EquipSlot.Thrown] = null;
                        _entity.Primary = 0;
                    }

                    if (_equippedItems[EquipSlot.Held] != null)
                    {
                        Remove(_equippedItems[EquipSlot.Held]);
                        _equippedItems[EquipSlot.Held] = null;
                        _entity.Primary = 0;
                    }

                    if (_equippedItems[equipSlot] != null)
                        Remove(_equippedItems[equipSlot]);

                    _equippedItems[equipSlot] = item;
                    item.EquipLocation = equipSlot;
                    _entity.Primary = item.Model;
                    break;

                //If a primary is equipped, secondary is always in secondary
                case EquipSlot.Secondary:
                    //If any of the first 2 if's are equipped, unequip them and remove stats
                    if (_equippedItems[EquipSlot.TwoHand] != null)
                    {
                        Remove(_equippedItems[EquipSlot.TwoHand]);
                        _equippedItems[EquipSlot.TwoHand] = null;
                        _entity.Primary = 0;
                    }

                    if(_equippedItems[EquipSlot.Thrown] != null)
                    {
                        Remove(_equippedItems[EquipSlot.Thrown]);
                        _equippedItems[EquipSlot.Thrown] = null;
                        _entity.Primary = 0;
                    }

                    if (_equippedItems[EquipSlot.Primary] == null)
                    {
                        _equippedItems[EquipSlot.Primary] = item;
                        item.EquipLocation = EquipSlot.Primary;
                        _entity.Primary = item.Model;
                    }

                    else
                    {
                        if(_equippedItems[EquipSlot.Shield] != null)
                        {
                            Remove(_equippedItems[EquipSlot.Shield]);
                            _equippedItems[EquipSlot.Shield] = null;
                            _entity.Shield = 0;
                        }

                        if (_equippedItems[EquipSlot.Held2] != null)
                        {
                            Remove(_equippedItems[EquipSlot.Held2]);
                            _equippedItems[EquipSlot.Held2] = null;
                            _entity.Secondary = 0;
                        }

                        if (_equippedItems[EquipSlot.Secondary] != null)
                        {
                            Remove(_equippedItems[EquipSlot.Secondary]);
                            _equippedItems[EquipSlot.Secondary] = null;
                            _entity.Secondary = 0;
                        }

                        _equippedItems[equipSlot] = item;
                        item.EquipLocation = equipSlot;
                        _entity.Secondary = item.Model;
                    }
                    break;

                case EquipSlot.Shield:
                    if (_equippedItems[EquipSlot.TwoHand] != null)
                    {
                        Remove(_equippedItems[EquipSlot.TwoHand]);
                        _equippedItems[EquipSlot.TwoHand] = null;
                        _entity.Primary = 0;
                    }

                    if (_equippedItems[EquipSlot.Thrown] != null)
                    {
                        Remove(_equippedItems[EquipSlot.Thrown]);
                        _equippedItems[EquipSlot.Thrown] = null;
                        _entity.Primary = 0;
                    }

                    if (_equippedItems[EquipSlot.Shield] != null)
                    {
                        Remove(_equippedItems[EquipSlot.Shield]);
                        _equippedItems[EquipSlot.Shield] = null;
                        _entity.Shield = 0;
                    }

                    if (_equippedItems[EquipSlot.Held2] != null)
                    {
                        Remove(_equippedItems[EquipSlot.Held2]);
                        _equippedItems[EquipSlot.Held2] = null;
                        _entity.Secondary = 0;
                    }

                    if (_equippedItems[EquipSlot.Secondary] != null)
                    {
                        Remove(_equippedItems[EquipSlot.Secondary]);
                        _equippedItems[EquipSlot.Secondary] = null;
                        _entity.Secondary = 0;
                    }

                    item.EquipLocation = equipSlot;
                    _equippedItems[equipSlot] = item;
                    _entity.Shield = item.Model;
                    break;

                case EquipSlot.Held:
                case EquipSlot.Held2:
                    if (_equippedItems[EquipSlot.TwoHand] != null)
                    {
                        Remove(_equippedItems[EquipSlot.TwoHand]);
                        _equippedItems[EquipSlot.TwoHand] = null;
                        _entity.Primary = 0;
                    }

                    if (_equippedItems[EquipSlot.Thrown] != null)
                    {
                        Remove(_equippedItems[EquipSlot.Thrown]);
                        _equippedItems[EquipSlot.Thrown] = null;
                        _entity.Primary = 0;
                    }

                    if (_equippedItems[EquipSlot.Primary] == null)
                    {
                        _equippedItems[EquipSlot.Held] = item;
                        item.EquipLocation = EquipSlot.Held;
                        _entity.Primary = item.Model;
                    }

                    else
                    {
                        if (_equippedItems[EquipSlot.Shield] != null)
                        {
                            Remove(_equippedItems[EquipSlot.Shield]);
                            _equippedItems[EquipSlot.Shield] = null;
                            _entity.Shield = 0;
                        }

                        if (_equippedItems[EquipSlot.Held2] != null)
                        {
                            Remove(_equippedItems[EquipSlot.Held2]);
                            _equippedItems[EquipSlot.Held2] = null;
                            _entity.Secondary = 0;
                        }

                        if (_equippedItems[EquipSlot.Secondary] != null)
                        {
                            Remove(_equippedItems[EquipSlot.Secondary]);
                            _equippedItems[EquipSlot.Secondary] = null;
                            _entity.Secondary = 0;
                        }

                        _equippedItems[EquipSlot.Held2] = item;
                        item.EquipLocation = EquipSlot.Held2;
                        _entity.Secondary = item.Model;
                    }
                    break;

                case EquipSlot.TwoHand:
                case EquipSlot.Thrown:
                    if (_equippedItems[EquipSlot.TwoHand] != null)
                    {
                        Remove(_equippedItems[EquipSlot.TwoHand]);
                        _equippedItems[EquipSlot.TwoHand] = null;
                        _entity.Primary = 0;
                    }

                    if (_equippedItems[EquipSlot.Thrown] != null)
                    {
                        Remove(_equippedItems[EquipSlot.Thrown]);
                        _equippedItems[EquipSlot.Thrown] = null;
                        _entity.Primary = 0;
                    }

                    if (_equippedItems[EquipSlot.Primary] != null)
                    {
                        Remove(_equippedItems[EquipSlot.Primary]);
                        _equippedItems[EquipSlot.Primary] = null;
                        _entity.Primary = 0;
                    }

                    if (_equippedItems[EquipSlot.Held] != null)
                    {
                        Remove(_equippedItems[EquipSlot.Held]);
                        _equippedItems[EquipSlot.Held] = null;
                        _entity.Primary = 0;
                    }

                    if (_equippedItems[EquipSlot.Shield] != null)
                    {
                        Remove(_equippedItems[EquipSlot.Shield]);
                        _equippedItems[EquipSlot.Shield] = null;
                        _entity.Shield = 0;
                    }

                    if (_equippedItems[EquipSlot.Held2] != null)
                    {
                        Remove(_equippedItems[EquipSlot.Held2]);
                        _equippedItems[EquipSlot.Held2] = null;
                        _entity.Secondary = 0;
                    }

                    if (_equippedItems[EquipSlot.Secondary] != null)
                    {
                        Remove(_equippedItems[EquipSlot.Secondary]);
                        _equippedItems[EquipSlot.Secondary] = null;
                        _entity.Secondary = 0;
                    }

                    _equippedItems[equipSlot] = item;
                    item.EquipLocation = equipSlot;
                    _entity.Primary = item.Model;
                    break;
            }
        }
    }
}
