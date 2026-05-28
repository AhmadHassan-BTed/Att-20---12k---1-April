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
using ReturnHome.Server.Managers;
using MySqlDataAdapter = MySql.Data.MySqlClient.MySqlDataAdapter;

namespace ReturnHome.Database.SQL
{
    //Class to handle all SQL Operations
    class CharacterSQL : SQLBase
    {
        public void CollectDefaultCharacters()
        {
            List<Hotkey> hotkeys = new List<Hotkey>();
            //Get Default hotkey here
            using var cmd = new MySqlCommand("GetCharHotkeys", con);
            cmd.CommandType = CommandType.StoredProcedure;
            cmd.Parameters.AddWithValue("charID", 1);
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
                hotkeys.Add(thisHotkey);
            }
            rdr.Close();

            using var cmd2 = new MySqlCommand("GetDefaultCharacters", con);
            cmd2.CommandType = CommandType.StoredProcedure;
            using MySqlDataReader rdr2 = cmd2.ExecuteReader();
            while (rdr2.Read())
            {
                List<KeyValuePair<StatModifiers, int>> temp = new();
                temp.Add(new KeyValuePair<StatModifiers, int>(StatModifiers.BaseSTR, rdr2.GetInt32(11)));
                temp.Add(new KeyValuePair<StatModifiers, int>(StatModifiers.BaseSTA, rdr2.GetInt32(12)));
                temp.Add(new KeyValuePair<StatModifiers, int>(StatModifiers.BaseAGI, rdr2.GetInt32(13)));
                temp.Add(new KeyValuePair<StatModifiers, int>(StatModifiers.BaseDEX, rdr2.GetInt32(14)));
                temp.Add(new KeyValuePair<StatModifiers, int>(StatModifiers.BaseWIS, rdr2.GetInt32(15)));
                temp.Add(new KeyValuePair<StatModifiers, int>(StatModifiers.BaseINT, rdr2.GetInt32(16)));
                temp.Add(new KeyValuePair<StatModifiers, int>(StatModifiers.BaseCHA, rdr2.GetInt32(17)));
                DefaultCharacter.DefaultCharacterDict.TryAdd(((Race)rdr2.GetInt32(0), (Class)rdr2.GetInt32(1), (HumanType)rdr2.GetInt32(2), (Sex)rdr2.GetInt32(3)), new Character(rdr2.GetInt32(0),
                                                                                                                                                                             rdr2.GetInt32(1),
                                                                                                                                                                             rdr2.GetInt32(2),
                                                                                                                                                                             rdr2.GetInt32(3),
                                                                                                                                                                             rdr2.GetFloat(4),
                                                                                                                                                                             rdr2.GetFloat(5),
                                                                                                                                                                             rdr2.GetFloat(6),
                                                                                                                                                                             rdr2.GetFloat(7),
                                                                                                                                                                             rdr2.GetFloat(8),
                                                                                                                                                                             rdr2.GetInt32(9),
                                                                                                                                                                             rdr2.GetInt32(10),
                                                                                                                                                                             temp, hotkeys));
            }
        }

        public void SavePlayerData(Character player)
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
            SecondCmd.Parameters.AddWithValue("playerFlags", (string)JsonSerializer.Serialize(player.playerFlags));
            SecondCmd.Parameters.AddWithValue("completedQuests", (string)JsonSerializer.Serialize<IList<Quest>>(player.completedQuests));
            SecondCmd.Parameters.AddWithValue("activeQuests", (string)JsonSerializer.Serialize<IList<Quest>>(player.activeQuests));

            //Execute parameterized statement entering it into the DB
            //using MySqlDataReader SecondRdr = SecondCmd.ExecuteReader();
            SecondCmd.ExecuteNonQuery();

