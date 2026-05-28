function event_say()
diagOptions = {}
    npcDialogue = "The river comes from up north, where dark elves, undead, and who knows what else could have laced it with any manner of disease. Be sure not to drink from the River Saren directly, playerName. The water should be purified by a druid first, such as from this well. "
SendDialogue(mySession, npcDialogue, diagOptions)
end