-- dockmaster deleth

local coaches = require("Scripts/ports")

local ch = tostring(choice)
function event_say()
    if (ch:find("Freeport")) then
        TeleportPlayer(
            mySession,
            GetWorld(coaches.freeport_ferry.world),
            coaches.freeport_ferry.x,
            coaches.freeport_ferry.y,
            coaches.freeport_ferry.z,
            coaches.freeport_ferry.facing
        )
    else
        npcDialogue = "Where would you like to go?"
        dialogueOptions = {"I would like passage bound for Freeport."}
        SendDialogue(mySession, npcDialogue, dialogueOptions)
    end
end
