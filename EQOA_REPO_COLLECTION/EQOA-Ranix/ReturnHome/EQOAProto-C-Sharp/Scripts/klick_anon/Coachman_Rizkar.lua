-- coachman rizkar

local coaches = require('Scripts/ports')

local playerCoaches = {
   neriak_coach = "Get me a horse to Neriak.",
   fayspires_coach = "Get me a horse to Fayspires.",
   bobble_by_water_coach = "Get me a horse to Bobble By Water.",
}

local dialogueOptions = {}
local ch = tostring(choice)
function event_say()
   if(GetPlayerFlags(mySession, "klick_anon_coach")) then
      if (ch:find("Neriak")) then
         TeleportPlayer(mySession,GetWorld(coaches.neriak.world),coaches.neriak.x,coaches.neriak.y,coaches.neriak.z,coaches.neriak.facing)
      elseif (ch:find("Fayspires")) then
         TeleportPlayer(mySession,GetWorld(coaches.fayspires.world),coaches.fayspires.x,coaches.fayspires.y,coaches.fayspires.z,coaches.fayspires.facing)
      elseif (ch:find("Bobble")) then
         TeleportPlayer(mySession,GeWorld(coaches.bobble_by_water.world),coaches.bobble_by_water.x,coaches.bobble_by_water.y,coaches.bobble_by_water.z,coaches.bobble_by_water.facing)
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
         SetPlayerFlags(mySession, "klick_anon_coach", true)
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
