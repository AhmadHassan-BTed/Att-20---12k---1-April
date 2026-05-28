using System;
using System.Collections.Generic;

namespace ReturnHome.Server.EntityObject.Stats
{
    public sealed class ModifierDictionary
    {
        public readonly Dictionary<StatModifiers, int> dictionary;
        private Entity _e;

        public ModifierDictionary(Entity e)
        {
            _e = e;
            dictionary = new Dictionary<StatModifiers, int>();
            foreach(StatModifiers mod in Enum.GetValues(typeof(StatModifiers)))
                dictionary.Add(mod, 0);
        }

        public void Add(StatModifiers key, int value)
        {
            if (dictionary.ContainsKey(key))
                dictionary[key] += value;

            //Shouldn't be needed
            else
                return;

            //If it is an npc, this isn't really relevant to run through?
            if (!_e.isPlayer && (key != StatModifiers.STA))
                return;

            UpdateStatInformation(key);
        }

        public void UpdateStatInformation(StatModifiers key)
        { 
            switch (key)
            {
                case StatModifiers.STR:
                    _e.CalculatePower();
                    _e.UpdateStrength();
                    break;

                case StatModifiers.STA:
                    _e.CalculateHP();
                    if (_e.isPlayer)
                    { 
                        _e.CalculatePower();
                        _e.UpdateStamina();
                    }
                    break;

                case StatModifiers.AGI:
                    _e.CalculatePower();
                    _e.UpdateAgility();
                    break;

                case StatModifiers.DEX:
                    _e.CalculatePower();
                    _e.UpdateDexterity();
                    break;

                case StatModifiers.WIS:
                    _e.CalculatePower();
                    _e.UpdateWisdom();
                    UpdateResists();
                    break;

                case StatModifiers.INT:
                    _e.CalculatePower();
                    _e.UpdateIntelligence();
                    break;

                case StatModifiers.CHA:
                    _e.CalculatePower();
                    _e.UpdateCharisma();
                    break;

                case StatModifiers.HPMAX:
                    //Check to make sure currentHP doesn't exceed MaxHP
                    _e.UpdateMaxHP();
                    _e.ObjectUpdateHPBar();
                    break;

                    //Need to recalculate Power, adjust the Power slider in object update and adjust the power stats in the stat update message
                case StatModifiers.POWMAX:
                    _e.UpdateMaxPower();
                    break;

                case StatModifiers.PoT:
                    _e.UpdatePowerOverTime();
                    break;

                case StatModifiers.HoT:
                    _e.UpdateHealthOverTime();
                    break;

                case StatModifiers.AC:
                    _e.UpdateAC();
                    break;

                case StatModifiers.PoisonResistance:
                    _e.UpdatePoisonResist();
                    break;

                case StatModifiers.DiseaseResistance:
                    _e.UpdateDiseaseResist();
                    break;

                case StatModifiers.FireResistance:
                    _e.UpdateFireResist();
                    break;

                case StatModifiers.ColdResistance:
                    _e.UpdateColdResist();
                    break;

                case StatModifiers.LightningResistance:
                    _e.UpdateLightningResist();
                    break;

                case StatModifiers.ArcaneResistance:
                    _e.UpdateArcaneResist();
                    break;

                case StatModifiers.TPSTR:
                    _e.UpdateBaseStrength();
                    goto case StatModifiers.STR;

                case StatModifiers.TPSTA:
                    _e.UpdateBaseStamina();
                    goto case StatModifiers.STA;

                case StatModifiers.TPAGI:
                    _e.UpdateBaseAgility();
                    goto case StatModifiers.AGI;

                case StatModifiers.TPDEX:
                    _e.UpdateBaseDexterity();
                    goto case StatModifiers.DEX;

                case StatModifiers.TPWIS:
                    _e.UpdateBaseWisdom();
                    goto case StatModifiers.WIS;

                case StatModifiers.TPINT:
                    _e.UpdateBaseIntelligence();
                    goto case StatModifiers.INT;

                case StatModifiers.TPCHA:
                    _e.UpdateBaseCharisma();
                    goto case StatModifiers.CHA;
            }
        }

        public void Remove(StatModifiers key, int value)
        {
            if (dictionary.ContainsKey(key))
                dictionary[key] -= value;

            //Shouldn't be needed
            else
                dictionary.Add(key, value);

            //If it is an npc, this isn't really relevant to run through?
            if (!_e.isPlayer && (key != StatModifiers.STA))
                return;

            UpdateStatInformation(key);
        }

        private void UpdateBaseResists()
        {
            /*
            _e.UpdateBaseArcaneResist();
            _e.UpdateBaseFireResist();
            _e.UpdateBaseDiseaseResist();
            _e.UpdateBaseColdResist();
            _e.UpdateBaseLightningResist();
            _e.UpdateBasePoisonResist();
            */
            UpdateResists();
        }

        private void UpdateResists()
        {
            
            _e.UpdatePoisonResist();
            _e.UpdateDiseaseResist();
            _e.UpdateFireResist();
            _e.UpdateColdResist();
            _e.UpdateLightningResist();
            _e.UpdateArcaneResist();
        }
    }
}
