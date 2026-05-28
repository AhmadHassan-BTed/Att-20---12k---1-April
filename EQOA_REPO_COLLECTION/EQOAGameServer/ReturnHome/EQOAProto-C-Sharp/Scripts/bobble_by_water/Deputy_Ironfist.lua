function event_say()
diagOptions = {}
    npcDialogue = "You can see that just to the north of our town is the Saren Volcano. Just beyond that is the land of Neriak, home of the dark elves. They live underground, in the darkness. They practice dark things, and worship dark gods. They spread hate and fear. I pray you never have business that way, playerName."
SendDialogue(mySession, npcDialogue, diagOptions)
end