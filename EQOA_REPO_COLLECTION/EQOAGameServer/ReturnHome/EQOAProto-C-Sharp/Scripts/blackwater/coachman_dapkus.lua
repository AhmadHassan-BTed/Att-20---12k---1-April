-- coachman dapkus
local coaches = require("Scripts/ports")

local playerCoaches = {
    forkwatch_coach = "Get me a horse to Forkwatch.",
    gerntar_mines_coach = "Get me a horse to Gerntar's Mine",
    oasis_of_marr_coach = "Get me a horse to Oasis of Marr"
}

local dialogueOptions = {}
local ch = tostring(choice)
function event_say()
        if (GetPlayerFlags(mySession, "blackwater_coach") == "true") then
        if (ch:find("Forkwatch")) then
            TeleportPlayer(
                mySession,
                GetWorld(coaches.forkwatch.world),
                coaches.forkwatch.x,
                coaches.forkwatch.y,
                coaches.forkwatch.z,
                coaches.forkwatch.facing
            )
        elseif (ch:find("Mine")) then
            TeleportPlayer(
                mySession,
                GetWorld(coaches.gerntar_mines.world),
                coaches.gerntar_mines.x,
                coaches.gerntar_mines.y,
                coaches.gerntar_mines.z,
                coaches.gerntar_mines.facing
            )
        elseif (ch:find("Oasis")) then
            TeleportPlayer(
                mySession,
                GetWorld(coaches.oasis.world),
                coaches.oasis.x,
                coaches.oasis.y,
                coaches.oasis.z,
                coaches.oasis.facing
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
            SetPlayerFlags(mySession, "blackwater_coach", "true")
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
