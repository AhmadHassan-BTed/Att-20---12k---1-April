using MySql.Data.MySqlClient;
using System;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using ReturnHome.Utilities;
using ReturnHome.Server.Network;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.EntityObject.Actors;
using System.Text.Json;
using ReturnHome.Server.Opcodes.Messages.Server;
using ReturnHome.Server.EntityObject;
using ReturnHome.Server.EntityObject.Items;
using ReturnHome.Server.EntityObject.Stats;

namespace ReturnHome.Database.SQL
{
    //Class to handle all SQL Operations
    class CharacterSQL : SQLBase
    {
        public void CollectDefaultCharacters()
        {
            using var cmd = new MySqlCommand("GetDefaultCharacters", con);
            cmd.CommandType = CommandType.StoredProcedure;
            using MySqlDataReader rdr = cmd.ExecuteReader();
            while (rdr.Read())
            {
                List<KeyValuePair<StatModifiers, int>> temp = new();
                temp.Add(new KeyValuePair<StatModifiers, int>(StatModifiers.BaseSTR, rdr.GetInt32(11)));
                temp.Add(new KeyValuePair<StatModifiers, int>(StatModifiers.BaseSTA, rdr.GetInt32(12)));
                temp.Add(new KeyValuePair<StatModifiers, int>(StatModifiers.BaseAGI, rdr.GetInt32(13)));
                temp.Add(new KeyValuePair<StatModifiers, int>(StatModifiers.BaseDEX, rdr.GetInt32(14)));
                temp.Add(new KeyValuePair<StatModifiers, int>(StatModifiers.BaseWIS, rdr.GetInt32(15)));
                temp.Add(new KeyValuePair<StatModifiers, int>(StatModifiers.BaseINT, rdr.GetInt32(16)));
                temp.Add(new KeyValuePair<StatModifiers, int>(StatModifiers.BaseCHA, rdr.GetInt32(17)));
                DefaultCharacter.DefaultCharacterDict.TryAdd(((Race)rdr.GetInt32(0), (Class)rdr.GetInt32(1), (HumanType)rdr.GetInt32(2), (Sex)rdr.GetInt32(3)), new Character(rdr.GetInt32(0),
                                                                                                                                                                             rdr.GetInt32(1),
                                                                                                                                                                             rdr.GetInt32(2),
                                                                                                                                                                             rdr.GetInt32(3),
                                                                                                                                                                             rdr.GetFloat(4),
                                                                                                                                                                             rdr.GetFloat(5),
                                                                                                                                                                             rdr.GetFloat(6),
                                                                                                                                                                             rdr.GetFloat(7),
                                                                                                                                                                             rdr.GetFloat(8),
                                                                                                                                                                             rdr.GetInt32(9),
                                                                                                                                                                             rdr.GetInt32(10),
                                                                                                                                                                             temp));
            }
        }

        public void SavePlayerData(Character player, string playerFlags)
        {
            //Create new sql connection calling stored proc to update data
            using var SecondCmd = new MySqlCommand("UpdatePlayerData", con);
            SecondCmd.CommandType = CommandType.StoredProcedure;

            //charServerID used to save data for specific character, to be used
            //in a loop whereever this method is called if saving more than one character
            SecondCmd.Parameters.AddWithValue("charServerID", player.ServerID);
            SecondCmd.Parameters.AddWithValue("playerLevel", player.Level);
            //May need other default values but these hard set values are placeholders for now
            SecondCmd.Parameters.AddWithValue("newTotalXP", player.XPEarnedInThisLevel);
            SecondCmd.Parameters.AddWithValue("newDebt", player.totalDebt);
            SecondCmd.Parameters.AddWithValue("newBreath", player.Breath);
            SecondCmd.Parameters.AddWithValue("newTunar", player.Inventory.Tunar);
            SecondCmd.Parameters.AddWithValue("newBankTunar", player.Bank.Tunar);
            SecondCmd.Parameters.AddWithValue("newUnusedTP", player.PlayerTrainingPoints.RemainingTrainingPoints);
            SecondCmd.Parameters.AddWithValue("newTotalTP", player.PlayerTrainingPoints.TotalTrainingPoints);
            SecondCmd.Parameters.AddWithValue("newSpeed", player.Speed);
            SecondCmd.Parameters.AddWithValue("newWorld", (byte)player.World);
            SecondCmd.Parameters.AddWithValue("newX", player.x);
            SecondCmd.Parameters.AddWithValue("newY", player.y);
            SecondCmd.Parameters.AddWithValue("newZ", player.z);
            SecondCmd.Parameters.AddWithValue("newFacing", player.Facing);
            SecondCmd.Parameters.AddWithValue("newTPStrength", player.CurrentStats.dictionary[StatModifiers.TPSTR]);
            SecondCmd.Parameters.AddWithValue("newTPStamina", player.CurrentStats.dictionary[StatModifiers.TPSTA]);
            SecondCmd.Parameters.AddWithValue("newTPAgility", player.CurrentStats.dictionary[StatModifiers.TPAGI]);
            SecondCmd.Parameters.AddWithValue("newTPDexterity", player.CurrentStats.dictionary[StatModifiers.TPDEX]);
            SecondCmd.Parameters.AddWithValue("newTPWisdom", player.CurrentStats.dictionary[StatModifiers.TPWIS]);
            SecondCmd.Parameters.AddWithValue("newTPIntel", player.CurrentStats.dictionary[StatModifiers.TPINT]);
            SecondCmd.Parameters.AddWithValue("newTPCharisma", player.CurrentStats.dictionary[StatModifiers.TPCHA]);
            SecondCmd.Parameters.AddWithValue("newCurrentHP", player.CurrentHP);
            SecondCmd.Parameters.AddWithValue("newCurrentPower", player.CurrentPower);
            SecondCmd.Parameters.AddWithValue("newAC", player.CurrentAC);
            SecondCmd.Parameters.AddWithValue("newPoisonr", player.PoisonResist);
            SecondCmd.Parameters.AddWithValue("newDiseaser", player.DiseaseResist);
            SecondCmd.Parameters.AddWithValue("newFirer", player.FireResist);
            SecondCmd.Parameters.AddWithValue("newColdr", player.ColdResist);
            SecondCmd.Parameters.AddWithValue("newLightningr", player.LightningResist);
            SecondCmd.Parameters.AddWithValue("newArcaner", player.ArcaneResist);
            SecondCmd.Parameters.AddWithValue("newFishing", player.Fishing);
            SecondCmd.Parameters.AddWithValue("playerFlags", playerFlags);

            //Execute parameterized statement entering it into the DB
            //using MySqlDataReader SecondRdr = SecondCmd.ExecuteReader();
            SecondCmd.ExecuteNonQuery();
        }

