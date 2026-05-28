function event_say()
diagOptions = {}
    npcDialogue = "I hate rats. I've been down there in the quarry many times to clear them out, but they multiply faster than barrels of ale at a dwarven wedding."
SendDialogue(mySession, npcDialogue, diagOptions)
end