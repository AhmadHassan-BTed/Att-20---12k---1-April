-- coachman opelim

local coaches = require('Scripts/ports')

local playerCoaches = {
   fort_seriak_coach = "Get me a horse to Fort Seriak.",
   fog_marsh_coach = "Get me a horse to Fog Marsh."
}

local dialogueOptions = {}
local ch = tostring(choice)
function event_say()
   if(GetPlayerFlags(mySession, "last_inn_coach") == "true") then
      if (ch:find("Seriak")) then
         TeleportPlayer(mySession,GetWorld(coaches.fort_seriak.world),coaches.fort_seriak.x,coaches.fort_seriak.y,coaches.fort_seriak.z,coaches.fort_seriak.facing)
      elseif (ch:find("Marsh")) then
         TeleportPlayer(mySession,GetWorld(coaches.fog_marsh.world),coaches.fog_marsh.x,coaches.fog_marsh.y,coaches.fog_marsh.z,coaches.fog_marsh.facing)
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
         SetPlayerFlags(mySession, "last_inn_coach","true")
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
