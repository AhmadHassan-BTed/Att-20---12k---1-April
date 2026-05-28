using System;
using System.Collections.Generic;
using System.Numerics;
using System.Text;
using System.Linq;

using ReturnHome.Server.Opcodes;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.Managers;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;
using ReturnHome.Server.Opcodes.Messages.Server;
using ReturnHome.Server.Opcodes.Chat;

using NLua;
using ReturnHome.Server.EntityObject.Items;
using System.Collections.Concurrent;
using ReturnHome.Server.EntityObject.Actors;
using System.Text.RegularExpressions;
using ReturnHome.Database.SQL;

namespace ReturnHome.Server.EntityObject
{
    public partial class Entity
    {
        public ItemContainer Inventory;
        private uint _target;
        private uint _targetCounter = 1;
        private Entity _ourTarget;

        public uint Target
        {
            get { return _target; }
            set
            {
                if (true)
                {
                    _target = value;
                    ObjectUpdateTarget();

                    //Keep a reference to our current target on hand
                    EntityManager.QueryForEntity(_target, out _ourTarget);
                    if (_ourTarget != null)
                        //Console.WriteLine($"{CharName} targeting {_ourTarget.CharName} at X: {_ourTarget.x} Y: {_ourTarget.y} Z: {_ourTarget.z} XVel:{_ourTarget.VelocityX} VelY: {_ourTarget.VelocityY} VelZ: {_ourTarget.VelocityZ}");

                        if (isPlayer && ObjectID != 0)
                        {
                            //Get target information about the object
                            TargetInformation(_target);
                        }
                }
            }
        }

        public void TargetInformation(uint Target)
        {
            //If false, ignore? Might need some kind of escalation, why would we target something not known about?
            //If true, prep message
            if (EntityManager.QueryForEntity(_target, out Entity ent))
            {
                //This shouldn't happen, but to be safe? Eventually could be an expired object that was originally target?
                if (ent == null)
                    return;

                ServerPlayerTarget.PlayerTarget(((Character)this).characterSession, 3, GenerateConColor(), _target, _targetCounter++);
            }
        }

        public bool IsWithinRange()
        {
            //What should be the distance check against interacting with NPC's?
            //Should this distance check be farther or shorter for attacking in combat, too? Such as auto attack
            if (Vector3.Distance(Position, _ourTarget.Position) <= 10.0f)
                return true;
            return false;
        }

        //Method use to determine Entities target con color. Specifically for Characters
        //but can be altered some to introduce if an Actor should attack a player based on level
        private byte GenerateConColor()
        {

            sbyte difference = (sbyte)(Level - _ourTarget.Level);

            switch (difference)
            {
                //Red con target
                case <= -3:
                    return 0;

                //Yellow con'ing target
                case -2:
                case -1:
                    return 1;

                //White con target
                case 0:
                    return 2;

                //Dark Blue con target
                case 1:
                case 2:
                    return 3;

                //Decide Light blue and green con targets here
                default:

                    //return (byte)(Level <= 15 ? (Level - _ourTarget.Level) > 3 ? 5 : 4 : (Level - _ourTarget.Level) > (Level / 4) ? 5 : 4);
                    if (Level <= 15)
                    {
                        //Green con target
                        if ((Level - _ourTarget.Level) > 3)
                            return 5;

                        //Light blue con target
                        else
                            return 4;
                    }

                    else
                    {
                        //Green con target
                        if ((Level - _ourTarget.Level) > (Level / 4))
                            return 5;

                        //Light blue con target
                        else
                            return 4;
                    }
            }
        }

