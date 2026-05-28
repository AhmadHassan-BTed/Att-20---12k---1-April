function event_say()
diagOptions = {}
    npcDialogue = "I've had to close down shop for the time being. Too many of my shipments have been pillaged by gnolls and goblins while en route. Why don't you make yourself useful, playerName, and go dispatch some of those damned creatures."
SendDialogue(mySession, npcDialogue, diagOptions)
end