function event_say()
diagOptions = {}
    npcDialogue = "We've spotted a nasty orc in that old dilapidated house in the north. It's quite concerning to see it this close to the city. Someone is going to need clear it out before more orcs show up."
SendDialogue(mySession, npcDialogue, diagOptions)
end