        //This is wrong... We are just referencing the npc item's state to a character here where the state will be shared to all users
        //Need to some how make a new copy of it, not sure the easiest way to do that
        public void MerchantBuy(byte itemSlot, int itemQty, uint targetNPC)
        {
            if (EntityManager.QueryForEntity(targetNPC, out Entity npc))
            {
                if (npc.Inventory.TryRetrieveItem(itemSlot, out Item item, out byte index))
                {
                    if (Inventory.Tunar < (item.Pattern.ItemCost * itemQty))
                    {
                        ChatMessage.ClientErrorMessage(((Character)this).characterSession, $"You can't afford that.");
                        ServerInventoryFull.InventoryFull(((Character)this).characterSession);
                    }

                    else
                    {
                        //TODO: Is this logic better? Verified we have the tunar. Try to add it to our inventory, if rejected for any reason, we don't pull the tunar from character
                        Item newItem = item.AcquireItem(itemQty);
                        if (Inventory.AddItem(newItem))
                            Inventory.RemoveTunar((int)(item.Pattern.ItemCost * itemQty));
                    }
                }
            }
        }

        //Method used to send any in game dialogue to player. Works for option box or regular dialogue box
        public void SendDialogue(Session session, string dialogue, LuaTable diagOptions)
        {
            if (dialogue != null)
            {
                dialogue = dialogue.Replace("playerName", session.MyCharacter.CharName);
            }

            if (dialogue == "")
            {
                return;
            }
            else if (dialogue.Length > 217)
            {
                string splitRegex = @"(?<=[.?!]\s)";
                string[] newDialogue = Regex.Split(dialogue, splitRegex);
                foreach (string s in newDialogue)
                {
                    SendDialogue(session, s, diagOptions);
                }
                return;

            }
            //Clear player's previous dialogue options before adding new ones.
            session.MyCharacter.MyDialogue.diagOptions.Clear();


            //loop over luatable, assigning every value to an element of the players diagOptions list
            //lua table always returns a dict type object of <object,object>
            if (diagOptions != null)
            {
                foreach (KeyValuePair<object, object> k in diagOptions)
                    session.MyCharacter.MyDialogue.diagOptions.Add(k.Value.ToString());
                //If it's not a yes/no choice then sort alphabetically.
                //This forces it to return choices the same every time.
                if (!session.MyCharacter.MyDialogue.diagOptions.Contains("Yes"))
                    session.MyCharacter.MyDialogue.diagOptions.Sort();
            }


            //Length of choices
            uint choicesLength = 0;
            //set choice counter for player
            uint choiceCounter = session.MyCharacter.MyDialogue.counter;
            byte textOptions = 0;
            GameOpcode dialogueType = GameOpcode.DialogueBoxOption;
            //if dialogue options exist set choice counter update
            if (session.MyCharacter.MyDialogue.diagOptions != null)
            {
                choiceCounter = (uint)session.MyCharacter.MyDialogue.diagOptions.Count;
                //Length of choices
                foreach (string choice in session.MyCharacter.MyDialogue.diagOptions)
                {
                    choicesLength += (uint)choice.Length;
                }

                //count the number of textOptions
                textOptions = (byte)session.MyCharacter.MyDialogue.diagOptions.Count;
                //set dialogue type to option
                dialogueType = GameOpcode.OptionBox;
            }

            //if there are no diagOptions send onlly diag box.
            if (session.MyCharacter.MyDialogue.diagOptions.Count <= 0)
                dialogueType = GameOpcode.DialogueBox;

            //set player dialogue to the incoming dialogue
            session.MyCharacter.MyDialogue.dialogue = dialogue;

            //create variable memory span for sending out dialogue
            Message message = Message.Create(MessageType.ReliableMessage, dialogueType);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write(choiceCounter);
            writer.WriteString(Encoding.Unicode, dialogue);
            //if it's an option box iterate through options and write options to message
            if (dialogueType == GameOpcode.OptionBox)
            {
                writer.Write(textOptions);

                if (session.MyCharacter.MyDialogue.diagOptions != null)
                {
                    for (int i = 0; i < session.MyCharacter.MyDialogue.diagOptions.Count; i++)
                        writer.WriteString(Encoding.Unicode, session.MyCharacter.MyDialogue.diagOptions[i]);
                }
            }

            message.Size = writer.Position;
            //Send Message
            session.sessionQueue.Add(message);
            session.MyCharacter.MyDialogue.choice = 1000;
        }

