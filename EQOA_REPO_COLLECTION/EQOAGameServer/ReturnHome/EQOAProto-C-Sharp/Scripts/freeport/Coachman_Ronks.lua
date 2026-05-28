-- coachman ronks
local coaches = require("Scripts/ports")
local quests = require("Scripts/FreeportQuests")
local playerCoaches = {
   highpass_coach = "Get me a horse to Highpass.",
   bobble_coach = "Get me a horse to Bobble By Water.",
   tea_garden_coach = "Get me a horse to Muniel's Tea Garden.",
   neriak_coach = "Get me a horse to the dark city of Neriak."
}
SetPlayerFlags(mySession, "admin", "true")
local dialogueOptions = {}
local ch = tostring(choice)
function event_say()
   --Magician                          --Enchanter
   if ((GetPlayerFlags(mySession, "10011") or GetPlayerFlags(mySession, "12011")) == "2") then
      npcDialogue = "What do you want?"
      diagOptions = {"Spiritmaster Alshan sent me.", "Nothing"}
      if (ch:find("Alshan")) then
         multiDialogue = {
            "Coachman Ronks: Oh...from Spiritmaster Alshan. You know, I'm getting sick and tired of the...well, nevermind.",
            "Coachman Ronks: Apparently, it is my duty as a citizen of this \"fine\" city to add you to the stable ledger.",
            "Coachman Ronks: This means that if you find yourself in a nearby city, you can visit the stable there and pay for a ride to Freeport.",
            "Coachman Ronks: Without that record, they cannot offer you passage. If you do, then one of their horses will take you to your destination.",
            "Coachman Ronks: All you need to do is talk to the coachman of the stable and he will add you to his ledger. I'll add you to mine now.",
            "Coachman Ronks: Ok, I've done my duty. You may return to your masters now. Be sure to tell them how much I appreciate the business."
         }
         SendMultiDialogue(mySession, multiDialogue)
         --Magician
         if (GetPlayerFlags(mySession, "10011") == "2") then
            ContinueQuest(mySession, 10011, quests[10011][2].log)
            --Enchanter
         elseif (GetPlayerFlags(mySession, "12011") == "2") then
            ContinueQuest(mySession, 12011, quests[12011][2].log)
         end
         SetPlayerFlags(mySession, "freeport_coach", "true")
         npcDialogue = ""
      elseif (ch:find("Nothing")) then
         npcDialogue = "Coachman Ronks: Well don't just stand around here, I have work to do!"
         diagOptions = {}
      end
      SendDialogue(mySession, npcDialogue, diagOptions)
      SetPlayerFlags(mySession, "freeport_coach", "true")
   elseif ((GetPlayerFlags(mySession, "11011") or GetPlayerFlags(mySession, "09011")) == "2") then
      npcDialogue = "What do you want?"
      diagOptions = {"Spiritmaster Keika sent me.", "Nothing"}
      if (ch:find("Keika")) then
         multiDialogue = {
            "Coachman Ronks: Oh...from Spiritmaster Keika. You know, I'm getting sick and tired of the...well, nevermind.",
            "Coachman Ronks: Apparently, it is my duty as a citizen of this \"fine\" city to add you to the stable ledger.",
            "Coachman Ronks: This means that if you find yourself in a nearby city, you can visit the stable there and pay for a ride to Freeport.",
            "Coachman Ronks: Without that record, they cannot offer you passage. If you do, then one of their horses will take you to your destination.",
            "Coachman Ronks: All you need to do is talk to the coachman of the stable and he will add you to his ledger. I'll add you to mine now.",
            "Coachman Ronks: Ok, I've done my duty. You may return to your masters now. Be sure to tell them how much I appreciate the business."
         }
         SendMultiDialogue(mySession, multiDialogue)
         --Necromancer
      if (GetPlayerFlags(mySession, "11011") == "2") then
         ContinueQuest(mySession, 11011, quests[11011][2].log)
         --Cleric
      elseif (GetPlayerFlags(mySession, "09011") == "2") then
         ContinueQuest(mySession, 09011, quests[09011][2].log)
      end
      SetPlayerFlags(mySession, "freeport_coach", "true")
      npcDialogue = ""
      elseif (ch:find("Nothing")) then
         npcDialogue = "Coachman Ronks: Well don't just stand around here, I have work to do!"
         diagOptions = {}
      end
   SendDialogue(mySession, npcDialogue, diagOptions)
   SetPlayerFlags(mySession, "freeport_coach", "true")
elseif (GetPlayerFlags(mySession, "10011") ~= "1") then
   if (GetPlayerFlags(mySession, "freeport_coach") == "true") then
      if (ch:find("Highpass")) then
         TeleportPlayer(
         mySession,
         GetWorld(coaches.highpass.world),
         coaches.highpass.x,
         coaches.highpass.y,
         coaches.highpass.z,
         coaches.highpass.facing
         )
      elseif (ch:find("Tea")) then
         TeleportPlayer(
         mySession,
         GetWorld(coaches.muniels_tea_garden.world),
         coaches.muniels_tea_garden.x,
         coaches.muniels_tea_garden.y,
         coaches.muniels_tea_garden.z,
         coaches.muniels_tea_garden.facing
         )
      elseif (ch:find("Bobble")) then
         TeleportPlayer(
         mySession,
         GetWorld(coaches.bobble_by_water.world),
         coaches.bobble_by_water.x,
         coaches.bobble_by_water.y,
         coaches.bobble_by_water.z,
         coaches.bobble_by_water.facing
         )
      elseif (ch:find("Neriak")) then
         TeleportPlayer(
         mySession,
         GetWorld(coaches.neriak.world),
         coaches.neriak.x,
         coaches.neriak.y,
         coaches.neriak.z,
         coaches.neriak.facing
         )
      else
         npcDialogue = "Where would you like to go?"
         for coach, diag in pairs(playerCoaches) do
            if ((GetPlayerFlags(mySession, "admin")  or GetPlayerFlags(mySession, coach)) == "true") then
               table.insert(dialogueOptions, diag)
            end
         end
         SendDialogue(mySession, npcDialogue, dialogueOptions)
      end
   else
      if (ch:find("Yes")) then
         npcDialogue = "Excellent, you can now use this coach any time."
         SetPlayerFlags(mySession, "freeport_coach", "true")
         SendDialogue(mySession, npcDialogue, dialogueOptions)
      elseif (ch:find("No")) then
         npcDialogue = "If you aren't interested then why are you wasting my time."
         SendDialogue(mySession, npcDialogue, dialogueOptions)
      else
         npcDialogue = "Would you like to sign the coachman's ledger?"
         dialogueOptions = {"Yes", "No"}
         SendDialogue(mySession, npcDialogue, dialogueOptions)
      end
   end
end
end
