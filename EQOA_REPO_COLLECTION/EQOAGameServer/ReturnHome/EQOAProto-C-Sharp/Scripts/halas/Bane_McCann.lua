function event_say()
diagOptions = {}
    npcDialogue = "Not lost are you? To the northwest, you'll find Coachman Dudley. Head south, through the tunnel, and you'll find your way out of the city."
SendDialogue(mySession, npcDialogue, diagOptions)
end