        //Queries NPC database to populate world lists
        public List<Actor> WorldActors()
        {
            //create list to hold NPCs
            List<Actor> npcData = new List<Actor>();
            //Call stored procedure using connection
            using var cmd = new MySqlCommand("GetAllNPCs", con);
            //use command type stored proc
            cmd.CommandType = CommandType.StoredProcedure;
            //create reader and execute
            using MySqlDataReader rdr = cmd.ExecuteReader();
            //while rows exist from data set continue to read in data 
            while (rdr.Read())
            {
                //create new actor object using database rows
                Actor newActor = new Actor(
                    //name
                    rdr.GetString(0),
                    //xCoord
                    rdr.GetFloat(1),
                    //yCoord
                    rdr.GetFloat(2),
                    //zCoord
                    rdr.GetFloat(3),
                    //facing
                    rdr.GetInt32(4),
                    //world
                    rdr.GetInt32(5),
                    //modelid
                    rdr.GetInt32(7),
                    //size
                    rdr.GetFloat(8),
                    //primary
                    rdr.GetInt32(9),
                    //secondary
                    rdr.GetInt32(10),
                    //shield
                    rdr.GetInt32(11),
                    //hair_color
                    rdr.GetInt32(12),
                    //hair_length
                    rdr.GetInt32(13),
                    //hair_style
                    rdr.GetInt32(14),
                    //level
                    rdr.GetInt32(15),
                    //torso
                    rdr.GetInt32(16),
                    //forearms
                    rdr.GetInt32(17),
                    //gloves
                    rdr.GetInt32(18),
                    //legs
                    rdr.GetInt32(19),
                    //feet
                    rdr.GetInt32(20),
                    //head
                    rdr.GetInt32(21),
                    //NPC Type
                    //Should be a ushort but throws an overflow error, needs to be looked at eventually, cast to ushort in Actor.cs
                    rdr.GetUInt32(22));
                //add the created actor to the npcData list
                npcData.Add(newActor);

            }
            rdr.Close();

            //Second SQL command and reader
            using var SecondCmd = new MySqlCommand("GetActorGear", con);
            SecondCmd.CommandType = CommandType.StoredProcedure;
            using MySqlDataReader SecondRdr = SecondCmd.ExecuteReader();

            string actorName;

            //Use second reader to iterate through character gear and assign to character attributes
            while (SecondRdr.Read())
            {
                //Hold charactervalue so we have names to compare against 
                actorName = SecondRdr.GetString(0);

                //Iterate through characterData list finding charnames that exist
                Actor thisActor = npcData.Find(i => Equals(i.CharName, actorName));

                if (thisActor.Inventory == null)
                    thisActor.Inventory = new(0);

                Item ThisItem = new Item(
                  //Stacksleft
                  SecondRdr.GetInt32(1),
                  //RemainingHP
                  SecondRdr.GetInt32(2),
                  //Charges
                  SecondRdr.GetInt32(3),
                  //Equipment Location
                  SecondRdr.GetInt32(4),
                  //Location (Bank, self, auction etc)
                  SecondRdr.GetByte(5),
                  //Location in inventory
                  SecondRdr.GetByte(6),
                  //ItemID
                  SecondRdr.GetInt32(7),
                  //Item cost 
                  SecondRdr.GetUInt32(8),
                  //ItemIcon
                  SecondRdr.GetInt32(9),
                  //Itempattern equipslot
                  SecondRdr.GetInt32(10),
                  //Attack Type 
                  SecondRdr.GetInt32(11),
                  //WeaponDamage
                  SecondRdr.GetInt32(12),
                  //MaxHP of item 
                  SecondRdr.GetInt32(13),
                  //Tradeable?
                  SecondRdr.GetInt32(14),
                  //Rentable
                  SecondRdr.GetInt32(15),
                  //Craft Item
                  SecondRdr.GetInt32(16),
                  //Lore item 
                  SecondRdr.GetInt32(17),
                  //Level requirement 
                  SecondRdr.GetInt32(18),
                  //Max stack of item 
                  SecondRdr.GetInt32(19),
                  //ItemName
                  SecondRdr.GetString(20),
                  //Item Description
                  SecondRdr.GetString(21),
                  //Duration
                  SecondRdr.GetInt32(22),
                  //useable classes
                  SecondRdr.GetInt32(23),
                  //useable races
                  SecondRdr.GetInt32(24),
                  //Proc Animation
                  SecondRdr.GetInt32(25),
                  new List<KeyValuePair<StatModifiers, int>>() { new KeyValuePair<StatModifiers, int>(StatModifiers.STR, SecondRdr.GetInt32(26)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.STA, SecondRdr.GetInt32(27)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.AGI, SecondRdr.GetInt32(28)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.DEX, SecondRdr.GetInt32(29)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.WIS, SecondRdr.GetInt32(30)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.INT, SecondRdr.GetInt32(31)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.CHA, SecondRdr.GetInt32(32)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.HPMAX, SecondRdr.GetInt32(33)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.POWMAX, SecondRdr.GetInt32(34)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.PoT, SecondRdr.GetInt32(35)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.HoT, SecondRdr.GetInt32(36)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.AC, SecondRdr.GetInt32(37)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.PoisonResistance, SecondRdr.GetInt32(38)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.DiseaseResistance, SecondRdr.GetInt32(39)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.FireResistance, SecondRdr.GetInt32(40)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.ColdResistance, SecondRdr.GetInt32(41)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.LightningResistance, SecondRdr.GetInt32(42)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.ArcaneResistance, SecondRdr.GetInt32(43))
                                                               },
                  //Model
                  SecondRdr.GetInt32(44),
                  //Color
                  SecondRdr.GetUInt32(45));

                //If this is 1, it needs to go to inventory
                if (ThisItem.Location == 1)
                {
                    thisActor.Inventory.AddItem(ThisItem);
                }
            }

            SecondRdr.Close();
            //return the list of actors from DB
            return npcData;
        }
        //Class to pull characters from DB via serverid
        public List<Character> AccountCharacters(Session session)
        {
            //Holds list of characters for whole class
            List<Character> characterData = new List<Character>();

            //Queries DB for all characters and their necessary attributes  to generate character select
            //Later should convert to a SQL stored procedure if possible.
            //Currently pulls ALL charcters, will pull characters based on accountID.
            using var cmd = new MySqlCommand("GetAccountCharacters", con);
            cmd.CommandType = CommandType.StoredProcedure;
            cmd.Parameters.AddWithValue("pAccountID", session.AccountID);
            using MySqlDataReader rdr = cmd.ExecuteReader();

            //string to hold local charcter Name
            string charName;

            //Read through results from query populating character data needed for character select
            while (rdr.Read())
            {
                //Instantiate character object
                Character newCharacter = new Character
                (
                    //charName 1
                    rdr.GetString(0),
                    //serverid 2
                    rdr.GetInt32(1),
                    //modelid 3
                    rdr.GetInt32(2),
                    //tclass 4
                    rdr.GetInt32(3),
                    //race 5
                    rdr.GetInt32(4),
                    //humType 6
                    rdr.GetInt32(5),
                    //level 7
                    rdr.GetInt32(6),
                    //haircolor 8
                    rdr.GetInt32(7),
                    //hairlength 9
                    rdr.GetInt32(8),
                    //hairstyle 10
                    rdr.GetInt32(9),
                    //faceoption 11
                    rdr.GetInt32(10),
                    //sex
                    rdr.GetInt32(11),
                    //totalXP 12
                    rdr.GetInt32(12),
                    //debt 13
                    rdr.GetInt32(13),
                    //breath 14
                    rdr.GetInt32(14),
                    //tunar 15
                    rdr.GetInt32(15),
                    //bankTunar 16
                    rdr.GetInt32(16),
                    //unusedTP 17
                    rdr.GetInt32(17),
                    //totalTP 18
                    rdr.GetInt32(18),
                    rdr.GetFloat(19),
                    //world 19
                    rdr.GetInt32(20),
                    //x 20
                    rdr.GetFloat(21),
                    //y 21
                    rdr.GetFloat(22),
                    //z 22
                    rdr.GetFloat(23),
                    //facing 23
                    rdr.GetFloat(24),
                    //strength 24
                    rdr.GetInt32(25),
                    //stamina 25
                    rdr.GetInt32(26),
                    //agility 26
                    rdr.GetInt32(27),
                    //dexterity 27
                    rdr.GetInt32(28),
                    //wisdom 28
                    rdr.GetInt32(29),
                    //intel 29
                    rdr.GetInt32(30),
                    //charisma 30
                    rdr.GetInt32(31),
                    //currentHP 31
                    rdr.GetInt32(32),
                    //currentPower 32
                    rdr.GetInt32(33),
                    //ac 33
                    rdr.GetInt32(34),
                    //poisonr 34
                    rdr.GetInt32(35),
                    //diseaser 35
                    rdr.GetInt32(36),
                    //firer 36
                    rdr.GetInt32(37),
                    //coldr 37
                    rdr.GetInt32(38),
                    //lightningr 38
                    rdr.GetInt32(39),
                    //arcaner 39
                    rdr.GetInt32(40),
                    //fishing 40
                    rdr.GetInt32(41),
                    //flags 41
                    rdr.GetString(42),
                    //58
                    session);


                //Add character attribute data to charaterData List
                //Console.WriteLine(newCharacter.CharName);
                characterData.Add(newCharacter);
            }
            //Close first reader
            rdr.Close();

            //Second SQL command and reader
            using var SecondCmd = new MySqlCommand("GetCharacterGear", con);
            SecondCmd.CommandType = CommandType.StoredProcedure;
            SecondCmd.Parameters.AddWithValue("pAccountID", session.AccountID);
            using MySqlDataReader SecondRdr = SecondCmd.ExecuteReader();

            //Use second reader to iterate through character gear and assign to character attributes
            while (SecondRdr.Read())
            {
                //Hold charactervalue so we have names to compare against 
                charName = SecondRdr.GetString(0);

                //Iterate through characterData list finding charnames that exist
                Character thisChar = characterData.Find(i => Equals(i.CharName, charName));

                Item ThisItem = new Item(
                  //Stacksleft
                  SecondRdr.GetInt32(1),
                  //RemainingHP
                  SecondRdr.GetInt32(2),
                  //Charges
                  SecondRdr.GetInt32(3),
                  //Equipment Location
                  SecondRdr.GetInt32(4),
                  //Location (Bank, self, auction etc)
                  SecondRdr.GetByte(5),
                  //Location in inventory
                  SecondRdr.GetByte(6),
                  //ItemID
                  SecondRdr.GetInt32(7),
                  //Item cost 
                  SecondRdr.GetUInt32(8),
                  //ItemIcon
                  SecondRdr.GetInt32(9),
                  //Itempattern equipslot
                  SecondRdr.GetInt32(10),
                  //Attack Type 
                  SecondRdr.GetInt32(11),
                  //WeaponDamage
                  SecondRdr.GetInt32(12),
                  //MaxHP of item 
                  SecondRdr.GetInt32(13),
                  //Tradeable?
                  SecondRdr.GetInt32(14),
                  //Rentable
                  SecondRdr.GetInt32(15),
                  //Craft Item
                  SecondRdr.GetInt32(16),
                  //Lore item 
                  SecondRdr.GetInt32(17),
                  //Level requirement 
                  SecondRdr.GetInt32(18),
                  //Max stack of item 
                  SecondRdr.GetInt32(19),
                  //ItemName
                  SecondRdr.GetString(20),
                  //Item Description
                  SecondRdr.GetString(21),
                  //Duration
                  SecondRdr.GetInt32(22),
                  //useable classes
                  SecondRdr.GetInt32(23),
                  //useable races
                  SecondRdr.GetInt32(24),
                  //Proc Animation
                  SecondRdr.GetInt32(25),
                  new List<KeyValuePair<StatModifiers, int>>() { new KeyValuePair<StatModifiers, int>(StatModifiers.STR, SecondRdr.GetInt32(26)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.STA, SecondRdr.GetInt32(27)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.AGI, SecondRdr.GetInt32(28)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.DEX, SecondRdr.GetInt32(29)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.WIS, SecondRdr.GetInt32(30)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.INT, SecondRdr.GetInt32(31)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.CHA, SecondRdr.GetInt32(32)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.HPMAX, SecondRdr.GetInt32(33)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.POWMAX, SecondRdr.GetInt32(34)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.PoT, SecondRdr.GetInt32(35)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.HoT, SecondRdr.GetInt32(36)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.AC, SecondRdr.GetInt32(37)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.PoisonResistance, SecondRdr.GetInt32(38)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.DiseaseResistance, SecondRdr.GetInt32(39)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.FireResistance, SecondRdr.GetInt32(40)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.ColdResistance, SecondRdr.GetInt32(41)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.LightningResistance, SecondRdr.GetInt32(42)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.ArcaneResistance, SecondRdr.GetInt32(43))
                  },
                  //Model
                  SecondRdr.GetInt32(44),
                  //Color
                  SecondRdr.GetUInt32(45));

                //If this is 1, it needs to go to inventory
                //Only this one one is needed for character select data
                if (ThisItem.Location == 1)
                {
                    thisChar.Inventory.AddItem(ThisItem);
                }

                
                //If this is 2, it needs to go to the Bank
                else if (ThisItem.Location == 2)
                {
                    thisChar.Bank.AddItem(ThisItem);
                }
                //If this is 4, it needs to go to "Auction items". This should be items you are selling and still technically in your possession
                else if (ThisItem.Location == 4)
                {
                    thisChar.AuctionItems.Add(ThisItem);
                }
            }

            SecondRdr.Close();
            //foreach (Character character in characterData.OrderBy(newCharacter => newCharacter.CharName)) Console.WriteLine(character);

            //return Character Data with characters and gear.
            return characterData;
        }

