local playerCoaches = {
   darvar_coach = "Get me a horse to Darvar Manor.",
   seriak_coach = "Get me a horse to Ft. Seriak.",
   freeport_coach = "Get me a horse to Freeport.",
   solace_coach = "Get me a horse to the city of Dark Solace.",
   rivervale_coach = "Get me a horse to the halfling home of Rivervale"
}
local coaches = require('Scripts/ports')
local ch = tostring(choice)
SetPlayerFlags(mySession, "admin", true)
function event_say()
   local dialogueOptions = {}
   if(GetPlayerFlags(mySession, "highpass_coach")) then
      if (ch:find("Freeport")) then
         TeleportPlayer(
         mySession,
         GetWorld(coaches.freeport.world),
         coaches.freeport.x,
         coaches.freeport.y,
         coaches.freeport.z,
         coaches.freeport.facing
         )
      elseif (ch:find("Seriak")) then
         TeleportPlayer(
         mySession,
         GetWorld(coaches.fort_seriak.world),
         coaches.fort_seriak.x,
         coaches.fort_seriak.y,
         coaches.fort_seriak.z,
         coaches.fort_seriak.facing
         )
      elseif (ch:find("Solace")) then
         TeleportPlayer(
         mySession,
         GetWorld(coaches.dark_solace.world),
         coaches.dark_solace.x,
         coaches.dark_solace.y,
         coaches.dark_solace.z,
         coaches.dark_solace.facing
         )
      elseif (ch:find("Rivervale")) then
         TeleportPlayer(
         mySession,
         GetWorld(coaches.rivervale.world),
         coaches.rivervale.x,
         coaches.rivervale.y,
         coaches.rivervale.z,
         coaches.rivervale.facing
         )
         elseif (ch:find("Darvar")) then
         SendDialogue(mySession, npcDialogue, dialogueOptions)
         TeleportPlayer(
         mySession,
         GetWorld(coaches.darvar_manor.world),
         coaches.darvar_manor.x,
         coaches.darvar_manor.y,
         coaches.darvar_manor.z,
         coaches.darvar_manor.facing
         )
      else
        local npcDialogue = "Where would you like to go?"
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
      SetPlayerFlags(mySession, "highpass_coach", true)
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