        //Method for only sending multiple strings of dialogue.
        public void SendMultiDialogue(Session session, LuaTable dialogue)
        {
            int choiceCounter = 0;
            List<string> multiDialogue = new List<string>();
            //Converts luatable 
            if (dialogue != null)
            {
                foreach (KeyValuePair<object, object> k in dialogue)
                {
                    string repString = k.Value.ToString();
                    repString = repString.Replace("playerName", session.MyCharacter.CharName);
                    multiDialogue.Add(repString);

                }
            }



            foreach (string d in multiDialogue)
            {
                //create variable memory span for sending out dialogue
                Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.DialogueBox);
                BufferWriter writer = new BufferWriter(message.Span);

                writer.Write(message.Opcode);
                writer.Write(choiceCounter);
                writer.WriteString(Encoding.Unicode, d);
                //if it's an option box iterate through options and write options to message

                message.Size = writer.Position;
                //Send Message
                session.sessionQueue.Add(message);
            }
        }


        public void ProcessDialogue(Session session, BufferReader reader, Message ClientPacket, uint interactTarget)
        {
            //if option choice incoming
            if (ClientPacket.Opcode == GameOpcode.DialogueBoxOption)
            {
                //try to pull the option counter and players choice out of message
                try
                {
                    session.MyCharacter.MyDialogue.choice = reader.Read<byte>();
                    //if diag message is 255(exit dialogue in client) return immediately without new message
                    //and set choice to incredibly high number to make sure it doesn't retrigger any specific dialogue.
                    if (session.MyCharacter.MyDialogue.choice == 255)
                    {
                        session.MyCharacter.MyDialogue.choice = 1000;
                        return;
                    }
                }
                catch (Exception)
                {
                    throw;
                }
            }
            //Create new instance of the event manager
            //EventManager eManager = new EventManager();
            //Entity npcEntity = new Entity(false);
            Dialogue dialogue = session.MyCharacter.MyDialogue;
            GameOpcode dialogueType = GameOpcode.DialogueBoxOption;
            //if a diag option choice incoming set outgoing to diag box option
            if (ClientPacket.Opcode == GameOpcode.DialogueBoxOption)
                dialogueType = GameOpcode.DialogueBoxOption;

            //else this is just a regular interaction with only dialogue
            else if (ClientPacket.Opcode == GameOpcode.Interact)
                dialogueType = GameOpcode.DialogueBox;

            //Gets NPC name from ObjectID
            if (ClientPacket.Opcode == GameOpcode.Interact)
            {
                if (EntityManager.QueryForEntity(interactTarget, out Entity npc))
                    dialogue.npcName = npc.CharName;
            }
            //call the event manager to process dialogue from here. Passes to Lua scripts.
            EventManager.GetNPCDialogue(dialogueType, session);
        }

        private int _debt = 0;
        private int _totXP = 0;
        private int _paidDebt = 0;
        private int _cm = 0;
        private int _cmXP = 0;
        private int _newTrainingPoints = 0;

