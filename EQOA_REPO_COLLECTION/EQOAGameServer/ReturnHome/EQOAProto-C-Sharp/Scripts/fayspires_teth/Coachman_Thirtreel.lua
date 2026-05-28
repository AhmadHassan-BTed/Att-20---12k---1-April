-- coachman thirtreel

local coaches = require('Scripts/ports')

local playerCoaches = {
   klick_anon_coach = "Get me a horse to Klick'Anon.",
   moradhim_coach = "Get me a horse to Moradhim.",
   tethelin_coach = "Get me a horse to Tethelin.",
   rivervale_coach = "Get me a horse to Rivervale."
}

local dialogueOptions = {}
local ch = tostring(choice)
function event_say()
   if(GetPlayerFlags(mySession, "fayspires_coach") == "true") then
      if (ch:find("Klick'Anon")) then
         TeleportPlayer(mySession,GetWorld(coaches.klick_anon.world),coaches.klick_anon.x,coaches.klick_anon.y,coaches.klick_anon.z,coaches.klick_anon.facing)
      elseif (ch:find("Moradhim")) then
         TeleportPlayer(mySession,GetWorld(coaches.moradhim.world),coaches.moradhim.x,coaches.moradhim.y,coaches.moradhim.z,coaches.moradhim.facing)
      elseif (ch:find("Tethelin")) then
         TeleportPlayer(mySession,GetWorld(coaches.tethelin.world),coaches.tethelin.x,coaches.tethelin.y,coaches.tethelin.z,coaches.tethelin.facing)
      elseif (ch:find("Rivervale")) then
         TeleportPlayer(mySession,GetWorld(coaches.rivervale.world),coaches.rivervale.x,coaches.rivervale.y,coaches.rivervale.z,coaches.rivervale.facing)
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
         SetPlayerFlags(mySession, "fayspires_coach", "true")
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




