function event_say()
diagOptions = {}
    npcDialogue = "playerName I've been sighting a strange wolf in the north east. It has been prowling nearer and nearer to the town. We will have to send someone to rid us of this dangerous beast."
SendDialogue(mySession, npcDialogue, diagOptions)
end