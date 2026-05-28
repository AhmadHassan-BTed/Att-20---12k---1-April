function event_say()
diagOptions = {}
    npcDialogue = "The wilds can destroy your corporeal form but never your spirit. Would you like me to bind your spirit to this location, playerName?"
SendDialogue(mySession, npcDialogue, diagOptions)
end