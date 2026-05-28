function event_say()
diagOptions = {}
    npcDialogue = "Going through this tunnel will lead you to Misty Thicket. The Inn and Coach are there but keep your eyes peeled for the aggressive wildlife."
SendDialogue(mySession, npcDialogue, diagOptions)
end