function event_say()
diagOptions = {}
    npcDialogue = "I am working on a spell that can teleport you to Neriak and Freeport. Where would you like to go, playerName?"
SendDialogue(mySession, npcDialogue, diagOptions)
end