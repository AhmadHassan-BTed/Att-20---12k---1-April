using ReturnHome.Server.EntityObject.Items;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Utilities;

namespace ReturnHome.Server.EntityObject
{
    public partial class Entity
    {
        private Race _race;
        private Class _actorClass;
        private HumanType _humType;
        private Sex _sex;

        private string _charName;

        private int _hairColor;
        private int _hairLength;
        private int _hairStyle;
        private int _faceOption;

        private byte _helm;
        private byte _chest;
        private byte _gloves;
        private byte _bracers;
        private byte _legs;
        private byte _boots;

        //default is 0xFFFFFFFF, means no robe
        private int _robe;
        private int _primary;
        private int _secondary;
        private int _shield;

        private int _modelID;
        private float _modelSize;

        ///Armor color
        private uint _helmColor;
        private uint _chestColor;
        private uint _glovesColor;
        private uint _bracerColor;
        private uint _legsColor;
        private uint _bootsColor;
        private uint _robeColor;

        #region Objext Update Properties

        public Race EntityRace
        {
            get { return _race; }
            set
            {
                if (value >= Race.Human && value <= Race.Ogre)
                {
                    _race = value;
                    ObjectUpdateRace();
                }

                else
                    Logger.Err($"Error setting Race value {value} for {_charName}");
            }
        }

        public Class EntityClass
        {
            get { return _actorClass; }
            set
            {
                if (value >= Class.Warrior && value <= Class.Alchemist)
                {
                    _actorClass = value;
                    ObjectUpdateClass();
                }

                else
                    Logger.Err($"Error setting Class value {value} for {_charName}");
            }
        }

        public Sex EntitySex
        {
            get { return _sex; }
            set
            {
                if(value >= Sex.Male && value <= Sex.Female)
                    _sex = value;
            }
        }

        public HumanType EntityHumanType
        {
            get {  return _humType; }
            set
            {
                if(value >= HumanType.Other && value <= HumanType.Western)
                {
                    _humType = value;
                }
            }
        }

        public string CharName
        {
            get { return _charName; }
            set
            {
                if (value.Length >= 2 && value.Length <= 25)
                {
                    _charName = value;
                    ObjectUpdateName();
                }

                else
                    Logger.Err($"Error setting Name {value} for {_charName}");
            }
        }

        public int HairColor
        {
            get { return _hairColor; }
            set
            {
                if (value >= 0 && value <= 9)
                {
                    _hairColor = value;
                    ObjectUpdateHairColor();
                }

                else
                    Logger.Err($"Error setting HairColor {value} for {_charName}");
            }
        }

        public int HairLength
        {
            get { return _hairLength; }
            set
            {
                if (value >= 0 && value <= 4)
                {
                    _hairLength = value;
                    ObjectUpdateHairLength();
                }

                else
                    Logger.Err($"Error setting Hair lengtb {_hairLength} for {_charName}");
            }
        }

        public int HairStyle
        {
            get { return _hairStyle; }
            set
            {
                if (value >= 0 && value <= 4)
                {
                    _hairStyle = value;
                    ObjectUpdateHairStyle();
                }

                else
                    Logger.Err($"Error setting Hair Style {value} for {_charName}");
            }
        }

        public int FaceOption
        {
            get { return _faceOption; }
            set
            {
                if (value >= 0 && value <= 4)
                {
                    _faceOption = value;
                    ObjectUpdateFaceOption();
                }

                else
                    Logger.Err($"Error setting Face {value} for {_charName}");
            }
        }

        public int Robe
        {
            get { return _robe; }
            set
            {
                if (value >= -1 && value <= 4)
                {
                    _robe = value;
                    ObjectUpdateRobe();
                }

                else
                    Logger.Err($"Error setting Robe {value} for {_charName}");
            }
        }

        public int Primary
        {
            get { return _primary; }
            set
            {
                if (true)
                {
                    _primary = value;
                    ObjectUpdatePrimary();
                }

                else
                    Logger.Err($"Error setting Primary {_primary} for {_charName}");
            }
        }

        public int Secondary
        {
            get { return _secondary; }
            set
            {
                if (true)
                {
                    _secondary = value;
                    ObjectUpdateSecondary();
                }

                else
                    Logger.Err($"Error setting Secondary {value} for {_charName}");
            }
        }

        public int Shield
        {
            get { return _shield; }
            set
            {
                if (true)
                {
                    _shield = value;
                    ObjectUpdateShield();
                }

                else
                    Logger.Err($"Error setting Shield {value} for {_charName}");
            }
        }

        public byte Helm
        {
            get { return _helm; }
            set
            {
                if (value >= 0 && value <= 8)
                {
                    _helm = value;
                    ObjectUpdateHelm();
                }

                else
                    Logger.Err($"Error setting Helm {value} for {_charName}");
            }
        }

        public byte Chest
        {
            get { return _chest; }
            set
            {
                if(value >= 0 && value <= 8)
                {
                    _chest = value;
                    ObjectUpdateChest();
                }
            }
        }

        public byte Gloves
        {
            get { return _gloves; }
            set
            {
                if ( value >= 0 && value <= 8)
                {
                    _gloves = value;
                    ObjectUpdateGloves();
                }
            }
        }

