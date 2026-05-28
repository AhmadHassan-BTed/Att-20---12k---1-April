function event_say()
diagOptions = {}
    npcDialogue = "If you take the road out of the hold heading east you will eventually hit the harbor town of Freeport.  That place is touched by darkness, I hear.  Keep your wits about you."
SendDialogue(mySession, npcDialogue, diagOptions)
end