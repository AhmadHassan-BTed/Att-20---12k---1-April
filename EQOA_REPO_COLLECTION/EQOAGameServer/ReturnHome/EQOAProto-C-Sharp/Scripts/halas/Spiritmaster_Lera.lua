function event_say()
diagOptions = {}
    npcDialogue = "Would you like me to bind your spirit to this location, playerName? When you die, I can recall your spirit to this place and you will be born anew."
SendDialogue(mySession, npcDialogue, diagOptions)
end