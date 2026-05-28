-- coachman doba

local coaches = require('Scripts/ports')

local playerCoaches = {
   darvar_manor_coach = "Get me a horse to Darvar Manor'.",
   fayspires_coach = "Get me a horse to Fayspires.",
   halas_coach = "Get me a horse to Halas.",
   rivervale_coach = "Get me a horse to Rivervale."
}

local dialogueOptions = {}
local ch = tostring(choice)
function event_say()
   if(GetPlayerFlags(mySession, "moradhim_coach")) then
      if (ch:find("Darvar Manor")) then
         TeleportPlayer(mySession,GetWorld(coaches.darvar_manor.world),coaches.darvar_manor.x,coaches.darvar_manor.y,coaches.darvar_manor.z,coaches.darvar_manor.facing)
      elseif (ch:find("Fayspires")) then
         TeleportPlayer(mySession,GetWorld(coaches.fayspires.world),coaches.fayspires.x,coaches.fayspires.y,coaches.fayspires.z,coaches.fayspires.facing)
      elseif (ch:find("Halas")) then
         TeleportPlayer(mySession,GetWorld(coaches.halas.world),coaches.halas.x,coaches.halas.y,coaches.halas.z,coaches.halas.facing)
      elseif (ch:find("Rivervale")) then
         TeleportPlayer(mySession,GetWorld(coaches.rivervale.world),coaches.rivervale.x,coaches.rivervale.y,coaches.rivervale.z,coaches.rivervale.facing)
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
         SetPlayerFlags(mySession, "moradhim_coach", true)
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
