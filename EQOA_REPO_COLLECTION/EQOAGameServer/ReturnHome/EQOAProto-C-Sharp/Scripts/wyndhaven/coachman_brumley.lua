-- coachman brumley

local coaches = require('Scripts/ports')

local playerCoaches = {
   murnf_coach = "Get me a horse to Murnf.",
   qeynos_coach = "Get me a horse to Qeynos.",
   surefall_glade_coach = "Get me a horse to Surefall Glade.",
   zentars_keep_coach = "Get me a horse to Zentar's Keep",
   fog_marsh_coach = "Get me a horse to Fog Marsh"
}

local dialogueOptions = {}
local ch = tostring(choice)
function event_say()
   if(GetPlayerFlags(mySession, "wyndhaven_coach") == "true") then
      if (ch:find("Murnf")) then
         TeleportPlayer(mySession,GetWorld(coaches.murnf.world),coaches.murnf.x,coaches.murnf.y,coaches.murnf.z,coaches.murnf.facing)
      elseif (ch:find("Qeynos")) then
         TeleportPlayer(mySession,GetWorld(coaches.qeynos.world),coaches.qeynos.x,coaches.qeynos.y,coaches.qeynos.z,coaches.qeynos.facing)
      elseif (ch:find("Surefall")) then
         TeleportPlayer(mySession,GetWorld(coaches.surefall_glade.world),coaches.surefall_glade.x,coaches.surefall_glade.y,coaches.surefall_glade.z,coaches.surefall_glade.facing)
      elseif (ch:find("Marsh")) then
         TeleportPlayer(mySession,GetWorld(coaches.fog_marsh.world),coaches.fog_marsh.x,coaches.fog_marsh.y,coaches.fog_marsh.z,coaches.fog_marsh.facing)
      elseif (ch:find("Keep")) then
         TeleportPlayer(mySession,GetWorld(coaches.zentars_keep.world),coaches.zentars_keep.x,coaches.zentars_keep.y,coaches.zentars_keep.z,coaches.zentars_keep.facing)
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
         SetPlayerFlags(mySession, "wyndhaven_coach", "true")
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




