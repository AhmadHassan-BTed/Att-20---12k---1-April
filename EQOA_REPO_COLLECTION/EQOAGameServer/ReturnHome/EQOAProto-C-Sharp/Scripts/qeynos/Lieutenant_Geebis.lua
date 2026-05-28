function event_say()
diagOptions = {}
    npcDialogue = "Follow the road east to Hagley Village, west to the city of Qeynos. Be mindful of the gnoll camps in the area."
SendDialogue(mySession, npcDialogue, diagOptions)
end