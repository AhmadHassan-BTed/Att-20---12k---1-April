function event_say()
diagOptions = {}
    npcDialogue = "Lots of travelers make their way down the road, and eventually they stop here for a drink and some rest. I'd tell you what you want to know, but that would put a hamper on my customers coming back, now wouldn't it?"
SendDialogue(mySession, npcDialogue, diagOptions)
end