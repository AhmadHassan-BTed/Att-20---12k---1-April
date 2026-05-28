-- coachman norial

local coaches = require('Scripts/ports')

local playerCoaches = {
   last_inn_coach = "Get me a horse to the Last Inn.",
   dark_solace_coach = "Get me a horse to Dark Solace.",
   wyndhaven_coach = "Get me a horse to Wyndhaven."
}

local dialogueOptions = {}
local ch = tostring(choice)
function event_say()
   if(GetPlayerFlags(mySession, "fog_marsh_coach") == "true") then
      if (ch:find("Last")) then
         TeleportPlayer(mySession,GetWorld(coaches.mt_hatespike.world),coaches.mt_hatespike.x,coaches.mt_hatespike.y,coaches.mt_hatespike.z,coaches.mt_hatespike.facing)
      elseif (ch:find("Solace")) then
         TeleportPlayer(mySession,GetWorld(coaches.dark_solace.world),coaches.dark_solace.x,coaches.dark_solace.y,coaches.dark_solace.z,coaches.dark_solace.facing)
      elseif (ch:find("Wyndhaven")) then
         TeleportPlayer(mySession,GetWorld(coaches.wyndhaven.world),coaches.wyndhaven.x,coaches.wyndhaven.y,coaches.wyndhaven.z,coaches.wyndhaven.facing)
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
         SetPlayerFlags(mySession, "fog_marsh_coach","true")
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
