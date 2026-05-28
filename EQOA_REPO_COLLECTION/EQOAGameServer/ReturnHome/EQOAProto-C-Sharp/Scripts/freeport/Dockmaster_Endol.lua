-- dockmaster endodl

local coaches = require("Scripts/ports")

local ch = tostring(choice)
function event_say()
diagOptions = {}
    npcDialogue = "I love listening to the waves crash.  I couldn't tell you of a more peaceful existence."
    if (ch:find("Hazinak")) then
        TeleportPlayer(
            mySession,
            GetWorld(coaches.hazinak_ferry.world),
            coaches.hazinak_ferry.x,
            coaches.hazinak_ferry.y,
            coaches.hazinak_ferry.z,
            coaches.hazinak_ferry.facing
        )
    else
        npcDialogue = "Where would you like to go?"
        dialogueOptions = {"I would like passage bound for Hazinak."}
        SendDialogue(mySession, npcDialogue, dialogueOptions)
    end
end
