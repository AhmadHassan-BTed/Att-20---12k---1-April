local ch = tostring(choice)
local quests = require('Scripts/FreeportQuests')
function event_say()
   local diagOptions = {}
   local questText = ""
   local npcDialogue = ""
   local questRewards = {}
   if
   (class == "Necromancer" and race == "Human" and humanType == "Eastern" and
   GetPlayerFlags(mySession, "11010") == "noFlags")
   then
      SetPlayerFlags(mySession, "11010", "0")
   end
   if (GetPlayerFlags(mySession, "11010") == "0") then
      if (ch:find("wish")) then
         multiDialogue = {
            "Corious Slaerin: You couldn't possibly know that, until you've seen with your own eyes the power of raising the dead to life with your own will.",
            "Corious Slaerin: But if you insist, I will expect you to complete a number of tasks before you will earn the title of necromancer.",
            "Corious Slaerin: Your first task is to acquire a Bone Earring from the merchant Gilgash. Your fee for this will be waived.",
            "Corious Slaerin: When you have the Bone Earring return to me and I'll send you on your second task.",
            "You have received a quest!"
         }
         SendMultiDialogue(mySession, multiDialogue)
         StartQuest(mySession, 11010, quests[11010][0].log)
      else
         npcDialogue = "If you have something to say, say it."
         diagOptions = {"I wish to be a necromancer of The House Slaerin."}
      end
   elseif (GetPlayerFlags(mySession, "11010") == "1") then
      if (CheckQuestItem(mySession, 8459, 1)) then
         if (ch:find("nevermind")) then
            npcDialogue = "Well don't just stand around wasting my time"
         elseif (ch:find("earring")) then
            multiDialogue = { "Corious Slaerin: It's going to take more than running errands to learn this power. We are going to have to test your wit and your will...",
            "Corious Slaerin: I will have your next task ready in a few moments. Don’t wander off now...",
            "You have finished a quest!" }
            SendMultiDialogue(mySession, multiDialogue)
            CompleteQuest(mySession, 11010, quests[11010][1].xp)
         else
            npcDialogue = "Do you have the earring yet?"
            diagOptions = { "I have the earring.", "Oh, nevermind." }
         end
      else
         npcDialogue = "If you can't follow my instructions, why shouldn't I just turn you into one of my pets right now? You will need the Bone Earring from Merchant Gilgash, then return to me."
      end
   elseif (GetPlayerFlags(mySession, "11011") == "0") then
      if (ch:find("ready")) then
         multiDialogue = { "Corious Slaerin: A necromancer's unholy spells specialize in robbing enemies of their physical abilities and health in order to bolster themselves and their party.",
         "Corious Slaerin: They also can command undead pets equal in strength to those of a magician. A skilled necromancer can fight alone or in a group, though many necromancers either choose or are forced to walk alone.",
         "Corious Slaerin: In a group, they can aid the attack by sending their pets into battle and assist with health and power restoration by transferring their own health and power to those in need.",
         "Corious Slaerin: As expected, most races shun necromancers due to their practice of the dark arts, making this a difficult path to pursue.",
         "Corious Slaerin: Now listen carefully. I need you to speak to Spiritmaster Keika.",
         "Corious Slaerin: You can find her just outside, to the east. Return only when you complete any tasks she gives you.",
         "You have received a quest!" }
         SendMultiDialogue(mySession, multiDialogue)
         StartQuest(mySession, 11011, quests[11011][0].log)
      else
         npcDialogue = "If you have something to say, say it."
         diagOptions = { "I am ready for my next test." }
      end
   elseif (GetPlayerFlags(mySession, "11011") == "3") then
      if (ch:find("Yes")) then
         multiDialogue = { "Corious Slaerin: Without fail, be sure to have yourself bound often. It is quite inconvenient to be defeated far from your last binding.",
         "Corious Slaerin: Now that you've completed that, I have another task for you. Go see Rathei Slaerin, she will assist you.",
         "Corious Slaerin: You can find Rathei Slaerin just behind me.",
         "You have finished a quest!"}
         SendMultiDialogue(mySession, multiDialogue)
         CompleteQuest(mySession, 11011, quests[11011][3].xp)
      elseif (ch:find("Sorry")) then
            npcDialogue = "Don't stand around wasting my time"
      else
         npcDialogue = "Have you completed the tasks that I sent you for?"
         diagOptions = {"Yes, it is done", "Sorry, i'll be on my way."}
      end
   else
      npcDialogue = "Corious_Slaerin: Away with you! Go now, or I will turn your corpse into a mound of bone and flesh!"
   end
   SendDialogue(mySession, npcDialogue, diagOptions)
end
