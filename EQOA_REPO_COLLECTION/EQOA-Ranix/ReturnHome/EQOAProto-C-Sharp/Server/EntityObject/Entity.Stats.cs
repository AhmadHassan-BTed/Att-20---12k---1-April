using System;
using ReturnHome.Server.EntityObject.Stats;

namespace ReturnHome.Server.EntityObject
{
    public partial class Entity
    {
        //Initialize this with every entity to account for their stats
        public ModifierDictionary CurrentStats;


        //This is the absolute max amount of a "base" stat that can be assigned, everything above here needs to be buff's and CM's
        public readonly int BaseMaxStat;
        private int _minimumStat = -100;
        private int _randoValue = 0x64;

        private int _unk2;

        //Our most current stats. This is stat totals influenced by gear, buff's, CM's and TP's.
        public int BaseStrength => Clamp((isPlayer ? DefaultCharacter.DefaultCharacterDict[(EntityRace, EntityClass, EntityHumanType, EntitySex)].CurrentStats.dictionary[StatModifiers.BaseSTR] : 0) + CurrentStats.dictionary[StatModifiers.TPSTR], _minimumStat, BaseMaxStat);
        public int BaseStamina => Clamp((isPlayer ? DefaultCharacter.DefaultCharacterDict[(EntityRace, EntityClass, EntityHumanType, EntitySex)].CurrentStats.dictionary[StatModifiers.BaseSTA] : 0) + CurrentStats.dictionary[StatModifiers.TPSTA], _minimumStat, BaseMaxStat);
        public int BaseWisdom => Clamp((isPlayer ? DefaultCharacter.DefaultCharacterDict[(EntityRace, EntityClass, EntityHumanType, EntitySex)].CurrentStats.dictionary[StatModifiers.BaseWIS] : 0) + CurrentStats.dictionary[StatModifiers.TPWIS], _minimumStat, BaseMaxStat);
        public int BaseIntelligence => Clamp((isPlayer ? DefaultCharacter.DefaultCharacterDict[(EntityRace, EntityClass, EntityHumanType, EntitySex)].CurrentStats.dictionary[StatModifiers.BaseINT] : 0) + CurrentStats.dictionary[StatModifiers.TPINT], _minimumStat, BaseMaxStat);
        public int BaseDexterity => Clamp((isPlayer ? DefaultCharacter.DefaultCharacterDict[(EntityRace, EntityClass, EntityHumanType, EntitySex)].CurrentStats.dictionary[StatModifiers.BaseDEX] : 0) + CurrentStats.dictionary[StatModifiers.TPDEX], _minimumStat, BaseMaxStat);
        public int BaseAgility => Clamp((isPlayer ? DefaultCharacter.DefaultCharacterDict[(EntityRace, EntityClass, EntityHumanType, EntitySex)].CurrentStats.dictionary[StatModifiers.BaseAGI] : 0) + CurrentStats.dictionary[StatModifiers.TPAGI], _minimumStat, BaseMaxStat);
        public int BaseCharisma => Clamp((isPlayer ? DefaultCharacter.DefaultCharacterDict[(EntityRace, EntityClass, EntityHumanType, EntitySex)].CurrentStats.dictionary[StatModifiers.BaseCHA] : 0) + CurrentStats.dictionary[StatModifiers.TPCHA], _minimumStat, BaseMaxStat);

        public int Strength => Clamp(BaseStrength + CurrentStats.dictionary[StatModifiers.STR], _minimumStat, MaxStrength);
        public int Stamina => Clamp(BaseStamina + CurrentStats.dictionary[StatModifiers.STA], _minimumStat, MaxStamina);
        public int Wisdom => Clamp(BaseWisdom + CurrentStats.dictionary[StatModifiers.WIS], _minimumStat, MaxWisdom);
        public int Intelligence => Clamp(BaseIntelligence + CurrentStats.dictionary[StatModifiers.INT], _minimumStat, MaxIntelligence);
        public int Dexterity => Clamp(BaseDexterity + CurrentStats.dictionary[StatModifiers.DEX], _minimumStat, MaxDexterity);
        public int Agility => Clamp(BaseAgility + CurrentStats.dictionary[StatModifiers.AGI], _minimumStat, MaxAgility);
        public int Charisma => Clamp(BaseCharisma + CurrentStats.dictionary[StatModifiers.CHA], _minimumStat, MaxCharisma);

