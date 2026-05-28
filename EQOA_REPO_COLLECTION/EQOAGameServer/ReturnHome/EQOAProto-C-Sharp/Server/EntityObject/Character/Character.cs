using System.Collections.Generic;
using System.Text;
using ReturnHome.Server.EntityObject.Items;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;
using ReturnHome.Server.EntityObject.Stats;
using System;
using System.Text.Json;

namespace ReturnHome.Server.EntityObject.Player
{
    public partial class Character : Entity
    {
        //default to true, only opcode reading from client should be writing here.
        public bool LootMessages = true;
        public bool DamageNumberMessages = true;
        public bool FactionMessages = true;


        public int CMCounter = 1;
        public int CMPercentage = 0;
        public int SpentCMs = 0;
        public int UnspentCMs = 0;

        public uint GroupID = 0;

        private static string Tunaria = "data\\tunaria.esf";

        //Our Lists for attributes of character
        public ItemContainer Bank;
        public List<Item> AuctionItems = new List<Item> { };
        public List<WeaponHotbar> WeaponHotbars = new List<WeaponHotbar> { };
        public List<Hotkey> MyHotkeys = new List<Hotkey> { };
        public List<Spell> MySpells = new List<Spell> { };
        public List<Auction> MySellingAuctions = new List<Auction> { };
        public List<Auction> MyBuyingAuctions = new List<Auction> { };
        public List<Quest> activeQuests = new List<Quest>();
        public List<Quest> completedQuests = new List<Quest>();
        public Dialogue MyDialogue = new Dialogue();
        public Dictionary<string, string> playerFlags = new Dictionary<string, string>();

        public TrainingPoints PlayerTrainingPoints;

        public World ExpectedWorld;
        //this Reference helps keep these 2 objects tied together
        public Session characterSession;


        public int XPEarnedInThisLevel;
        public long TotalXP = 0;
        public int totalDebt = 0;

        public int Breath = 255;

        public int Fishing;

        public int Teleportcounter { get; internal set; } = 0;

        //Used to create our Default characters stored on the server to either reference values, or deep copy for entirely new characters
        public Character(int race, int tclass, int humanType, int sex, float X, float Y, float Z, float facing, float speed, int world, int modelID, List<KeyValuePair<StatModifiers, int>> temp, List<Hotkey> hotkeys) : base(true, 1)
        {
            Speed = speed;
            EntityRace = (Race)race;
            EntityClass = (Class)tclass;
            EntityHumanType = (HumanType)humanType;
            EntitySex = (Sex)sex;
            //Default characters start with 20 total and 20 unused TP's
            PlayerTrainingPoints = new(20, 20);
            Level = 1;
            Inventory = new(50000000, this);
            Bank = new(0, this, false);
            x = X;
            y = Y;
            z = Z;
            FacingF = facing;
            World = (World)world;
            ModelID = modelID;
            MyHotkeys= hotkeys;
            //Base HPFactor calculated in Character
            switch (EntityClass)
            {
                case Class.Paladin:
                case Class.Warrior:
                case Class.ShadowKnight:
                    HPFactor = 24;
                    break;

                case Class.Druid:
                case Class.Shaman:
                case Class.Cleric:
                    HPFactor = 13;
                    break;

                case Class.Ranger:
                case Class.Monk:
                case Class.Bard:
                case Class.Rogue:
                    HPFactor = 16;
                    break;

                case Class.Alchemist:
                case Class.Enchanter:
                case Class.Magician:
                case Class.Necromancer:
                case Class.Wizard:
                    HPFactor = 10;
                    break;
            }

            //Add Default Stats to dictionary
            foreach (KeyValuePair<StatModifiers, int> kv in temp)
                CurrentStats.Add(kv.Key, kv.Value);
        }

