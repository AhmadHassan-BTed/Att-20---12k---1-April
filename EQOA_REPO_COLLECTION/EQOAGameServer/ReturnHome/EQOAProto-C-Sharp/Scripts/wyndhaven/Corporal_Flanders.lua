function event_say()
diagOptions = {}
    npcDialogue = "Here at The Lost Watch we protect the city state of Qeynos from the wilds in the north. A long mountain range extends far to the east from here, keeping our lands separate, however here where the mountains open up is where all manor of unusual things, man and beast may wander south."
SendDialogue(mySession, npcDialogue, diagOptions)
end