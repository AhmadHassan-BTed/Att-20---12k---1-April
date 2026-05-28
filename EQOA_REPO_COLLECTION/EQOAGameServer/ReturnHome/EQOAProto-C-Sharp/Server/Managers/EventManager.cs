using System.IO;
using ReturnHome.Server.Opcodes;
using ReturnHome.Server.EntityObject.Player;
using NLua;
using ReturnHome.Server.Network;
using ReturnHome.Server.Opcodes.Messages.Server;
using ReturnHome.Server.EntityObject;
using ReturnHome.Utilities;
using System;
using System.Text.Json;
using System.Collections;
using System.Collections.Generic;
using ReturnHome.Database.SQL;
using System.Data;
using ReturnHome.Server.EntityObject.Items;

namespace ReturnHome.Server.Managers
{
    public static class EventManager
    {
        public static void GetNPCDialogue(GameOpcode opcode, Session mySession)
        {

            //CharacterSQL sql = new CharacterSQL();
            //sql.SavePlayerItems(mySession.MyCharacter);

            //ItemManager.GrantItem(mySession, 31010, 1);



            string[] file;
            EntityManager.QueryForEntity(mySession.MyCharacter.Target, out Entity targetNPC);

            if (mySession.MyCharacter.MyDialogue.npcName == null)
                return;
            //Strip white spaces from NPC name and replace with Underscores
            mySession.MyCharacter.MyDialogue.npcName = mySession.MyCharacter.MyDialogue.npcName.Replace(" ", "_");
            //Find Lua script recursively through scripts directory by zone
            //May rewrite later if this proves slow. Probably needs exception catching in case it doesn't find it
            if (targetNPC != null && FileExistsRecursive("../../../Scripts", mySession.MyCharacter.MyDialogue.npcName + "_" + targetNPC.ServerID + ".lua"))
            {
                file = Directory.GetFiles("../../../Scripts", mySession.MyCharacter.MyDialogue.npcName + "_" + targetNPC.ServerID + ".lua", SearchOption.AllDirectories);

            }
            else
            {
                file = Directory.GetFiles("../../../Scripts", mySession.MyCharacter.MyDialogue.npcName + ".lua", SearchOption.AllDirectories);
                if (file.Length < 1 || file == null)
                    return;
            }
            //TODO: work around for a npc with no scripts etc? Investigate more eventually


            //Create new lua object
            Lua lua = new Lua();
            //load lua CLR library 
            lua.LoadCLRPackage();
            //If the op code to be sent is a dialogue box make sure we capture dialogue choices 
            if (opcode == GameOpcode.DialogueBoxOption)
            {
                //send the dialogue choice to the lua script based on the chosen option index
                string choiceOption = mySession.MyCharacter.MyDialogue.diagOptions[(int)mySession.MyCharacter.MyDialogue.choice];

                //pass the string choice to the Lua as choice
                lua["choice"] = choiceOption;
            }
            //Create handles for the lua script to access some c# variables and methods
            lua["GetPlayerFlags"] = mySession.MyCharacter.GetPlayerFlags;
            lua["SetPlayerFlags"] = mySession.MyCharacter.SetPlayerFlag;
            lua["AddQuest"] = Quest.AddQuest;
            lua["DeleteQuest"] = Quest.DeleteQuest;
            lua["SendDialogue"] = mySession.MyCharacter.SendDialogue;
            lua["SendMultiDialogue"] = mySession.MyCharacter.SendMultiDialogue;
            lua["TeleportPlayer"] = ServerTeleportPlayer.TeleportPlayer;
            lua["StartQuest"] = Quest.StartQuest;
            lua["ContinueQuest"] = Quest.ContinueQuest;
            lua["CompleteQuest"] = Quest.CompleteQuest;
            lua["CheckQuestItem"] = Character.CheckIfQuestItemInInventory;
            lua["mySession"] = mySession;
            lua["GenerateChatMessage"] = Opcodes.Chat.ChatMessage.GenerateClientSpecificChat;
            lua["GrantXP"] = Character.GrantXP;
            lua["GetWorld"] = Utility_Funcs.GetEnumObjectByValue<World>;
            lua["GetClass"] = Utility_Funcs.GetEnumObjectByValue<Class>;
            lua["GetRace"] = Utility_Funcs.GetEnumObjectByValue<Race>;
            lua["GetHumanType"] = Utility_Funcs.GetEnumObjectByValue<HumanType>;
            lua["UpdateAnim"] = Entity.UpdateAnim;
            //wont actually return npc object to update, only npcid
            lua["thisNPC"] = mySession.MyCharacter.Target;
            lua["GrantItem"] = ItemManager.GrantItem;
            lua["TurnInItem"] = ItemManager.UpdateQuantity;
            lua["RemoveTunar"] = Entity.RemoveTunar;
            lua["AddTunar"] = Entity.AddTunar;


            lua["race"] = Entity.GetRace(mySession.MyCharacter.EntityRace);
            lua["class"] = Entity.GetClass(mySession.MyCharacter.EntityClass);
            lua["humanType"] = Entity.GetHumanType(mySession.MyCharacter.EntityHumanType);
            lua["level"] = mySession.MyCharacter.Level;

            //Call the Lua script found by the Dictionary Find above
            lua.DoFile(file[0]);

            //switch to find correct lua function based on op code from NPC Interact 0x04
            if (opcode == GameOpcode.DialogueBox || opcode == GameOpcode.DialogueBoxOption)
            {
                //Call Lua function for initial interaction
                LuaFunction callFunction = lua.GetFunction("event_say");
                mySession.MyCharacter.TurnToPlayer((int)mySession.MyCharacter.Target);
                callFunction.Call();
            }
        }

        public static string GetMerchantDialogue(Session session, Entity npc)
        {
            string myString = " ";
            //Strip white spaces from NPC name and replace with Underscores
            string npcNameSearch = npc.CharName.Replace(" ", "_");
            //Find Lua script recursively through scripts directory by zone
            //May rewrite later if this proves slow. Probably needs exception catching in case it doesn't find it
            string[] file = Directory.GetFiles("../../../Scripts", npcNameSearch + ".lua", SearchOption.AllDirectories);
            //TODO: work around for a npc with no scripts etc? Investigate more eventually
            if (file.Length < 1 || file == null)
            {
                return myString;
            }

            //Create new lua object
            Lua lua = new Lua();
            //load lua CLR library 
            lua.LoadCLRPackage();
            lua.DoFile(file[0]);

            myString = (string)lua["merchantDialogue"];

            if (string.IsNullOrEmpty(myString))
            {
                myString = " ";
            }

            if (myString != null)
            {
                myString = myString.Replace("playerName", session.MyCharacter.CharName);
            }

            return myString;
        }

        public static bool FileExistsRecursive(string rootPath, string filename)
        {
            if (File.Exists(Path.Combine(rootPath, filename)))
                return true;

            foreach (string subDir in Directory.GetDirectories(rootPath))
            {
                if (FileExistsRecursive(subDir, filename))
                    return true;
            }

            return false;
        }

        public static void printDataTable(DataTable tbl)
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
        }
    }
}