            //SavePlayerItems(player);
        }

        /*public void SavePlayerItems(Character player)
        {

            DataTable dt = new DataTable("charInv");
            dt.Clear();
            dt.Columns.Add("serverID");
            dt.Columns.Add("stackLeft");
            dt.Columns.Add("remainHP");
            dt.Columns.Add("remainCharge");
            dt.Columns.Add("patternID");
            dt.Columns.Add("equipLoc");
            dt.Columns.Add("location");
            dt.Columns.Add("listNumber");
            DataRow dr = null;
            int auto = 17;
            foreach (ClientItemWrapper item in player.Inventory.itemContainer)
            {
                dr = dt.NewRow();
                dr["serverID"] = player.ServerID;
                dr["stackLeft"] = item.item.StackLeft;
                dr["remainHP"] = item.item.RemainingHP;
                dr["remainCharge"] = item.item.Charges;
                dr["patternID"] = item.item.Pattern.ItemID;
                dr["equiploc"] = (byte)item.item.EquipLocation;
                dr["location"] = item.item.Location;
                dr["listNumber"] = item.item.ClientIndex;
                dt.Rows.Add(dr);
            }

            printDataTable(dt);


            MySqlDataAdapter da = new MySqlDataAdapter("Select * from charInventory", con);
            MySqlCommandBuilder cb = new MySqlCommandBuilder(da);
            //da.Fill(dt);
            DataTable changes = dt.GetChanges();
            da.Update(changes);
            dt.AcceptChanges();
            da.Dispose();

        }*/

        public void AddPlayerItem(Character player, Item item)
        {

            //Create new sql connection calling stored proc to update data
            using var Cmd = new MySqlCommand("AddPlayerItem", con);
            Cmd.CommandType = CommandType.StoredProcedure;

            Cmd.Parameters.AddWithValue("playerID", player.ServerID);
            Cmd.Parameters.AddWithValue("ID", item.ID);
            Cmd.Parameters.AddWithValue("stack", item.StackLeft);
            Cmd.Parameters.AddWithValue("remHP", item.RemainingHP);
            Cmd.Parameters.AddWithValue("remCharge", item.Charges);
            Cmd.Parameters.AddWithValue("pattern", item.Pattern.ItemID);
            Cmd.Parameters.AddWithValue("equip_location", (sbyte)item.EquipLocation);
            Cmd.Parameters.AddWithValue("loc", (sbyte)item.Location);
            Cmd.Parameters.AddWithValue("listnum", item.ClientIndex);

            Cmd.ExecuteNonQuery();
        }

        public void UpdatePlayerItem(Character player, Item item)
        {
            using var Cmd = new MySqlCommand("UpdatePlayerItem", con);
            Cmd.CommandType = CommandType.StoredProcedure;

            Cmd.Parameters.AddWithValue("playerID", player.ServerID);
            Cmd.Parameters.AddWithValue("stack", item.StackLeft);
            Cmd.Parameters.AddWithValue("remHP", item.RemainingHP);
            Cmd.Parameters.AddWithValue("remCharge", item.Charges);
            Cmd.Parameters.AddWithValue("equip_location", (sbyte)item.EquipLocation);
            Cmd.Parameters.AddWithValue("ID", item.ID);
            Cmd.Parameters.AddWithValue("loc", (sbyte)item.Location);


            Cmd.ExecuteNonQuery();

        }

        public void DeletePlayerItem(int itemID)
        {
            //Create new sql connection calling stored proc to update data
            using var Cmd = new MySqlCommand("DeletePlayerItem", con);
            Cmd.CommandType = CommandType.StoredProcedure;

            Cmd.Parameters.AddWithValue("ID", itemID);
            Cmd.ExecuteNonQuery();

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
                    (EntityType)rdr.GetUInt32(22),
                    //NPC ID
                    rdr.GetInt32(23));
                //add the created actor to the npcData list
                npcData.Add(newActor);

            }
            rdr.Close();

            //Second SQL command and reader
            using var SecondCmd = new MySqlCommand("GetActorGear", con);
            SecondCmd.CommandType = CommandType.StoredProcedure;
            using MySqlDataReader SecondRdr = SecondCmd.ExecuteReader();

            int actorID;

            //Use second reader to iterate through character gear and assign to character attributes
            while (SecondRdr.Read())
            {
                //Hold charactervalue so we have names to compare against 
                actorID = SecondRdr.GetInt32(0);

                //Iterate through characterData list finding charnames that exist
                Actor thisActor = npcData.Find(i => Equals(i.ServerID, actorID));
                if (thisActor != null)
                {
                    if (thisActor.Inventory == null)
                        thisActor.Inventory = new(0, thisActor);

                    Item ThisItem = ItemManager.CreateItem(SecondRdr.GetInt32(7), SecondRdr.GetInt32(1), thisActor);

                    //If this is 1, it needs to go to inventory
                    if (ThisItem.Location == ItemLocation.Inventory)
                        thisActor.Inventory.AddItem(ThisItem);
                }
            }

            SecondRdr.Close();
            //return the list of actors from DB
            return npcData;
        }

        public List<ItemPattern> ItemPatterns()
        {
            List<ItemPattern> itemPatterns = new();

            using var cmd = new MySqlCommand("GetItemPatterns", con);
            cmd.CommandType = CommandType.StoredProcedure;
            using MySqlDataReader rdr = cmd.ExecuteReader();

            while (rdr.Read())
            {
                ItemPattern item = new(
                      //ItemID
                      rdr.GetInt32(0),
                      //Item cost 
                      rdr.GetUInt32(1),
                      //ItemIcon
                      rdr.GetInt32(2),
                      //Itempattern equipslot
                      rdr.GetInt32(3),
                      //Attack Type 
                      rdr.GetInt32(4),
                      //WeaponDamage
                      rdr.GetInt32(5),
                      //MaxHP of item 
                      rdr.GetInt32(6),
                      //Level requirement 
                      rdr.GetInt32(11),
                      //Max stack of item 
                      rdr.GetInt32(12),
                      //ItemName
                      rdr.GetString(13),
                      //Item Description
                      rdr.GetString(14),
                      //Duration
                      rdr.GetInt32(15),
                      //useable classes
                      rdr.GetInt32(16),
                      //useable races
                      rdr.GetInt32(17),
                      //Proc Animation
                      rdr.GetInt32(18),
                      new int[28] { rdr.GetInt32(19), rdr.GetInt32(20), rdr.GetInt32(21), rdr.GetInt32(22), rdr.GetInt32(23), rdr.GetInt32(24), rdr.GetInt32(25),
                                    0, rdr.GetInt32(26), 0, rdr.GetInt32(27), 0, rdr.GetInt32(28),  rdr.GetInt32(29), rdr.GetInt32(30), 0, 0, 0, 0, 0, 0, 0,
                                    rdr.GetInt32(31),  rdr.GetInt32(32), rdr.GetInt32(33), rdr.GetInt32(34), rdr.GetInt32(35), rdr.GetInt32(36) },
                      //Model
                      rdr.GetInt32(37),
                      //Color
                      rdr.GetUInt32(38),
                      (rdr.GetInt32(7) == 1 ? ItemFlags.NoTrade : 0) | (rdr.GetInt32(8) == 0 ? ItemFlags.NoRent : 0) | (rdr.GetInt32(9) == 1 ? ItemFlags.Craft : 0) | (rdr.GetInt32(10) == 1 ? ItemFlags.Lore : 0));

                itemPatterns.Add(item);
            }

            rdr.Close();
            return itemPatterns;
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
                    //serverid 1
                    rdr.GetString(1),
                    //charName 2
                    rdr.GetInt32(0),
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
                    //activeQuests
                    rdr.GetString(44),
                    //completedQuests
                    rdr.GetString(43),
                    //58
                    session);

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
                  (ItemLocation)SecondRdr.GetSByte(5),
                  //Location in inventory
                  SecondRdr.GetByte(6),
                  //ItemID
                  ItemManager.GetItemPattern(SecondRdr.GetInt32(7)),
                  SecondRdr.GetInt32(46));

                //If this is 1, it needs to go to inventory
                //Only this one one is needed for character select data
                //TODO:Add a enum? for Item Location to represent inventory, bank, auction
                if (ThisItem.Location == ItemLocation.Inventory)
                    thisChar.Inventory.AddItem(ThisItem);


                //If this is 2, it needs to go to the Bank
                else if (ThisItem.Location == ItemLocation.Bank)
                    thisChar.Bank.AddItem(ThisItem);

                //If this is 4, it needs to go to "Auction items". This should be items you are selling and still technically in your possession
                else if (ThisItem.Location == ItemLocation.Auction)
                    thisChar.AuctionItems.Add(ThisItem);

            }

            SecondRdr.Close();



            //return Character Data with characters and gear.
            return characterData;
        }

        public Character AcquireCharacter(Session session, int serverID)
        {
            //Holds list of characters for whole class
            Character selectedCharacter = null;

            //Queries DB for all characters and their necessary attributes  to generate character select
            using var cmd = new MySqlCommand("GetAccountCharacters", con);
            cmd.CommandType = CommandType.StoredProcedure;
            cmd.Parameters.AddWithValue("pAccountID", session.AccountID);
            using MySqlDataReader rdr = cmd.ExecuteReader();


            //Read through results from query populating character data needed for character select
            while (rdr.Read())
            {
                if (rdr.GetInt32(0) == serverID)
                {
                    //Instantiate character object
                    selectedCharacter = new Character
                    (
                        //charName 1
                        rdr.GetString(1),
                        //serverid 2
                        rdr.GetInt32(0),
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
                        //activeQuests
                        rdr.GetString(44),
                        //completedQuests
                        rdr.GetString(43),
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
                      (ItemLocation)SecondRdr.GetSByte(5),
                      //Location in inventory
                      SecondRdr.GetByte(6),
                      //ItemID
                      ItemManager.GetItemPattern(SecondRdr.GetInt32(7)),
                      SecondRdr.GetInt32(46));

                    //If this is 1, it needs to go to inventory
                    //TODO:Add a enum? for Item Location to represent inventory, bank, auction
                    if (ThisItem.Location == ItemLocation.Inventory)
                        selectedCharacter.Inventory.AddItem(ThisItem);

                    //If this is 2, it needs to go to the Bank
                    else if (ThisItem.Location == ItemLocation.Bank)
                        selectedCharacter.Bank.AddItem(ThisItem);

                    //If this is 4, it needs to go to "Auction items". This should be items you are selling and still technically in your possession
                    else if (ThisItem.Location == ItemLocation.Auction)
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
            cmd.Parameters.AddWithValue("charID", session.MyCharacter.ServerID);
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
            charCreation.playerFlags = new Dictionary<string, string>()
{
{ "NewPlayerIntro", "0" },
                {"admin", "true" }
};
            string serializedPlayerFlags = (string)JsonSerializer.Serialize(charCreation.playerFlags);

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
            SecondCmd.Parameters.AddWithValue("completedQuests", "[]");
            SecondCmd.Parameters.AddWithValue("activeQuests", "[]");
            //Execute parameterized statement entering it into the DB
            SecondCmd.ExecuteNonQuery();

            using var cmd = new MySqlCommand("GetAccountCharacters", con);
            cmd.CommandType = CommandType.StoredProcedure;
            cmd.Parameters.AddWithValue("pAccountID", session.AccountID);
            using MySqlDataReader rdr = cmd.ExecuteReader();

            //string to hold local charcter Name
            int serverID = 0;

            //Read through results from query populating character data needed for character select
            while (rdr.Read())
            {
                if(rdr.GetString(1) == charCreation.CharName)
                    serverID = rdr.GetInt32(0);
            }
            rdr.Close();

            if (serverID == 0)
            {
                Console.WriteLine("Character Creation Failed");
                return;
            }

            using var ThirdCmd = new MySqlCommand("CreateHotkeys", con);
            ThirdCmd.CommandType = CommandType.StoredProcedure;
            for(int i = 0; i < charCreation.MyHotkeys.Count; i++)
            {
                ThirdCmd.Parameters.Clear();
                ThirdCmd.Parameters.AddWithValue("Serverid", serverID);
                ThirdCmd.Parameters.AddWithValue("Direction", charCreation.MyHotkeys[i].Direction);
                ThirdCmd.Parameters.AddWithValue("nlabel", charCreation.MyHotkeys[i].NLabel);
                ThirdCmd.Parameters.AddWithValue("nmessage", charCreation.MyHotkeys[i].NMessage);
                ThirdCmd.Parameters.AddWithValue("wlabel", charCreation.MyHotkeys[i].WLabel);
                ThirdCmd.Parameters.AddWithValue("wmessage", charCreation.MyHotkeys[i].WMessage);
                ThirdCmd.Parameters.AddWithValue("elabel", charCreation.MyHotkeys[i].ELabel);
                ThirdCmd.Parameters.AddWithValue("emessage", charCreation.MyHotkeys[i].EMessage);
                ThirdCmd.Parameters.AddWithValue("slabel", charCreation.MyHotkeys[i].SLabel);
                ThirdCmd.Parameters.AddWithValue("smessage", charCreation.MyHotkeys[i].SMessage);
                ThirdCmd.ExecuteNonQuery();
            }

            //Don't close connection because we have character list generated next
            List<Character> MyCharacterList = AccountCharacters(session);

            //Send Fresh Character Listing
            ServerCreateCharacterList.CreateCharacterList(MyCharacterList, session);
        }

        //Probably no longer need this since we're doing all DB saves immediately
        /*public static void printDataTable(DataTable tbl)
        {
            string line = "";
            foreach (DataColumn item in tbl.Columns)
            {
                line += item.ColumnName + "   ";
            }
            line += "\n";
            foreach (DataRow row in tbl.Rows)
            {
                for (int i = 0; i < tbl.Columns.Count; i++)
                {
                    line += row[i].ToString() + "   ";
                }
                line += "\n";
            }
            Console.WriteLine(line);
        }*/

        public void GetMaxItemID()
        {
            using var CheckItemRows = new MySqlCommand("Select COUNT(*) from charInventory;", con);
            int result = int.Parse(CheckItemRows.ExecuteScalar().ToString());

            if (result == 0)
            {
                Console.WriteLine("No items exist for any characters. Starting at 1.");
                ItemManager.nextItemID = 1;
            }
            else
            {

                //TODO: Need to figure out a way to track this while still working if there are no existing entries to return max ID of.
                //Set and open SQL con
                //SQL query to check if a name exists in the DB or not
                using var GetItemIDCmd = new MySqlCommand("select MAX(itemID) from charInventory;", con);
                //Executes the SQL reader
                ItemManager.nextItemID = (int)GetItemIDCmd.ExecuteScalar() + 1;
            }
            Console.WriteLine($"Next Item ID available: {ItemManager.nextItemID}");
        }
    }
}
