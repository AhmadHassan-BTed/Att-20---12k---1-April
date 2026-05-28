-- coachman bilfer

local coaches = require('Scripts/ports')

local playerCoaches = {
   highbourne_coach = "Get me a horse to Highbourne.",
   wyndhaven_coach = "Get me a horse to Wyndhaven.",
   forkwatch_coach = "Get me a horse to Forkwatch.",
   surefall_coach = "Get me a horse to Surefall Glade."
}

local dialogueOptions = {}
local ch = tostring(choice)
function event_say()
   if(GetPlayerFlags(mySession, "qeynos_coach") == "true") then
      if (ch:find("Highbourne")) then
         TeleportPlayer(mySession,GetWorld(coaches.highbourne.world),coaches.highbourne.x,coaches.highbourne.y,coaches.highbourne.z,coaches.highbourne.facing)
      elseif (ch:find("Wyndhaven")) then
         TeleportPlayer(mySession,GetWorld(coaches.wyndhaven.world),coaches.wyndhaven.x,coaches.wyndhaven.y,coaches.wyndhaven.z,coaches.wyndhaven.facing)
      elseif (ch:find("Forkwatch")) then
         TeleportPlayer(mySession,GetWorld(coaches.forkwatch.world),coaches.forkwatch.x,coaches.forkwatch.y,coaches.forkwatch.z,coaches.forkwatch.facing)
      elseif (ch:find("Surefall")) then
        TeleportPlayer(mySession,GetWorld(coaches.surefall_glade.world),coaches.surefall_glade.x,coaches.surefall_glade.y,coaches.surefall_glade.z,coaches.surefall_glade.facing)
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
         SetPlayerFlags(mySession, "qeynos_coach", "true")
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



