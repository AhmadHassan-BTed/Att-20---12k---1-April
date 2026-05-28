using System;
using System.IO;
using System.Text;
using ReturnHome.Utilities;

namespace ReturnHome.Server.EntityObject.Player
{
  //
    public class Spell
    {
        public int SpellID { get; private set; }
        public int AddedOrder { get; private set; }
        public int OnHotBar { get; private set; }
        public int WhereOnHotBar { get; private set; }
        public int Unk1 { get; private set; }
        public int ShowHide { get; private set; }
        public int AbilityLevel { get; private set; }
        public int Unk2 { get; private set; }
        public int Unk3 { get; private set; }
        public float SpellRange { get; private set; }
        public int CastTime { get; private set; }
        public int Power { get; private set; }
        public int IconColor { get; private set; }
        public int Icon { get; private set; }
        public int Scope { get; private set; }
        public int Recast { get; private set; }
        public int EqpRequirement { get; private set; }
        public string SpellName { get; private set; }
        public string SpellDesc { get; private set; }

        public Spell()
        { }

        //Will instantiate a spell object
        //Alot of this probably should move to some kind of scripting? Alot of repetitive data here on a per object instance basis, that could be maximized by scripting
        //Only items really needed are (int thisSpellID, int thisAddedOrder, int thisOnHotBar, int thisWhereOnHotBar, int thisUnk1, int thisShowHide) rest of data could be acquired from scripting
        public Spell(int thisSpellID, int thisAddedOrder, int thisOnHotBar, int thisWhereOnHotBar, int thisUnk1, int thisShowHide, int thisAbilityLevel, int thisUnk2, int thisUnk3, float thisRange, int thisCastTime, int thisPower, int thisIconColor, int thisIcon, int thisScope, int thisRecast, int thisEqpRequirement, string thisSpellName, string thisSpellDesc)
        {
            SpellID = thisSpellID;
            AddedOrder = thisAddedOrder;
            OnHotBar = thisOnHotBar;
            WhereOnHotBar = thisWhereOnHotBar;
            Unk1 = thisUnk1;
            ShowHide = thisShowHide;
            AbilityLevel = thisAbilityLevel;
            Unk2 = thisUnk2;
            Unk3 = thisUnk3;
            SpellRange = thisRange;
            CastTime = thisCastTime;
            Power = thisPower;
            IconColor = thisIconColor;
            Icon = thisIcon;
            Scope = thisScope;
            Recast = thisRecast;
            EqpRequirement = thisEqpRequirement;
            SpellName = thisSpellName;
            SpellDesc = thisSpellDesc;
        }

        
        public void DumpSpell(ref BufferWriter writer)
        {
            //Start gathering the data
            writer.Write7BitEncodedInt64(SpellID);
            writer.Write7BitEncodedInt64(AddedOrder);
            writer.Write7BitEncodedInt64(OnHotBar);
            writer.Write7BitEncodedInt64(WhereOnHotBar);
            writer.Write7BitEncodedInt64(Unk1);
            writer.Write7BitEncodedInt64(ShowHide);
            writer.Write7BitEncodedInt64(AbilityLevel);
            writer.Write7BitEncodedInt64(Unk2);
            writer.Write7BitEncodedInt64(Unk3);

            writer.Write((Half)SpellRange);
            writer.Write7BitEncodedInt64(CastTime);
            writer.Write7BitEncodedInt64(Power);
            writer.Write7BitEncodedInt64(IconColor);
            writer.Write7BitEncodedInt64(Icon);
            writer.Write7BitEncodedInt64(Scope);
            writer.Write7BitEncodedInt64(Recast);
            writer.Write7BitEncodedInt64(EqpRequirement);
            writer.WriteString(Encoding.Unicode, SpellName);
            writer.WriteString(Encoding.Unicode, SpellDesc);
            writer.Write((byte)0);
        }
    }
}
