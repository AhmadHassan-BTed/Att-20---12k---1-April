-- coachman ronks

local coaches = require('Scripts/ports')

local playerCoaches = {
   highpass_coach = "Get me a horse to Highpass.",
   bobble_coach = "Get me a horse to Bobble By Water.",
   tea_garden_coach = "Get me a horse to Muniel's Tea Garden.",
   neriak_coach = "Get me a horse to the dark city of Neriak."
}

local dialogueOptions = {}
local ch = tostring(choice)
function event_say()
SetPlayerFlags(mySession, "admin", true)
   if(GetPlayerFlags(mySession, "freeport_coach")) then
      if (ch:find("Highpass")) then
         TeleportPlayer(mySession,GetWorld(coaches.highpass.world),coaches.highpass.x,coaches.highpass.y,coaches.highpass.z,coaches.highpass.facing)
      elseif (ch:find("Tea")) then
         TeleportPlayer(mySession,GetWorld(coaches.muniels_tea_garden.world),coaches.muniels_tea_garden.x,coaches.muniels_tea_garden.y,coaches.muniels_tea_garden.z,coaches.muniels_tea_garden.facing)
      elseif (ch:find("Bobble")) then
         TeleportPlayer(mySession,GetWorld(coaches.bobble_by_water.world),coaches.bobble_by_water.x,coaches.bobble_by_water.y,coaches.bobble_by_water.z,coaches.bobble_by_water.facing)
      elseif (ch:find("Neriak")) then
        TeleportPlayer(mySession,GetWorld(coaches.neriak.world),coaches.neriak.x,coaches.neriak.y,coaches.neriak.z,coaches.neriak.facing)
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
         SetPlayerFlags(mySession, "freeport_coach", true)
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



