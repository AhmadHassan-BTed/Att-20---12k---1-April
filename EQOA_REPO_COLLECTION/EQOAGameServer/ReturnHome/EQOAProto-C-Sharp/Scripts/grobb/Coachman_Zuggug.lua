-- coachman Zuggug

local coaches = require('Scripts/ports')

local playerCoaches = {
   hazinak_dock_coach = "Get me a horse to da hazinak docks.",
   kerplunk_coach = "Get me a horse to da kerplunk outpost."
}
SetPlayerFlags(mySession, "admin", "true")
local dialogueOptions = {}
local ch = tostring(choice)
function event_say()
   if(GetPlayerFlags(mySession, "grobb_coach") == "true") then
      if (ch:find("kerplunk")) then
         TeleportPlayer(mySession,GetWorld(coaches.kerplunk.world),coaches.kerplunk.x,coaches.kerplunk.y,coaches.kerplunk.z,coaches.kerplunk.facing)
      elseif (ch:find("hazinak")) then
         TeleportPlayer(mySession,GetWorld(coaches.hazinak_dock.world),coaches.hazinak_dock.x,coaches.hazinak_dock.y,coaches.hazinak_dock.z,coaches.hazinak_dock.facing)
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
      print("in the yes")
         npcDialogue = "Excellent, you can now use this coach any time."
         SetPlayerFlags(mySession, "grobb_coach", "true")
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