        public Character AcquireCharacter(Session session, int serverID)
        {
            //Holds list of characters for whole class
            Character selectedCharacter = null;

            //Queries DB for all characters and their necessary attributes  to generate character select
            //Later should convert to a SQL stored procedure if possible.
            //Currently pulls ALL charcters, will pull characters based on accountID.
            using var cmd = new MySqlCommand("GetAccountCharacters", con);
            cmd.CommandType = CommandType.StoredProcedure;
            cmd.Parameters.AddWithValue("pAccountID", session.AccountID);
            using MySqlDataReader rdr = cmd.ExecuteReader();

            //Read through results from query populating character data needed for character select
            while (rdr.Read())
            {
                if (rdr.GetInt32(1) == serverID)
                {
                    //Instantiate character object
                    selectedCharacter = new Character
                    (
                        //charName 1
                        rdr.GetString(0),
                        //serverid 2
                        rdr.GetInt32(1),
                        //modelid 3
                        rdr.GetInt32(2),
                        //tclass 4
                        rdr.GetInt32(3),
                        //race 5
                        rdr.GetInt32(4),
                        //humType 6
                        rdr.GetInt32(5),
                        //level 7
                        rdr.GetInt32(6),
                        //haircolor 8
                        rdr.GetInt32(7),
                        //hairlength 9
                        rdr.GetInt32(8),
                        //hairstyle 10
                        rdr.GetInt32(9),
                        //faceoption 11
                        rdr.GetInt32(10),
                        //sex
                        rdr.GetInt32(11),
                        //totalXP 12
                        rdr.GetInt32(12),
                        //debt 13
                        rdr.GetInt32(13),
                        //breath 14
                        rdr.GetInt32(14),
                        //tunar 15
                        rdr.GetInt32(15),
                        //bankTunar 16
                        rdr.GetInt32(16),
                        //unusedTP 17
                        rdr.GetInt32(17),
                        //totalTP 18
                        rdr.GetInt32(18),
                        //world 19
                        rdr.GetFloat(19),
                        //world 19
                        rdr.GetInt32(20),
                        //x 20
                        rdr.GetFloat(21),
                        //y 21
                        rdr.GetFloat(22),
                        //z 22
                        rdr.GetFloat(23),
                        //facing 23
                        rdr.GetFloat(24),
                        //strength 24
                        rdr.GetInt32(25),
                        //stamina 25
                        rdr.GetInt32(26),
                        //agility 26
                        rdr.GetInt32(27),
                        //dexterity 27
                        rdr.GetInt32(28),
                        //wisdom 28
                        rdr.GetInt32(29),
                        //intel 29
                        rdr.GetInt32(30),
                        //charisma 30
                        rdr.GetInt32(31),
                        //currentHP 31
                        rdr.GetInt32(32),
                        //currentPower 32
                        rdr.GetInt32(33),
                        //ac 33
                        rdr.GetInt32(34),
                        //poisonr 34
                        rdr.GetInt32(35),
                        //diseaser 35
                        rdr.GetInt32(36),
                        //firer 36
                        rdr.GetInt32(37),
                        //coldr 37
                        rdr.GetInt32(38),
                        //lightningr 38
                        rdr.GetInt32(39),
                        //arcaner 39
                        rdr.GetInt32(40),
                        //fishing 40
                        rdr.GetInt32(41),
                        //flags 41
                        rdr.GetString(42),
                        //58
                        session);
                        break;
                }
            }

            //Close first reader
            rdr.Close();

            //Second SQL command and reader
            using var SecondCmd = new MySqlCommand("GetCharacterGear", con);
            SecondCmd.CommandType = CommandType.StoredProcedure;
            SecondCmd.Parameters.AddWithValue("pAccountID", session.AccountID);
            using MySqlDataReader SecondRdr = SecondCmd.ExecuteReader();

            //Use second reader to iterate through character gear and assign to character attributes
            while (SecondRdr.Read())
            {
                //Hold character value so we have names to compare against 
                if (SecondRdr.GetString(0) == selectedCharacter.CharName)
                {
                    Item ThisItem = new Item(
                      //Stacksleft
                      SecondRdr.GetInt32(1),
                      //RemainingHP
                      SecondRdr.GetInt32(2),
                      //Charges
                      SecondRdr.GetInt32(3),
                      //Equipment Location
                      SecondRdr.GetInt32(4),
                      //Location (Bank, self, auction etc)
                      SecondRdr.GetByte(5),
                      //Location in inventory
                      SecondRdr.GetByte(6),
                      //ItemID
                      SecondRdr.GetInt32(7),
                      //Item cost 
                      SecondRdr.GetUInt32(8),
                      //ItemIcon
                      SecondRdr.GetInt32(9),
                      //Itempattern equipslot
                      SecondRdr.GetInt32(10),
                      //Attack Type 
                      SecondRdr.GetInt32(11),
                      //WeaponDamage
                      SecondRdr.GetInt32(12),
                      //MaxHP of item 
                      SecondRdr.GetInt32(13),
                      //Tradeable?
                      SecondRdr.GetInt32(14),
                      //Rentable
                      SecondRdr.GetInt32(15),
                      //Craft Item
                      SecondRdr.GetInt32(16),
                      //Lore item 
                      SecondRdr.GetInt32(17),
                      //Level requirement 
                      SecondRdr.GetInt32(18),
                      //Max stack of item 
                      SecondRdr.GetInt32(19),
                      //ItemName
                      SecondRdr.GetString(20),
                      //Item Description
                      SecondRdr.GetString(21),
                      //Duration
                      SecondRdr.GetInt32(22),
                      //useable classes
                      SecondRdr.GetInt32(23),
                      //useable races
                      SecondRdr.GetInt32(24),
                      //Proc Animation
                      SecondRdr.GetInt32(25),
                      new List<KeyValuePair<StatModifiers, int>>() { new KeyValuePair<StatModifiers, int>(StatModifiers.STR, SecondRdr.GetInt32(26)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.STA, SecondRdr.GetInt32(27)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.AGI, SecondRdr.GetInt32(28)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.DEX, SecondRdr.GetInt32(29)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.WIS, SecondRdr.GetInt32(30)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.INT, SecondRdr.GetInt32(31)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.CHA, SecondRdr.GetInt32(32)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.HPMAX, SecondRdr.GetInt32(33)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.POWMAX, SecondRdr.GetInt32(34)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.PoT, SecondRdr.GetInt32(35)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.HoT, SecondRdr.GetInt32(36)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.AC, SecondRdr.GetInt32(37)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.PoisonResistance, SecondRdr.GetInt32(38)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.DiseaseResistance, SecondRdr.GetInt32(39)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.FireResistance, SecondRdr.GetInt32(40)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.ColdResistance, SecondRdr.GetInt32(41)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.LightningResistance, SecondRdr.GetInt32(42)),
                                                                 new KeyValuePair<StatModifiers, int>(StatModifiers.ArcaneResistance, SecondRdr.GetInt32(43))
                      },
                      //Model
                      SecondRdr.GetInt32(44),
                      //Color
                      SecondRdr.GetUInt32(45));

                    //If this is 1, it needs to go to inventory
                    if (ThisItem.Location == 1)
                        selectedCharacter.Inventory.AddItem(ThisItem);

                    //If this is 2, it needs to go to the Bank
                    else if (ThisItem.Location == 2)
                        selectedCharacter.Bank.AddItem(ThisItem);

                    //If this is 4, it needs to go to "Auction items". This should be items you are selling and still technically in your possession
                    else if (ThisItem.Location == 4)
                        selectedCharacter.AuctionItems.Add(ThisItem);
                }

                //With all of the gear in inventory, cycle over and equip it if equipped

            }
            SecondRdr.Close();

            //return Character Data with characters and gear.
            return selectedCharacter;
        }

