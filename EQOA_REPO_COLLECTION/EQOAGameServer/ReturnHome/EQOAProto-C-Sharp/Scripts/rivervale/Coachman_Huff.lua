local coaches = require('Scripts/ports')


local playerCoaches = {
    bobble_coach = "Get me a horse to Bobble by water.",
   moradhim_coach = "Get me a horse to Moradhim.",
   fayspires_coach = "Get me a horse to Fayspires",
   highpass_coach = "Get me a horse to Highpass"
}

local dialogueOptions = {}
local ch = tostring(choice)
function event_say()
   if(GetPlayerFlags(mySession, "rivervale_coach") == "true") then
      if (ch:find("Moradhim")) then
         TeleportPlayer(mySession,GetWorld(coaches.moradhim.world),coaches.moradhim.x,coaches.moradhim.y,coaches.moradhim.z,coaches.moradhim.facing)
      elseif (ch:find("Fayspires")) then
         TeleportPlayer(mySession,GetWorld(coaches.fayspires.world),coaches.fayspires.x,coaches.fayspires.y,coaches.fayspires.z,coaches.fayspires.facing)
      elseif (ch:find("Bobble")) then
         TeleportPlayer(mySession,GetWorld(coaches.bobble_by_water.world),coaches.bobble_by_water.x,coaches.bobble_by_water.y,coaches.bobble_by_water.z,coaches.bobble_by_water.facing)
      elseif (ch:find("Highpass")) then
         TeleportPlayer(mySession,GetWorld(coaches.highpass.world),coaches.highpass.x,coaches.highpass.y,coaches.highpass.z,coaches.highpass.facing)
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
         SetPlayerFlags(mySession, "rivervale_coach", "true")
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