        #region Properties for Stat Maxes and Private Var's

        //Our most current stats. This is stat totals influenced by gear, buff's, CM's and TP's.
        private int _maxStrength;
        private int _maxStamina;
        private int _maxWisdom;
        private int _maxIntelligence;
        private int _maxCharisma;
        private int _maxAgility;
        private int _maxDexterity;

        public int MaxStrength
        {
            get { return _maxStrength + BaseMaxStat; }
            set { _maxStrength = value; }
        }

        public int MaxStamina
        {
            get { return _maxStamina + BaseMaxStat; }
            set { _maxStamina = value; }
        }

        public int MaxWisdom
        {
            get { return _maxWisdom + BaseMaxStat; }
            set { _maxWisdom = value; }
        }

        public int MaxIntelligence
        {
            get { return _maxIntelligence + BaseMaxStat; }
            set { _maxIntelligence = value; }
        }

        public int MaxDexterity
        {
            get { return _maxDexterity + BaseMaxStat; }
            set { _maxDexterity = value; }
        }

        public int MaxAgility
        {
            get { return _maxAgility + BaseMaxStat; }
            set { _maxAgility = value; }
        }

        public int MaxCharisma
        {
            get { return _maxCharisma + BaseMaxStat; }
            set { _maxCharisma = value; }
        }

        #endregion

        #region Properties of base/current HP/Power/AC and private var's

        //This is decided by the character's arch type + CM's, should npc/boss type mobs have special modifier's?
        private int _hpFactor;

        private int _baseAC;
        private int _baseHP;
        private int _basePower;

        public int HPFactor
        {
            get { return _hpFactor; }
            set
            {
                _hpFactor = value;
            }
        }
        public int BaseAC
        {
            get { return _baseAC; }
            set
            {
                _baseAC = value;
                UpdateBaseAC();
            }
        }

        public int BaseHP
        {
            get { return _baseHP; }
            set
            {
                _baseHP = value;
                CurrentHP = _baseHP;
                UpdateBaseHP();
            }
        }

        public int BasePower
        {
            get { return _basePower; }
            set
            {
                _basePower = value;
                UpdateBasePower();
            }
        }

        //Gear and Buff's should write to this variable to add to the total HPMax Value
        public int GearBuffHPMax = 0;
        public int GearBuffPowerMax = 0;
        public int GearBuffAC = 0;

        private int _currentAC;
        private int _currentHP;
        private int _currentPower;
        private bool _hpFlag;

        public int CurrentAC => BaseAC + CurrentStats.dictionary[StatModifiers.AC];

        public int CurrentHP
        {
            get { return _currentHP; }
            set
            {

                if (value > HPMax)
                    _currentHP = _hpMax;

                else
                    _currentHP = value;

                //TODO: Trigger dying stuff here?
                if (value <= 0)
                    Console.WriteLine($"{CharName} died.");

                //Update HP for stat message
                UpdateCurrentHP1();

                //Update HP bar for the object update message
                ObjectUpdateHPBar();
            }
        }

        public int CurrentPower
        {
            get { return _currentPower; }
            set
            {
                _currentPower = value;
                UpdateCurrentPower();
            }
        }

        //Need to calculate HP/PWR max on the fly
        //Will be influenced by archtype, level, stamina, gear, buffs and CM's
        private int _hpMax;
        private int _powerMax;

        public int HPMax
        {
            get { return _hpMax; }
            set
            {
                _hpMax = value;
                UpdateMaxHP();
                ObjectUpdateHPBar();

                if (_currentHP > _hpMax)
                    _currentHP = _hpMax;
            }
        }

        public int PowerMax
        {
            get { return _powerMax; }
            set
            {
                _powerMax = value;
                UpdateMaxPower();
            }
        }

        public bool HPFlag
        {
            get { return _hpFlag; }
            set
            {
                _hpFlag = value;
                ObjectUpdateHPFlag();
            }
        }

        private int _basePoisonResist { get { return 40 + BaseWisdom / 7; } }
        private int _baseDiseaseResist { get { return 40 + BaseWisdom / 7; } }
        private int _baseLightningResist { get { return 40 + BaseWisdom / 7; } }
        private int _baseFireResist { get { return 40 + BaseWisdom / 7; } }
        private int _baseColdResist { get { return 40 + BaseWisdom / 7; } }
        private int _baseArcaneResist { get { return 40 + BaseWisdom / 7; } }

