using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Text.Json;
using NLua;
using ReturnHome.Server.Managers;
using ReturnHome.Server.Network;
using ReturnHome.Server.Opcodes.Messages.Server;
using ReturnHome.Utilities;

namespace ReturnHome.Server.EntityObject.Player
{
    public class Quest
    {
        public int questID { get; set; }
        public int questNPCID { get; set; }
        public int questIndex { get; set; }
        public string log { get; set; }
        public int questStep { get; set; }
        public int questXP { get; set; }

        public Quest()
        {

        }

        public Quest(int questID, int questIndex, int questStep, string questLogText)
        {
            this.questID = questID;
            this.log = questLogText;
            this.questIndex = questIndex;
        }

        public void DumpQuest(ref BufferWriter writer)
        {
            writer.WriteString(Encoding.Unicode, log);

        }

        //Potentially part of new Quest framework, still being worked on.
        public static void StartQuest(Session session, int questID, string questText)
        {
            Quest newQuest = new Quest();
            newQuest.questID = questID;
            newQuest.questStep = 1;
            newQuest.log = questText;

            if (session.MyCharacter.activeQuests.Count > 0)
                newQuest.questIndex = session.MyCharacter.activeQuests.Count;

            else
                newQuest.questIndex = 0;

            Logger.Info(($"Quest {questID}, step {newQuest.questStep} started by {session.MyCharacter.CharName}"));
            session.MyCharacter.activeQuests.Add(newQuest);

            session.MyCharacter.SetPlayerFlag(session, questID.ToString(), newQuest.questStep.ToString());
            ServerAddQuestLog.AddQuestLog(session, (uint)newQuest.questIndex, newQuest.log);
        }

        //Potentially part of new Quest framework, still being worked on.
        public static void ContinueQuest(Session session, int questID, string questText)
        {
            Quest quest = session.MyCharacter.activeQuests.Find(x => x.questID == questID);
            quest.questStep++;
            quest.log = questText;
            ServerDeleteQuest.DeleteQuest(session, quest.questIndex);

            if (session.MyCharacter.activeQuests.Count > 0)
                quest.questIndex = session.MyCharacter.activeQuests.Count + 1;

            else
                quest.questIndex = 0;

            //convert to logger
            Logger.Info($"Quest {questID}, step {quest.questStep} started by {session.MyCharacter.CharName}");
            session.MyCharacter.SetPlayerFlag(session, quest.questID.ToString(), quest.questStep.ToString());
            ServerAddQuestLog.AddQuestLog(session, (uint)quest.questIndex, quest.log);
        }

        //Only for when the player wants to delete the quest and stop doing it. Potentially part of new Quest framework, still being worked on.
        public static void DeleteQuest(Session session, int questIndex)
        {
            Quest thisQuest = session.MyCharacter.activeQuests[questIndex];

            if (thisQuest != null)
            {
                Logger.Info($"Quest with questIndex {thisQuest.questIndex} deleted for player {session.MyCharacter.CharName}.");
                if (session.MyCharacter.playerFlags.ContainsKey(thisQuest.questID.ToString()))
                {
                    session.MyCharacter.playerFlags[thisQuest.questID.ToString()] = "0";
                }
                session.MyCharacter.activeQuests.Remove(thisQuest);

            }

            ServerDeleteQuest.DeleteQuest(session, questIndex);
        }


        //Only for when the player actually completes the quest. Potentially part of new Quest framework, still being worked on.
        public static void CompleteQuest(Session session, int questID, int questXP)
        {

            Quest thisQuest = null;
            int questIndex = 0;

            for (int i = 0; i < session.MyCharacter.activeQuests.Count; i++)
                if (session.MyCharacter.activeQuests[i].questID == questID)
                {
                    thisQuest = session.MyCharacter.activeQuests[i];
                    questIndex = i;
                }

            thisQuest = session.MyCharacter.activeQuests.Find(i => Equals(i.questID, questID));

            if (thisQuest != null)
            {
                session.MyCharacter.completedQuests.Add(thisQuest);
                session.MyCharacter.activeQuests.Remove(thisQuest);
                session.MyCharacter.SetPlayerFlag(session, questID.ToString(), "99");
                session.MyCharacter.SetPlayerFlag(session, (++questID).ToString(), "0");
                if (questXP > 0)
                    Entity.GrantXP(session, questXP);

                ServerDeleteQuest.DeleteQuest(session, questIndex);

                //convert to logger
                Logger.Info($"Quest with {thisQuest.questID}, step {thisQuest.questStep} completed by player {session.MyCharacter.CharName}");

            }
        }

        public static void AddQuest(Session session, int questID, string questText)
        {
            Quest newQuest = new Quest();
            newQuest.questID = questID;
            newQuest.log = questText;

            if (session.MyCharacter.activeQuests.Count > 0)
                newQuest.questIndex = session.MyCharacter.activeQuests.Count + 1;

            else
                newQuest.questIndex = 0;

            //convert to logger
            Logger.Info($"Quest {questID} started by {session.MyCharacter.CharName}");
            session.MyCharacter.activeQuests.Add(newQuest);
            ServerAddQuestLog.AddQuestLog(session, (uint)newQuest.questIndex, newQuest.log);
        }
    }
}