        public void GetPlayerSpells(Session session)
        {
            //Queries DB for all characters and their necessary attributes  to generate character select
            //Later should convert to a SQL stored procedure if possible.
            //Currently pulls ALL charcters, will pull characters based on accountID.
            using var cmd = new MySqlCommand("GetCharSpells", con);
            cmd.CommandType = CommandType.StoredProcedure;
            cmd.Parameters.AddWithValue("charID", session.MyCharacter.ServerID);
            using MySqlDataReader rdr = cmd.ExecuteReader();

            while (rdr.Read())
            {
                //Instantiate new character object, not to be confused with a newly created character
                Spell thisSpell = new Spell
                (
                     //SpellID
                     rdr.GetInt32(0),
                     //AddedOrder
                     rdr.GetInt32(1),
                     //OnHotBar
                     rdr.GetInt32(2),
                     //WhereOnHotBar
                     rdr.GetInt32(3),
                     //Unk1
                     rdr.GetInt32(4),
                     //ShowHide
                     rdr.GetInt32(5),
                     //AbilityLevel
                     rdr.GetInt32(6),
                     //Unk2
                     rdr.GetInt32(7),
                     //Unk3
                     rdr.GetInt32(8),
                     //SpellRange
                     rdr.GetInt32(9),
                     //CastTime
                     rdr.GetInt32(10),
                     //Power
                     rdr.GetInt32(11),
                     //IconColor
                     rdr.GetInt32(12),
                     //Icon
                     rdr.GetInt32(13),
                     //SpellScope
                     rdr.GetInt32(14),
                     //Recast
                     rdr.GetInt32(15),
                     //EqpRequirement
                     rdr.GetInt32(16),
                     //SpellName
                     rdr.GetString(17),
                     //SpellDesc
                     rdr.GetString(18)
                );

                //Add these spells to player book
                session.MyCharacter.MySpells.Add(thisSpell);
            }

            rdr.Close();
        }

