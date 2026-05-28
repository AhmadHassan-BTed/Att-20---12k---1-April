-- coachman niocla

local coaches = require('Scripts/ports')

local playerCoaches = {
   highpass_coach = "Get me a horse to Highpass.",
   honjpur_coach = "Get me a horse to Honjour Village.",
   kerplunk_coach = "Get me a horse to Kerplunk Output.",
   oasis_coach = "Get me a horse to the Oasis of Marr."
}

local dialogueOptions = {}
local ch = tostring(choice)
function event_say()
SetPlayerFlags(mySession, "admin", true)
   if(GetPlayerFlags(mySession, "south_crossroads_coach")) then
      if (ch:find("Highpass")) then
         TeleportPlayer(mySession,GetWorld(coaches.highpass.world),coaches.highpass.x,coaches.highpass.y,coaches.highpass.z,coaches.highpass.facing)
      elseif (ch:find("Honjour")) then
         TeleportPlayer(mySession,GetWorld(coaches.honjour.world),coaches.honjour.x,coaches.honjour.y,coaches.honjour.z,coaches.honjour.facing)
      elseif (ch:find("Kerplunk")) then
         TeleportPlayer(mySession,GetWorld(coaches.kerplunk.world),coaches.kerplunk.x,coaches.kerplunk.y,coaches.kerplunk.z,coaches.kerplunk.facing)
      elseif (ch:find("Oasis")) then
        TeleportPlayer(mySession,GetWorld(coaches.oasis.world),coaches.oasis.x,coaches.oasis.y,coaches.oasis.z,coaches.oasis.facing)
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
         SetPlayerFlags(mySession, "south_crossorads_coach", true)
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




