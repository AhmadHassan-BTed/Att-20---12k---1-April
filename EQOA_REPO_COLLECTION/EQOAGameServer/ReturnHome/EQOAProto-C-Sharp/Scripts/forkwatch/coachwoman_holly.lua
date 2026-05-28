-- coachwoman holly

local coaches = require("Scripts/ports")

local playerCoaches = {
    qeynos_coach = "Get me a horse to Qeynos.",
    darvar_coach = "Get me a horse to Darvar Manor.",
    blackwater_coach = "Get me a horse to Blackwater",
    highbourne_coach = "Get me a horse to Highbourne"
}

local dialogueOptions = {}
local ch = tostring(choice)
function event_say()
        if (GetPlayerFlags(mySession, "forkwatch_coach") == "true") then
        if (ch:find("Qeynos")) then
            TeleportPlayer(
                mySession,
                GetWorld(coaches.qeynos.world),
                coaches.qeynos.x,
                coaches.qeynos.y,
                coaches.qeynos.z,
                coaches.qeynos.facing
            )
        elseif (ch:find("Darvar")) then
            TeleportPlayer(
                mySession,
                GetWorld(coaches.darvar_manor.world),
                coaches.darvar_manor.x,
                coaches.darvar_manor.y,
                coaches.darvar_manor.z,
                coaches.darvar_manor.facing
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
        elseif (ch:find("Highbourne")) then
            TeleportPlayer(
                mySession,
                GetWorld(coaches.highbourne.world),
                coaches.highbourne.x,
                coaches.highbourne.y,
                coaches.highbourne.z,
                coaches.highbourne.facing
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
            SetPlayerFlags(mySession, "forkwatch_coach", "true")
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