        public void GetPlayerHotkeys(Session session)
        {
            //Queries DB for all characters and their necessary attributes  to generate character select
            //Later should convert to a SQL stored procedure if possible.
            //Currently pulls ALL charcters, will pull characters based on accountID.
            using var cmd = new MySqlCommand("GetCharHotkeys", con);
            cmd.CommandType = CommandType.StoredProcedure;
            cmd.Parameters.AddWithValue("charID", session.MyCharacter.ObjectID);
            using MySqlDataReader rdr = cmd.ExecuteReader();

            while (rdr.Read())
            {
                //Instantiate new character object, not to be confused with a newly created character
                Hotkey thisHotkey = new Hotkey
                (
                     //direction
                     rdr.GetString(0),
                     //Nlabel
                     rdr.GetString(1),
                     //Nmessage
                     rdr.GetString(2),
                     //Wlabel
                     rdr.GetString(3),
                     //Wmessage
                     rdr.GetString(4),
                     //Elabel
                     rdr.GetString(5),
                     //Emessage
                     rdr.GetString(6),
                     //Slabel
                     rdr.GetString(7),
                     //Smessage
                     rdr.GetString(8)
                );

                //Add these spells to player book
                session.MyCharacter.MyHotkeys.Add(thisHotkey);
            }
            rdr.Close();
        }