        public int PoisonResist => Clamp(40 + (Wisdom / 7) + CurrentStats.dictionary[StatModifiers.PoisonResistance], -100, 315);

        public int DiseaseResist => Clamp(40 + (Wisdom / 7) + CurrentStats.dictionary[StatModifiers.DiseaseResistance], -100, 315);

        public int LightningResist => Clamp(40 + (Wisdom / 7) + CurrentStats.dictionary[StatModifiers.LightningResistance], -100, 315);

        public int FireResist => Clamp(40 + (Wisdom / 7) + CurrentStats.dictionary[StatModifiers.FireResistance], -100, 315);

        public int ColdResist => Clamp(40 + (Wisdom / 7) + CurrentStats.dictionary[StatModifiers.ColdResistance], -100, 315);

        public int ArcaneResist => Clamp(40 + (Wisdom / 7) + CurrentStats.dictionary[StatModifiers.ArcaneResistance], -100, 315);
        public int _poisonResist => Clamp(CurrentStats.dictionary[StatModifiers.PoisonResistance], -100, 315 - 40 + (Wisdom / 7));

        public int _diseaseResist => Clamp(CurrentStats.dictionary[StatModifiers.DiseaseResistance], -100, 315 - 40 + (Wisdom / 7));

        public int _lightningResist => Clamp(CurrentStats.dictionary[StatModifiers.LightningResistance], -100, 315 - 40 + (Wisdom / 7));

        public int _fireResist => Clamp(CurrentStats.dictionary[StatModifiers.FireResistance], -100, 315 - 40 + (Wisdom / 7));

        public int _coldResist => Clamp(CurrentStats.dictionary[StatModifiers.ColdResistance], -100, 315 - 40 + (Wisdom / 7));

        public int _arcaneResist => Clamp(CurrentStats.dictionary[StatModifiers.ArcaneResistance], -100, 315 - 40 + (Wisdom / 7));
        #endregion

        private int _fishing;

        public void CalculateHP()
        {
            //BaseHP is calculated by utilizing the current Stamina of the player + and HP Factors they may have
            BaseHP = (Level * ((_hpFactor * 100) + (Stamina * 100) / 11)) / 100;

            //Utilizing BaseHP + Gear HP + buffs
            HPMax = BaseHP + CurrentStats.dictionary[StatModifiers.HPMAX];

            //For now set current HP to HP MAX
            CurrentHP = BaseHP;
        }

        public void CalculatePower()
        {
            //Create a switch expression to calculate Base Power
            BasePower = _actorClass switch
            {
                //Warrior/Paladin/Shadowknight
                Class.Warrior or Class.Paladin or Class.ShadowKnight => Calculatetemp(Strength, Stamina),

                //Ranger/Rogue
                Class.Ranger or Class.Rogue => Calculatetemp(Agility, Dexterity),

                //Monk/Bard
                Class.Monk or Class.Bard => Calculatetemp(Agility, Wisdom),

                //Druid
                Class.Druid => Calculatetemp(Wisdom, Dexterity),

                //Shaman
                Class.Shaman => Calculatetemp(Wisdom, Stamina),

                //Cleric
                Class.Cleric => Calculatetemp(Wisdom, Charisma),

                //Magician
                Class.Magician => Calculatetemp(Intelligence, Agility),

                //Necromancer
                Class.Necromancer => Calculatetemp(Intelligence, Stamina),

                //Enchanter
                Class.Enchanter => Calculatetemp(Intelligence, Charisma),

                //Wizard
                Class.Wizard => Calculatetemp(Intelligence, Dexterity),

                //Alchemist
                Class.Alchemist => Calculatetemp(Intelligence, Wisdom),

                _ => throw new NotImplementedException()
            }; // + CMPower;

            PowerMax = BasePower + CurrentStats.dictionary[StatModifiers.POWMAX];

            CurrentPower = BasePower;
        }

        private int Calculatetemp(int Primary, int Secondary) => (((28 * 100) + ((Primary * 100) / 10) + ((Secondary * 100) / 20)) * Level) / 100;

        public static int Clamp(int value, int min, int max)
        {
            if (min > max)
                throw new ArgumentException($"{nameof(min)} can not exceed {nameof(max)}");

            if (value < min)
                return min;
            else if (value > max)
                return max;

            return value;
        }
    }
}