        //Need instantiation, but needs some review because it's so big... 
        public Character(string charName, int serverID, int modelID, int tClass, int race, int humType, int level, int hairColor, int hairLength, int hairStyle, int faceOption, int sex, int earnedXP, int debt, int breath, int tunar, int bankTunar, int UnusedTrainingPoints, int TotalTrainingPoints,
                         float speed, int world, float xCoord, float yCoord, float zCoord, float facing, int tpStrength, int tpStamina, int tpAgility, int tpDexterity, int tpWisdom, int tpIntelligence, int tpCharisma, int currentHP, int currentPower, int aC,
                         int poisonResist, int diseaseResist, int fireResist, int coldResist, int lightningResist, int arcaneResist, int fishing, string playerFlags, string completedQuests, string activeQuests, Session MySession) : base(true, level)
        {
            Speed = speed;
            Target = 0xFFFFFFFF;
            CharName = charName;
            ServerID = serverID;
            ObjectID = MySession.SessionID;
            ModelID = modelID;
            EntityClass = (Class)tClass;
            EntityRace = (Race)race;
            EntityHumanType = (HumanType)humType;
            EntitySex = (Sex)sex;
            Level = level;
            HairColor = hairColor;
            HairLength = hairLength;
            HairStyle = hairStyle;
            FaceOption = faceOption;
            XPEarnedInThisLevel = earnedXP;

            //Calculate Total XP
            for (int i = 1; i < Level; i++)
                TotalXP += CharacterUtilities.CharXPDict[i];
            TotalXP += XPEarnedInThisLevel;

            totalDebt = debt;
            Breath = breath;
            Inventory = new(tunar, this);
            Bank = new(bankTunar, this, false);
            PlayerTrainingPoints = new(TotalTrainingPoints, UnusedTrainingPoints);
            World = (World)world;
            ExpectedWorld = (World)world;
            x = xCoord;
            y = yCoord;
            z = zCoord;
            Facing = (byte)(facing / 0.0245433693f);
            //Strength = strength;
            //Stamina = stamina;
            //Agility = agility;
            //Dexterity = dexterity;
            //Wisdom = wisdom;
            //Intelligence = intelligence;
            //Charisma = charisma;
            HPFlag = true;
            Fishing = fishing;
            //TODO: Store HP Factors in database? Or should we calculate this once character loads in? BASE HP Factor + CM's, seems easy enough to calculate on the fly
            //Base HPFactor calculated in Character
            switch (EntityClass)
            {
                case Class.Paladin:
                case Class.Warrior:
                case Class.ShadowKnight:
                    HPFactor = 24;
                    break;

                case Class.Druid:
                case Class.Shaman:
                case Class.Cleric:
                    HPFactor = 13;
                    break;

                case Class.Ranger:
                case Class.Monk:
                case Class.Bard:
                case Class.Rogue:
                    HPFactor = 16;
                    break;

                case Class.Alchemist:
                case Class.Enchanter:
                case Class.Magician:
                case Class.Necromancer:
                case Class.Wizard:
                    HPFactor = 10;
                    break;
            }

            CurrentStats.Add(StatModifiers.TPSTR, tpStrength);
            CurrentStats.Add(StatModifiers.TPSTA, tpStamina);
            CurrentStats.Add(StatModifiers.TPAGI, tpAgility);
            CurrentStats.Add(StatModifiers.TPDEX, tpDexterity);
            CurrentStats.Add(StatModifiers.TPWIS, tpWisdom);
            CurrentStats.Add(StatModifiers.TPINT, tpIntelligence);
            CurrentStats.Add(StatModifiers.TPCHA, tpCharisma);


            if (playerFlags != null)
            {
                this.playerFlags = JsonSerializer.Deserialize<Dictionary<string, string>>(playerFlags);
            }

            if(completedQuests != null)
            {
                this.completedQuests = JsonSerializer.Deserialize<List<Quest>>(completedQuests);
            }

            if(activeQuests != null)
            {
                this.activeQuests = JsonSerializer.Deserialize<List<Quest>>(activeQuests);
            }

            characterSession = MySession;
        }

        public void UpdateFeatures(Session MySession, int hairColor, int hairLength, int hairStyle, int faceOption)
        {
            HairColor = hairColor;
            HairLength = hairLength;
            HairStyle = hairStyle;
            FaceOption = faceOption;
            MySession.MyCharacter = this;
        }

        public void DumpCharacter(ref BufferWriter writer)
        {
            //Start pulling data together
            writer.Write((byte)0);
            writer.WriteString(Encoding.UTF8, Tunaria);
            writer.Write7BitEncodedInt64(ServerID);
            writer.WriteString(Encoding.UTF8, CharName);
            writer.Write7BitEncodedInt64((int)EntityClass);
            writer.Write7BitEncodedInt64((int)EntityRace);
            writer.Write7BitEncodedInt64(Level);
            writer.Write7BitEncodedInt64(XPEarnedInThisLevel);
            writer.Write7BitEncodedInt64(totalDebt);
            writer.Write((byte)Breath);
            writer.Write7BitEncodedInt64(Inventory.Tunar);
            writer.Write7BitEncodedInt64(Bank.Tunar);
            writer.Write7BitEncodedInt64(PlayerTrainingPoints.RemainingTrainingPoints);
            writer.Write7BitEncodedInt64(BaseMaxStat);
            writer.Write7BitEncodedInt64((byte)World);
            writer.Write(x);
            writer.Write(y);
            writer.Write(z);
            writer.Write(FacingF);

            //Two unknown values must be packed on the end, can be updated later to
            //include database values when we figure out what it does.
            writer.Write(0.0f);
            writer.Write(0.0f);
        }
        public string GetPlayerFlags(Session mySession, string flagKey)
        {
            if (mySession.MyCharacter.playerFlags == null)
                return "noFlags";

            if (mySession.MyCharacter.playerFlags.ContainsKey(flagKey))
            {
                return mySession.MyCharacter.playerFlags[flagKey];
            }



            else
                return "noFlags";

        }

        public void SetPlayerFlag(Session mySession, string flagKey, string flagValue)
        {
            //TODO: Fix for characters without flags? bypassing for now
            if (mySession.MyCharacter.playerFlags == null)
                return;

            if (!mySession.MyCharacter.playerFlags.ContainsKey(flagKey))
                mySession.MyCharacter.playerFlags.Add(flagKey, flagValue);

            else if (mySession.MyCharacter.playerFlags.ContainsKey(flagKey))
                mySession.MyCharacter.playerFlags[flagKey] = flagValue;
            Logger.Info($"Flag {mySession.MyCharacter.playerFlags[flagKey]} set for {mySession.MyCharacter.CharName}");
        }

        public Character Copy() => (Character)MemberwiseClone();

    }
}