        public void GetPlayerWeaponHotbar(Session session)
        {
            //Queries DB for all characters and their necessary attributes  to generate character select
            //Later should convert to a SQL stored procedure if possible.
            //Currently pulls ALL charcters, will pull characters based on accountID.
            using var cmd = new MySqlCommand("GetCharWeaponHotbar", con);
            cmd.CommandType = CommandType.StoredProcedure;
            cmd.Parameters.AddWithValue("charID", session.MyCharacter.ObjectID);
            using MySqlDataReader rdr = cmd.ExecuteReader();

            while (rdr.Read())
            {
                //Instantiate new character object, not to be confused with a newly created character
                WeaponHotbar thisHotbar = new WeaponHotbar
                (
                     //Hotbar name
                     rdr.GetString(0),
                     //primaryID
                     rdr.GetInt32(1),
                     //secondaryID
                     rdr.GetInt32(2)
                );

                //Add these weapon hotbars
                session.MyCharacter.WeaponHotbars.Add(thisHotbar);
            }
            rdr.Close();

            //If less then 4 weapon hotbars, need constructor dummies untill 4 total
            for (int i = session.MyCharacter.WeaponHotbars.Count(); i < 4; i++)
            {
                session.MyCharacter.WeaponHotbars.Add(new WeaponHotbar());
            }
        }

