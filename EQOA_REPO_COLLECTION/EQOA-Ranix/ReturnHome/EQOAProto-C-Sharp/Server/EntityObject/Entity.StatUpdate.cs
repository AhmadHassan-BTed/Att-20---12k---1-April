using System.Runtime.InteropServices;
using ReturnHome.Server.EntityObject.Stats;

namespace ReturnHome.Server.EntityObject
{
    public partial class Entity
    {
        public void UpdateStrength()
        {
            int strength = Strength;
            MemoryMarshal.Write(StatUpdate.Span[0..], ref strength);
        }
        public void UpdateStamina()
        {
            int stamina = Stamina;
            MemoryMarshal.Write(StatUpdate.Span[4..], ref stamina);
        }
        public void UpdateAgility()
        {
            int agility = Agility;
            MemoryMarshal.Write(StatUpdate.Span[8..], ref agility);
        }
        public void UpdateDexterity()
        {
            int dexterity = Dexterity;
            MemoryMarshal.Write(StatUpdate.Span[12..], ref dexterity);
        }
        public void UpdateWisdom()
        {
            int wisdom = Wisdom;
            MemoryMarshal.Write(StatUpdate.Span[16..], ref wisdom);
        }
        public void UpdateIntelligence()
        {
            int intelligence = Intelligence;
            MemoryMarshal.Write(StatUpdate.Span[20..], ref intelligence);
        }
        public void UpdateCharisma()
        {
            int charisma = Charisma;
            MemoryMarshal.Write(StatUpdate.Span[24..], ref charisma);
        }
        public void UpdateCurrentHP1()
        {
            MemoryMarshal.Write(StatUpdate.Span[28..], ref _currentHP);
            MemoryMarshal.Write(StatUpdate.Span[144..], ref _currentHP);
        }
        public void UpdateMaxHP() => MemoryMarshal.Write(StatUpdate.Span[32..], ref _hpMax);

        public void UpdateBaseHP() => MemoryMarshal.Write(StatUpdate.Span[148..], ref _baseHP);

        public void UpdateCurrentPower()
        {
            MemoryMarshal.Write(StatUpdate.Span[36..], ref _currentPower);
            MemoryMarshal.Write(StatUpdate.Span[152..], ref _currentPower);
        }

        public void UpdateBasePower() => MemoryMarshal.Write(StatUpdate.Span[156..], ref _basePower);

        public void UpdateMaxPower() => MemoryMarshal.Write(StatUpdate.Span[40..], ref _powerMax);

        public void UpdateUnknown1()
        {
            MemoryMarshal.Write(StatUpdate.Span[44..], ref _randoValue);
            MemoryMarshal.Write(StatUpdate.Span[160..], ref _randoValue);
        }

        public void UpdateHealthOverTime()
        {
            int hot = CurrentStats.dictionary[StatModifiers.HoT];
            MemoryMarshal.Write(StatUpdate.Span[48..], ref hot);
            MemoryMarshal.Write(StatUpdate.Span[164..], ref hot);
        }

        public void UpdatePowerOverTime()
        {
            int pot = CurrentStats.dictionary[StatModifiers.PoT];
            MemoryMarshal.Write(StatUpdate.Span[52..], ref pot);
            MemoryMarshal.Write(StatUpdate.Span[168..], ref pot);
        }

        public void UpdateAC()
        {
            int ac = CurrentAC;
            MemoryMarshal.Write(StatUpdate.Span[56..], ref ac);
        }

        public void UpdateUnknown2() => MemoryMarshal.Write(StatUpdate.Span[64..], ref _unk2);

        public void UpdatePoisonResist()
        {
            int Poison = _poisonResist;
            MemoryMarshal.Write(StatUpdate.Span[88..], ref Poison);
        }

        public void UpdateDiseaseResist()
        {
            int Disease = _diseaseResist;
            MemoryMarshal.Write(StatUpdate.Span[92..], ref Disease);
        }

        public void UpdateFireResist()
        {
            int Fire = _fireResist;
            MemoryMarshal.Write(StatUpdate.Span[96..], ref Fire);
        }

        public void UpdateColdResist()
        {
            int Cold = _coldResist;
            MemoryMarshal.Write(StatUpdate.Span[100..], ref Cold);
        }

        public void UpdateLightningResist()
        {
            int Lightning = _lightningResist;
            MemoryMarshal.Write(StatUpdate.Span[104..], ref Lightning);
        }

        public void UpdateArcaneResist()
        {
            int Arcane = _arcaneResist;
            MemoryMarshal.Write(StatUpdate.Span[108..], ref Arcane);
        }

        public void UpdateFishing() => MemoryMarshal.Write(StatUpdate.Span[112..], ref _fishing);

        public void UpdateBaseStrength()
        {
            int baseStrength = BaseStrength;
            MemoryMarshal.Write(StatUpdate.Span[116..], ref baseStrength);
        }
        public void UpdateBaseStamina()
        {
            int baseStamina = BaseStamina;
            MemoryMarshal.Write(StatUpdate.Span[120..], ref baseStamina);
        }
        public void UpdateBaseAgility()
        {
            int baseAgility = BaseAgility;
            MemoryMarshal.Write(StatUpdate.Span[124..], ref baseAgility);
        }
        public void UpdateBaseDexterity()
        {
            int baseDexterity = BaseDexterity;
            MemoryMarshal.Write(StatUpdate.Span[128..], ref baseDexterity);
        }
        public void UpdateBaseWisdom()
        {
            int baseWisdom = BaseWisdom;
            MemoryMarshal.Write(StatUpdate.Span[132..], ref baseWisdom);
        }
        public void UpdateBaseIntelligence()
        {
            int baseIntelligence = BaseIntelligence;
            MemoryMarshal.Write(StatUpdate.Span[136..], ref baseIntelligence);
        }
        public void UpdateBaseCharisma()
        {
            int baseCharisma = BaseCharisma;
            MemoryMarshal.Write(StatUpdate.Span[140..], ref baseCharisma);
        }
        public void UpdateBaseAC() => MemoryMarshal.Write(StatUpdate.Span[172..], ref _baseAC);
        /*
        public void UpdateBasePoisonResist()
        {
            _basePoisonResist = BasePoisonResist;
            MemoryMarshal.Write(StatUpdate.Span[204..], ref _basePoisonResist);
        }
        public void UpdateBaseDiseaseResist()
        {
            _baseDiseaseResist = BaseDiseaseResist;
            MemoryMarshal.Write(StatUpdate.Span[208..], ref _baseDiseaseResist);
        }
        public void UpdateBaseFireResist()
        {
            _baseFireResist = BaseFireResist;
            MemoryMarshal.Write(StatUpdate.Span[212..], ref _baseFireResist);
        }
        public void UpdateBaseColdResist()
        {
            _baseColdResist = BaseColdResist;
            MemoryMarshal.Write(StatUpdate.Span[216..], ref _baseColdResist);
        }
        public void UpdateBaseLightningResist()
        {
            _baseLightningResist = BaseLightningResist;
            MemoryMarshal.Write(StatUpdate.Span[220..], ref _baseLightningResist);
        }
        public void UpdateBaseArcaneResist()
        {
            _baseArcaneResist = BaseArcaneResist;
            MemoryMarshal.Write(StatUpdate.Span[224..], ref _baseArcaneResist);
        }*/
    }
}
