-- coachman galdah

local coaches = require("Scripts/ports")

local playerCoaches = {
    qeynos_coach = "Get me a horse to Qeynos.",
    forkwatch_coach = "Get me a horse to Forkwatch."
}

local dialogueOptions = {}
local ch = tostring(choice)
function event_say()
        if (GetPlayerFlags(mySession, "highbourne_coach") == "true") then
        if (ch:find("Qeynos")) then
            TeleportPlayer(
                mySession,
                GetWorld(coaches.qeynos.world),
                coaches.qeynos.x,
                coaches.qeynos.y,
                coaches.qeynos.z,
                coaches.qeynos.facing
            )
        elseif (ch:find("Forkwatch")) then
            TeleportPlayer(
                mySession,
                GetWorld(coaches.forkwatch.world),
                coaches.forkwatch.x,
                coaches.forkwatch.y,
                coaches.forkwatch.z,
                coaches.forkwatch.facing
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
            SetPlayerFlags(mySession, "highbourne_coach", "true")
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