        //Method to delete character from player's account
        public void DeleteCharacter(int serverid, Session session)
        {
            //Creates var to store a MySQlcommand with the query and connection parameters.
            using var cmd = new MySqlCommand("DeleteCharacter", con);
            cmd.CommandType = CommandType.StoredProcedure;
            cmd.Parameters.AddWithValue("cServerID", serverid);

            //Executes a reader on the previous var.
            using MySqlDataReader rdr = cmd.ExecuteReader();
            rdr.Close();
            //Log which character serverid was deleted
            Logger.Info($"Deleted Character with ServerID: {serverid}");

            //Create a new list of characters after deletion
            List<Character> MyCharacterList = AccountCharacters(session);

            //Don't close connection because we recreate character list and resend it, it handles closing connection

            //Send Fresh Character Listing
            ServerCreateCharacterList.CreateCharacterList(MyCharacterList, session);
        }

        //Method to check if characters name exist in the DB
        public string CheckName(string CharName)
        {
            //Create local var to hold character name from DB
            String TestCharName = "";
            //Set and open SQL con

            //SQL query to check if a name exists in the DB or not
            using var CheckNameCmd = new MySqlCommand("CheckName", con);
            CheckNameCmd.CommandType = CommandType.StoredProcedure;
            //Assigns variable to in line @Sql variable
            CheckNameCmd.Parameters.AddWithValue("@CharacterName", CharName);
            //Executes the SQL reader
            using MySqlDataReader CheckNameRdr = CheckNameCmd.ExecuteReader();

            //Reads through the returned rows(should only be 1 or none) and sets variable to returned value
            while (CheckNameRdr.Read())
            {
                TestCharName = CheckNameRdr.GetString(0);
            }

            //Return the matched name if it existed in the DB.
            return TestCharName;
        }

