function event_say()
diagOptions = {}
    npcDialogue = "Welcome to the Academy of Arcane Science, playerName. Feel free to take a look around, but be warned that the masters here to do not tolerate any mischievous activity."
SendDialogue(mySession, npcDialogue, diagOptions)
end