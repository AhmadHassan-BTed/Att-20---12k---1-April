function event_say()
diagOptions = {}
    npcDialogue = "I've had a long day extractin' nectar from cherrywood trees. I could part with some, but I'm afraid it'll cost ya a pretty penny. The nectar is a precious commodity in these parts o' the world."
SendDialogue(mySession, npcDialogue, diagOptions)
end