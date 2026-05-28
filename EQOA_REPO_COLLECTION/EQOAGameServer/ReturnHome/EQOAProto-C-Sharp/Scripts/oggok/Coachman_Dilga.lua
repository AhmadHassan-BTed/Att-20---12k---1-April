-- coachman dilga

local coaches = require('Scripts/ports')

local playerCoaches = {
   kerplunk_coach = "Get me a horse to Kerplunk Outpost.",
   dark_solace_coach = "Get me a horse to Dark Solace."
}

local dialogueOptions = {}
local ch = tostring(choice)
function event_say()
   if(GetPlayerFlags(mySession, "oggok_coach") == "true") then
      if (ch:find("Kerplunk")) then
         TeleportPlayer(mySession,GetWorld(coaches.kerplunk.world),coaches.kerplunk.x,coaches.kerplunk.y,coaches.kerplunk.z,coaches.kerplunk.facing)
      elseif (ch:find("Solace")) then
         TeleportPlayer(mySession,GetWorld(coaches.dark_solace.world),coaches.dark_solace.x,coaches.dark_solace.y,coaches.dark_solace.z,coaches.dark_solace.facing)
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
         SetPlayerFlags(mySession, "oggok_coach", "true")
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



