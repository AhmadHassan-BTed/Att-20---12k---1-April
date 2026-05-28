-- coachman frender

local coaches = require('Scripts/ports')

local playerCoaches = {
   darvar_manor_coach = "Get me a horse to Darvar Manor.",
   qeynos_coach = "Get me a horse to Qeynos.",
   wyndhaven_coach = "Get me a horse to Wyndhaven."
}

local dialogueOptions = {}
local ch = tostring(choice)
function event_say()
SetPlayerFlags(mySession, "admin", true)
   if(GetPlayerFlags(mySession, "surefall_coach")) then
      if (ch:find("Darvar")) then
         TeleportPlayer(mySession,GetWorld(coaches.darvar_manor.world),coaches.darvar_manor.x,coaches.darvar_manor.y,coaches.darvar_manor.z,coaches.darvar_manor.facing)
      elseif (ch:find("Qeynos")) then
         TeleportPlayer(mySession,GetWorld(coaches.qeynos.world),coaches.qeynos.x,coaches.qeynos.y,coaches.qeynos.z,coaches.qeynos.facing)
      elseif (ch:find("Wyndhaven")) then
         TeleportPlayer(mySession,GetWorld(coaches.wyndhaven.world),coaches.wyndhaven.x,coaches.wyndhaven.y,coaches.wyndhaven.z,coaches.wyndhaven.facing)
      else
         npcDialogue = "Where would you like to go?"
         for coach, diag in pairs(playerCoaches) do
            if (GetPlayerFlags(mySession, coach) or GetPlayerFlags(mySession, "admin")) then
               table.insert(dialogueOptions, diag)
            end
         end
         SendDialogue(mySession, npcDialogue, dialogueOptions)
      end
   else
      if (ch:find("Yes")) then
         npcDialogue = "Excellent, you can now use this coach any time."
         SetPlayerFlags(mySession, "surefall_coach", true)
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




