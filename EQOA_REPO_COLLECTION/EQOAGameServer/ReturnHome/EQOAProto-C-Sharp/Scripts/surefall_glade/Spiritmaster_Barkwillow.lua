function event_say()
diagOptions = {}
    npcDialogue = "Greetings playerName. I can bind you here if you wish."
SendDialogue(mySession, npcDialogue, diagOptions)
end