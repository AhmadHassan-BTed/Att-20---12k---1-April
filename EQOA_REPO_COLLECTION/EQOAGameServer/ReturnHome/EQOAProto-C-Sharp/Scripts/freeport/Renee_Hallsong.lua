function event_say()
diagOptions = {}
    npcDialogue = "Bandits have been raiding caravans into the Freeport. Their thefts of imported goods include food and supplies for the poor. As bards, we must do what we can to help the city."
SendDialogue(mySession, npcDialogue, diagOptions)
end