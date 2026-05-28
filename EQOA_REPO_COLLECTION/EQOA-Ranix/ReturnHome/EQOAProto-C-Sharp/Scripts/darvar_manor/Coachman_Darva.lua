-- coachman darva

local coaches = require('Scripts/ports')

local playerCoaches = {
   forkwatch_coach = "Get me a horse to Forkwatch.",
   highpass_coach = "Get me a horse to Highpass Hold.",
   moradhim_coach = "Get me a horse to Moradhim.",
   surefall_glade_coach = "Get me a horse to Surefall Glade."
}

local dialogueOptions = {}
local ch = tostring(choice)
function event_say()
   if(GetPlayerFlags(mySession, "darvar_manor_coach")) then
      if (ch:find("Forkwatch")) then
         TeleportPlayer(mySession,GetWorld(coaches.forkwatch.world),coaches.forkwatch.x,coaches.forkwatch.y,coaches.forkwatch.z,coaches.forkwatch.facing)
      elseif (ch:find("Highpass")) then
         TeleportPlayer(mySession,GetWorld(coaches.highpass.world),coaches.highpass.x,coaches.highpass.y,coaches.highpass.z,coaches.highpass.facing)
      elseif (ch:find("Moradhim")) then
         TeleportPlayer(mySession,GetWorld(coaches.moradhim.world),coaches.moradhim.x,coaches.moradhim.y,coaches.moradhim.z,coaches.moradhim.facing)
      elseif (ch:find("Surefall")) then
         TeleportPlayer(mySession,GetWorld(coaches.surefall_glade.world),coaches.surefall_glade.x,coaches.surefall_glade.y,coaches.surefall_glade.z,coaches.surefall_glade.facing)
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
         SetPlayerFlags(mySession, "darvar_manor_coach", true)
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




