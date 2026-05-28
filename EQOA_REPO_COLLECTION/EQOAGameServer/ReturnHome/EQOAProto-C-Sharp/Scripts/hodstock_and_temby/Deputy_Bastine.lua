function event_say()
diagOptions = {}
    npcDialogue = "This town is virtually surrounded by orcs. Even scarier than that, is that ominously evil looking structure to the west. A few of our townsfolk went off to go see what it was, and we never saw them again. Somedays I wonder if Temby is hiring…"
SendDialogue(mySession, npcDialogue, diagOptions)
end