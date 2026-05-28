-- coachwoman dainei

local coaches = require('Scripts/ports')

local playerCoaches = {
   surefall_glade_coach = "Get me a horse to Surefall Glade.",
   castle_lightwolf_coach = "Get me a horse to Castle Lightwolf.",
   wyndhaven_coach = "Get me a horse to Wyndhaven."
}

local dialogueOptions = {}
local ch = tostring(choice)
function event_say()
   if(GetPlayerFlags(mySession, "murnf_coach") == "true") then
      if (ch:find("Glade")) then
         TeleportPlayer(mySession,GetWorld(coaches.surefall_glade.world),coaches.surefall_glade.x,coaches.surefall_glade.y,coaches.surefall_glade.z,coaches.surefall_glade.facing)
      elseif (ch:find("Lightwolf")) then
         TeleportPlayer(mySession,GetWorld(coaches.castle_lightwolf.world),coaches.castle_lightwolf.x,coaches.castle_lightwolf.y,coaches.castle_lightwolf.z,coaches.castle_lightwolf.facing)
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
         SetPlayerFlags(mySession, "murnf_coach","true")
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