        //Method to create new character for player's account
        public void CreateCharacter(Session session, Character charCreation)
        {
            Console.WriteLine("Trying to write flags");
            charCreation.playerFlags = new Dictionary<string, bool>()
            {
                { "NewPlayerControls", true }
            };
            string serializedPlayerFlags = Newtonsoft.Json.JsonConvert.SerializeObject(charCreation.playerFlags);
            Console.WriteLine(serializedPlayerFlags);

            //Create second command using second connection and char insert query string
            using var SecondCmd = new MySqlCommand("CreateCharacter", con);
            SecondCmd.CommandType = CommandType.StoredProcedure;

            //Add all character attributes for new character creation to parameterized values
            SecondCmd.Parameters.AddWithValue("@charName", charCreation.CharName);
            //Needs to be session.AccountID once CharacterSelect shows characters off true AccountID.
            SecondCmd.Parameters.AddWithValue("AccountID", session.AccountID);
            SecondCmd.Parameters.AddWithValue("ModelID", charCreation.ModelID);
            SecondCmd.Parameters.AddWithValue("TClass", (byte)charCreation.EntityClass);
            SecondCmd.Parameters.AddWithValue("Race", (byte)charCreation.EntityRace);
            SecondCmd.Parameters.AddWithValue("HumType", (byte)charCreation.EntityHumanType);
            SecondCmd.Parameters.AddWithValue("Level", charCreation.Level);
            SecondCmd.Parameters.AddWithValue("HairColor", charCreation.HairColor);
            SecondCmd.Parameters.AddWithValue("HairLength", charCreation.HairLength);
            SecondCmd.Parameters.AddWithValue("HairStyle", charCreation.HairStyle);
            SecondCmd.Parameters.AddWithValue("FaceOption", charCreation.FaceOption);
            SecondCmd.Parameters.AddWithValue("Sex", (byte)charCreation.EntitySex);
            //May need other default values but these hard set values are placeholders for now
            SecondCmd.Parameters.AddWithValue("TotalXP", charCreation.TotalXP);
            SecondCmd.Parameters.AddWithValue("Debt", charCreation.totalDebt);
            SecondCmd.Parameters.AddWithValue("Breath", charCreation.Breath);
            SecondCmd.Parameters.AddWithValue("Tunar", charCreation.Inventory.Tunar);
            SecondCmd.Parameters.AddWithValue("BankTunar", charCreation.Bank.Tunar);
            SecondCmd.Parameters.AddWithValue("UnusedTP", charCreation.PlayerTrainingPoints.RemainingTrainingPoints);
            SecondCmd.Parameters.AddWithValue("TotalTP", charCreation.PlayerTrainingPoints.TotalTrainingPoints);
            SecondCmd.Parameters.AddWithValue("Speed", charCreation.Speed);
            SecondCmd.Parameters.AddWithValue("World", (byte)charCreation.World);
            SecondCmd.Parameters.AddWithValue("X", charCreation.x);
            SecondCmd.Parameters.AddWithValue("Y", charCreation.y);
            SecondCmd.Parameters.AddWithValue("Z", charCreation.z);
            SecondCmd.Parameters.AddWithValue("Facing", charCreation.Facing);
            SecondCmd.Parameters.AddWithValue("TPStrength", charCreation.CurrentStats.dictionary[StatModifiers.TPSTR]);
            SecondCmd.Parameters.AddWithValue("TPStamina", charCreation.CurrentStats.dictionary[StatModifiers.TPSTA]);
            SecondCmd.Parameters.AddWithValue("TPAgility", charCreation.CurrentStats.dictionary[StatModifiers.TPAGI]);
            SecondCmd.Parameters.AddWithValue("TPDexterity", charCreation.CurrentStats.dictionary[StatModifiers.TPDEX]);
            SecondCmd.Parameters.AddWithValue("TPWisdom", charCreation.CurrentStats.dictionary[StatModifiers.TPWIS]);
            SecondCmd.Parameters.AddWithValue("TPIntelligence", charCreation.CurrentStats.dictionary[StatModifiers.TPINT]);
            SecondCmd.Parameters.AddWithValue("TPCharisma", charCreation.CurrentStats.dictionary[StatModifiers.TPCHA]);
            //May need other default or calculated values but these hard set values are placeholders for now
            SecondCmd.Parameters.AddWithValue("CurrentHP", charCreation.CurrentHP);
            SecondCmd.Parameters.AddWithValue("CurrentPower", charCreation.CurrentPower);
            SecondCmd.Parameters.AddWithValue("Ac", charCreation.BaseAC);
            SecondCmd.Parameters.AddWithValue("PoisonR", 40);
            SecondCmd.Parameters.AddWithValue("DiseaseR", 40);
            SecondCmd.Parameters.AddWithValue("FireR", 40);
            SecondCmd.Parameters.AddWithValue("ColdR", 40);
            SecondCmd.Parameters.AddWithValue("LightningR", 40);
            SecondCmd.Parameters.AddWithValue("ArcaneR", 40);
            SecondCmd.Parameters.AddWithValue("Fishing", 0);
            SecondCmd.Parameters.AddWithValue("playerFlags", serializedPlayerFlags);
            //Execute parameterized statement entering it into the DB
            SecondCmd.ExecuteNonQuery();

            //Don't close connection because we have character list generated next
            List<Character> MyCharacterList = AccountCharacters(session);

            //Send Fresh Character Listing
            ServerCreateCharacterList.CreateCharacterList(MyCharacterList, session);
        }
    }
}
