function event_say()
diagOptions = {}
    npcDialogue = "There is a nice lake just over the mountain to the west. At first, it looks like a nice spot to fish. When you see what's on the other side of the lake you'll think twice about eating any fish from it."
SendDialogue(mySession, npcDialogue, diagOptions)
end