        public static void GrantXP(Session session, int xpAmount)
        {
            session.MyCharacter._debt = 0;
            session.MyCharacter._totXP = xpAmount;
            session.MyCharacter._paidDebt = 0;
            session.MyCharacter._cm = 0;
            session.MyCharacter._cmXP = 0;

            if (session.MyCharacter.totalDebt > 0)
            {
                session.MyCharacter._debt = session.MyCharacter.totalDebt;

                //If true, will erase remaining debt
                if ((xpAmount >> 1) >= session.MyCharacter.totalDebt)
                {
                    session.MyCharacter._paidDebt = session.MyCharacter.totalDebt;
                    xpAmount -= session.MyCharacter.totalDebt;
                    session.MyCharacter.totalDebt = 0;
                }

                //Otherwise, chop xp amount in half to satisify some debt
                else
                {
                    session.MyCharacter._paidDebt = xpAmount >> 1;
                    session.MyCharacter.totalDebt -= xpAmount >> 1;
                    xpAmount -= xpAmount >> 1;
                }
            }

            //check to see if player is having a percentage of XP going to CMs
            if (session.MyCharacter._cm > 0)
            {
                session.MyCharacter._cmXP = (int)(session.MyCharacter._cm / 100f) * xpAmount;
                xpAmount -= session.MyCharacter._cmXP;

                //Only XP based towards cm's should remove from our total earned xp
                session.MyCharacter._totXP -= session.MyCharacter._cmXP;
            }

            //Inform client of the gainz
            if (xpAmount > 0)
            {
                if (session.MyCharacter._debt > 0)
                    ChatMessage.GenerateClientSpecificChat(session, $"You received {session.MyCharacter._totXP} XP. {session.MyCharacter._paidDebt} was paid towards your debt of {session.MyCharacter._debt}.");

                else
                    ChatMessage.GenerateClientSpecificChat(session, $"You received {xpAmount} XP.");
            }

            //Need to send earned CM opcode with this if any
            if (session.MyCharacter._cmXP > 0)
                ChatMessage.GenerateClientSpecificChat(session, $"You received {session.MyCharacter._cmXP} Mastery XP.");

            session.MyCharacter.XPEarnedInThisLevel += xpAmount;
            session.MyCharacter.TotalXP += xpAmount;

            if (session.MyCharacter.Level < 60)
            {
                while (session.MyCharacter.XPEarnedInThisLevel > CharacterUtilities.CharXPDict[session.MyCharacter.Level])
                {
                    //Some logic to take away prior level's experience and place player's xp into new bracket
                    session.MyCharacter.XPEarnedInThisLevel -= CharacterUtilities.CharXPDict[session.MyCharacter.Level];

                    //Increase player level
                    session.MyCharacter.Level++;
                    //When we level up, recalculate HP and Power
                    session.MyCharacter.CalculateHP();
                    session.MyCharacter.CalculatePower();

                    ChatMessage.GenerateClientSpecificChat(session, $"You have reached level {session.MyCharacter.Level}");
                    if (session.MyCharacter.Level >= 60)
                        break;
                }

                //check for Training points to be added
                while (session.MyCharacter.TotalXP >= TrainingPoints.TrainingPointDict[session.MyCharacter.PlayerTrainingPoints.TotalTrainingPoints + 1 + session.MyCharacter._newTrainingPoints])
                {
                    session.MyCharacter._newTrainingPoints += 1;
                    if (session.MyCharacter.PlayerTrainingPoints.TotalTrainingPoints + 1 + session.MyCharacter._newTrainingPoints > 482)
                        break;
                }

                if (session.MyCharacter._newTrainingPoints > 0)
                {
                    session.MyCharacter.PlayerTrainingPoints.EarnTrainingPoints(session.MyCharacter._newTrainingPoints);
                    session.MyCharacter.SendDialogue(session, "You have received more training points!", null);

                    //Since we are adding training points, we have to indicate a negative value to the client so it will add it to the total. it's weird...
                    ServerAdjustTrainingPoints.AdjustTrainingPoints(session, session.MyCharacter._newTrainingPoints * -1);
                    session.MyCharacter._newTrainingPoints = 0;
                }
            }

            //TODO: Need to findout Level 60 top xp range to halt xp gain on this opcode at some point, we could also force 100% CM's at 60 too?
            //Something similar as above for Training points
            ServerUpdatePlayerXPandLevel.UpdatePlayerXPandLevel(session, session.MyCharacter.Level, session.MyCharacter.XPEarnedInThisLevel);

        }

        //Not the most efficent, but works..
        public static bool CheckIfQuestItemInInventory(Session session, int itemID, int itemQty)
        {
            for (int i = 0; i < session.MyCharacter.Inventory.Count; ++i)
                if (session.MyCharacter.Inventory.itemContainer[i].item.Pattern.ItemID == itemID)
                    if (session.MyCharacter.Inventory.itemContainer[i].item.StackLeft >= itemQty)
                        return true;
            return false;
        }

