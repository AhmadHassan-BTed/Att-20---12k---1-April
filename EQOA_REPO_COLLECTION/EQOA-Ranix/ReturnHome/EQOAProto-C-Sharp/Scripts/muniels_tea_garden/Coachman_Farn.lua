local playerCoaches = {
   freeport_coach = "Get me a horse to Freeport.",
   oasis_coach = "Get me a horse to the Oasis of Marr."
}

local coaches = require('Scripts/ports')

}
local ch = tostring(choice)
function event_say()
   dialogueOptions = {}
   SetPlayerFlags(mySession, "freeport_coach", true)

   if(GetPlayerFlags(mySession, "tea_garden_coach")) then
      if (ch:find("Freeport")) then
         TeleportPlayer(
         mySession,
         GetWorld(coaches.Freeport.world),
         coaches.Freeport.x,
         coaches.Freeport.y,
         coaches.Freeport.z,
         coaches.Freeport.facing
         )
      elseif (ch:find("Oasis")) then
         TeleportPlayer(
         mySession,
         GetWorld(coaches.Oasis.world),
         coaches.Oasis.x,
         coaches.Oasis.y,
         coaches.Oasis.z,
         coaches.Oasis.facing
         )
      else
         npcDialogue = "Where would you like to go?"
         for coach, diag in pairs(playerCoaches) do
            if (GetPlayerFlags(mySession, coach)) then
               table.insert(dialogueOptions, diag)
            end
            
         end
         
                     SendDialogue(mySession, npcDialogue, dialogueOptions)

      end
   else

      if (ch:find("Yes")) then
         npcDialogue = "Excellent, you can now use this coach any time."
         SetPlayerFlags(mySession, "tea_garden_coach", true)
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