        public byte Bracer
        {
            get { return _bracers; }
            set
            {
                if( value >= 0 && value <= 8)
                {
                    _bracers = value;
                    ObjectUpdateBracer();
                }
            }
        }

        public byte Legs
        {
            get { return _legs; }
            set
            {
                if( value >= 0 && value <= 8)
                {
                    _legs = value;
                    ObjectUpdateLegs();
                }
            }
        }

        public byte Boots
        {
            get { return _boots; }
            set
            {
                if( value >= 0 && value <= 8)
                {
                    _boots = value;
                    ObjectUpdateBoots();
                }
            }
        }

        public uint HelmColor
        {
            get { return _helmColor; }
            set
            {
                if (true)
                {
                    _helmColor = value;
                    ObjectUpdateHelmColor();
                }
            }
        }

        public uint ChestColor 
        {
            get { return _chestColor; }
            set
            {
                if (true)
                {
                    _chestColor = value;
                    ObjectUpdateChestColor();
                }
            }
        }

        public uint GloveColor
        {
            get { return _glovesColor; }
            set
            {
                if (true)
                {
                    _glovesColor = value;
                    ObjectUpdateGloveColor();
                }
            }
        }

        public uint BracerColor
        {
            get { return _bracerColor; }
            set
            {
                if (true)
                {
                    _bracerColor = value;
                    ObjectUpdateBracerColor();
                }
            }
        }

        public uint LegColor
        {
            get { return _legsColor; }
            set
            {
                if (true)
                {
                    _legsColor = value;
                    ObjectUpdateLegColor();
                }
            }
        }

        public uint BootsColor
        {
            get { return _bootsColor; }
            set
            {
                if (true)
                {
                    _bootsColor = value;
                    ObjectUpdateBootsColor();
                }
            }
        }

        public uint RobeColor
        {
            get { return _robeColor; }
            set
            {
                if (true)
                {
                    _robeColor = value;
                    ObjectUpdateRobeColor();
                }
            }
        }

        public int ModelID
        {
            get { return _modelID; }
            set
            {
                if(true)
                {
                    _modelID = value;
                    ObjectUpdateModelID();
                }
            }
        }

        public float ModelSize
        {
            get { return _modelSize; }
            set
            {
                if(value >= .025f && value <= 10.0f)
                {
                    _modelSize = value;
                    ObjectUpdateModelSize();
                }
            }
        }
        #endregion

        //This provides us with the proper gear and gear type for visual display on character
        public void EquipGear(Character character)
        {
            ///Start processing item
            for (int i = 0; i < character.Inventory.Count; ++i)
            {
                Item item = character.Inventory.itemContainer[i].item;
                if ((sbyte)item.EquipLocation == -1)
                    return;

                ///Use a switch to sift through item and add them properly
                switch ((byte)item.EquipLocation)
                {
                    ///Helm
                    case 0:
                        Helm = (byte)item.Pattern.Model;
                        HelmColor = item.Pattern.Color;
                        break;

                    ///Robe
                    case 21:
                        Robe = (byte)item.Pattern.Model;
                        RobeColor = item.Pattern.Color;
                        break;

                    ///Gloves
                    case 1:
                        Gloves = (byte)item.Pattern.Model;
                        GloveColor = item.Pattern.Color;
                        break;

                    ///Chest
                    case 5:
                        Chest = (byte)item.Pattern.Model;
                        ChestColor = item.Pattern.Color;
                        break;

                    ///Bracers
                    case 8:
                        Bracer = (byte)item.Pattern.Model;
                        BracerColor = item.Pattern.Color;
                        break;

                    ///Legs
                    case 12:
                        Legs = (byte)item.Pattern.Model;
                        LegColor = item.Pattern.Color;
                        break;

                    ///Feet
                    case 13:
                        Boots = (byte)item.Pattern.Model;
                        BootsColor = item.Pattern.Color;
                        break;

                    ///Primary
                    case 14:
                        Primary = item.Pattern.Model;
                        break;

                    ///Secondary
                    case 15:

                        ///If we have a secondary equipped already, puts next secondary into primary slot
                        if (Secondary > 0)
                        {
                            Primary = item.Pattern.Model;
                        }

                        ///If no secondary, add to secondary slot
                        else
                        {
                            Secondary = item.Pattern.Model;
                        }
                        break;

                    ///2 Hand
                    case 16:
                        Primary = item.Pattern.Model;
                        break;

                    ///Shield
                    case 17:
                        Shield = item.Pattern.Model;
                        break;

                    ///Bow
                    case 18:
                        Primary = item.Pattern.Model;
                        break;

                    ///Held
                    case 19:
                    case 20: 
                        ///If we have a secondary equipped already, puts next secondary into primary slot
                        if (Secondary > 0)
                        {
                            Primary = item.Pattern.Model;
                        }

                        ///If no secondary, add to secondary slot
                        else
                        {
                            Secondary = item.Pattern.Model;
                        }
                        break;

                    default:
                        Logger.Err("Equipment not in list, this may need to be changed");
                        break;
                }
            }
        }
    }
}
