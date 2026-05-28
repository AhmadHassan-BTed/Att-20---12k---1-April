-- coachman ungar
local coaches = require("Scripts/ports")
local playerCoaches = {
    forkwatch_coach = "Get me a horse to Blackwater."
}

local dialogueOptions = {}
local ch = tostring(choice)
function event_say()
        if (GetPlayerFlags(mySession, "gerntar_mines_coach") == "true") then
        if (ch:find("Blackwater")) then
            TeleportPlayer(
                mySession,
                GetWorld(coaches.blackwater.world),
                coaches.blackwater.x,
                coaches.blackwater.y,
                coaches.blackwater.z,
                coaches.blackwater.facing
            )
        else
            npcDialogue = "Where would you like to go?"
            for coach, diag in pairs(playerCoaches) do
                if ((GetPlayerFlags(mySession, "admin") or GetPlayerFlags(mySession, coach)) == "true") then
                    table.insert(dialogueOptions, diag)
                end
            end
            SendDialogue(mySession, npcDialogue, dialogueOptions)
        end
    else
        if (ch:find("Yes")) then
            npcDialogue = "Excellent, you can now use this coach any time."
            SetPlayerFlags(mySession, "gerntar_mines_coach", "true")
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
