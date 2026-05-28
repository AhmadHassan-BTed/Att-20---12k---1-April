--coachwoman dernia
local playerCoaches = {
   hazinak_docks_coach = "Get me a horse to the Hazinak Docks.",
   fort_seriak_coach = "Get me a horse to Muniel's Tea Garden.",
   hazinak_temple_coach = "Get me a horse to the Hazinak Temple.",
   blackwater_coach = "Get me a horse to Blackwater.",
   dark_solace_coach = "Get me a horse to Dark Solace"
}
local coaches = require('Scripts/ports')
local ch = tostring(choice)
function event_say()
   local dialogueOptions = {}
   if(GetPlayerFlags(mySession, "oasis_of_marr_coach") == "true") then
      if (ch:find("Docks")) then
         TeleportPlayer(
         mySession,
         GetWorld(coaches.hazinak_dock.world),
         coaches.hazinak_dock.x,
         coaches.hazinak_dock.y,
         coaches.hazinak_dock.z,
         coaches.hazinak_dock.facing
         )
      elseif (ch:find("Garden")) then
         TeleportPlayer(
         mySession,
         GetWorld(coaches.muniels_tea_garden.world),
         coaches.muniels_tea_garden.x,
         coaches.muniels_tea_garden.y,
         coaches.muniels_tea_garden.z,
         coaches.muniels_tea_garden.facing
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
      elseif (ch:find("Temple")) then
         TeleportPlayer(
         mySession,
         GetWorld(coaches.hazinak_temple.world),
         coaches.hazinak_temple.x,
         coaches.hazinak_temple.y,
         coaches.hazinak_temple.z,
         coaches.hazinak_temple.facing
         )
         elseif (ch:find("Blackwater")) then
         TeleportPlayer(
         mySession,
         GetWorld(coaches.blackwater.world),
         coaches.blackwater.x,
         coaches.blackwater.y,
         coaches.blackwater.z,
         coaches.blackwater.facing
         )
      else
        local npcDialogue = "Where would you like to go?"
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
      SetPlayerFlags(mySession, "oasis_of_marr_coach", "true")
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



