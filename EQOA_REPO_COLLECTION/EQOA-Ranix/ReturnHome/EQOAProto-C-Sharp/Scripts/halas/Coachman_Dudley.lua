-- coachman dudley

local coaches = require('Scripts/ports')

local playerCoaches = {
   moradhim_coach = "Get me a horse to Moradhim.",
   castle_lightwolf_coach = "Get me a horse to Castle Lightwolf."
}

local dialogueOptions = {}
local ch = tostring(choice)
function event_say()
   if(GetPlayerFlags(mySession, "halas_coach")) then
      if (ch:find("Moradhim")) then
         TeleportPlayer(mySession,GetWorld(coaches.moradhim.world),coaches.moradhim.x,coaches.moradhim.y,coaches.moradhim.z,coaches.moradhim.facing)
      elseif (ch:find("Lightwolf")) then
         TeleportPlayer(mySession,GetWorld(coaches.castle_lightwolf.world),coaches.castle_lightwolf.x,coaches.castle_lightwolf.y,coaches.castle_lightwolf.z,coaches.castle_lightwolf.facing)
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
         SetPlayerFlags(mySession, "halas_coach", true)
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
