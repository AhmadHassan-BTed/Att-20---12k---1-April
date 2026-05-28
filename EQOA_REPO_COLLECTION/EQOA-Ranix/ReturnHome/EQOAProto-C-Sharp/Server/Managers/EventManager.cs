using System.IO;
using ReturnHome.Server.Opcodes;
using ReturnHome.Server.EntityObject.Player;
using NLua;
using ReturnHome.Server.Network;
using ReturnHome.Server.Opcodes.Messages.Server;
using ReturnHome.Server.EntityObject;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Managers
{
    public static class EventManager
    {
        public static void GetNPCDialogue(GameOpcode opcode, Session mySession)
        {
            //Strip white spaces from NPC name and replace with Underscores
            mySession.MyCharacter.MyDialogue.npcName = mySession.MyCharacter.MyDialogue.npcName.Replace(" ", "_");
            //Find Lua script recursively through scripts directory by zone
            //May rewrite later if this proves slow. Probably needs exception catching in case it doesn't find it
            string[] file = Directory.GetFiles("Scripts\\", mySession.MyCharacter.MyDialogue.npcName + ".lua", SearchOption.AllDirectories);
            //TODO: work around for a npc with no scripts etc? Investigate more eventually
            if (file.Length < 1 || file == null)
                return;

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
            lua["SendDialogue"] = mySession.MyCharacter.SendDialogue;
            lua["TeleportPlayer"] = ServerTeleportPlayer.TeleportPlayer;
            lua["AddQuestLog"] = Character.AddQuestLog;
            lua["DeleteQuestLog"] = Character.DeleteQuest;
            lua["CheckQuestItem"] = Character.CheckQuestItem;
            lua["mySession"] = mySession;
            lua["GenerateChatMessage"] = Opcodes.Chat.ChatMessage.GenerateClientSpecificChat;
            lua["GrantXP"] = Character.GrantXP;
            lua["GetWorld"] = Utility_Funcs.GetEnumObjectByValue<World>;
            lua["GetClass"] = Utility_Funcs.GetEnumObjectByValue<Class>;
            lua["GetRace"] = Utility_Funcs.GetEnumObjectByValue<Race>;
            lua["GetHumanType"] = Utility_Funcs.GetEnumObjectByValue<HumanType>;

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
                callFunction.Call();
            }
        }
    }
}