        public static bool CheckIfItemInInventory(Session session, int itemID, out byte key, out Item item)
        {
            item = default;
            key = 0;

            for (int i = 0; i < session.MyCharacter.Inventory.Count; ++i)
                if (session.MyCharacter.Inventory.itemContainer[i].item.Pattern.ItemID == itemID)
                {
                    item = session.MyCharacter.Inventory.itemContainer[i].item;
                    key = session.MyCharacter.Inventory.itemContainer[i].key;
                    return true;
                }

            return false;
        }

        //Not the most efficent, but works..
        public static bool RemoveQuestItemFromPlayerInventory(Session session, int itemID, int itemQty)
        {
            for (byte i = 0; i < session.MyCharacter.Inventory.Count; i++)
                if (session.MyCharacter.Inventory.itemContainer[i].item.Pattern.ItemID == itemID && session.MyCharacter.Inventory.itemContainer[i].item.StackLeft >= itemQty)
                {
                    session.MyCharacter.Inventory.UpdateQuantity(session.MyCharacter.Inventory.itemContainer[i].key, itemQty);
                    return true;
                }

            return false;
        }

        public static bool AddItemToPlayerInventory(Session session)
        {
            //Do some magic to create an item here?!
            //Maybe quest NPC's store quest rewards and we figure out how to give accordingly?
            throw new NotImplementedException("AddItemToPlayerInventory not fully implemented, don't use this");
            session.MyCharacter.Inventory.AddItem(default);
        }

        public void TakeDamage(uint playerID, int dmg)
        {

            if (CurrentHP > 0)
            {
                if (dmg > this.CurrentHP)
                {
                    this.CurrentHP = 0;
                }
                else
                {
                    this.CurrentHP -= dmg;
                }
            }
            //TODO: We have Character entities here causing casting exceptions
            ((Actor)this).EvaluateAggro(dmg, playerID);
        }

        public static void UpdateAnim(uint ServerID, AnimationState animation)
        {
            if (EntityManager.QueryForEntityByServerID(ServerID, out Entity entity))
            {
                entity.Animation = (byte)animation;
            }
        }

        public static void UpdateAnimByte(uint ServerID, byte animation)
        {
            if (EntityManager.QueryForEntityByServerID(ServerID, out Entity entity))
            {
                entity.Animation = animation;
            }
        }

        public static void AddTunar(Session session, int tunar) => session.MyCharacter.Inventory.AddTunar(tunar);

        public static void RemoveTunar(Session session, int tunar) => session.MyCharacter.Inventory.RemoveTunar(tunar);

        //Handles turning NPC to player when they interact.
        public void TurnToPlayer(int serverID)
        {
            if (EntityManager.QueryForEntityByServerID(Target, out Entity e))
            {
                byte newFacing = (byte)Lerp(e.Facing, Facing - (255 / 2), 1f);


                //Probably need to use quaternions but lerping for now just to make them turn correctly.
                /*Quaternion newPlayerRotation = new Quaternion(x, y, z, Facing);
                Quaternion newNPCRotation = new Quaternion(e.x, e.y, e.z, MathF.Round(e.FacingF) * MathF.PI / 128.0f);
                Quaternion qSlerp = Quaternion.Slerp(newPlayerRotation, newNPCRotation, 1);
                e.Facing = (byte)MathF.Round(qSlerp.W * 128.0f / MathF.PI);*/
                e.Facing = newFacing;


                //disabling until FSM is built and/or can make them stop "shuffling"
                /*if (newFacing > e.Facing)
                {
                    e.Animation = (byte)AnimationState.TurningInPlaceRight;
                }
                else
                {
                    e.Animation = (byte)AnimationState.TurningInPlaceLeft;
                }*/

                //basic lerp function
                float Lerp(float firstFloat, float secondFloat, float by)
                {
                    return firstFloat * (1 - by) + secondFloat * by;
                }

            }
        }
    }
}
