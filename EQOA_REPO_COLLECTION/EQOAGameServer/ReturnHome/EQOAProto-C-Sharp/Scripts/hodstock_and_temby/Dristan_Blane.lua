function event_say()
diagOptions = {}
    npcDialogue = "My clients expect swift results, and they pay me well for it. Occasionally something goes wrong, but I always solve it before my reputation takes a hit. You don't need anyone…snuffed out…do you playerName?"
SendDialogue(mySession, npcDialogue, diagOptions)
end