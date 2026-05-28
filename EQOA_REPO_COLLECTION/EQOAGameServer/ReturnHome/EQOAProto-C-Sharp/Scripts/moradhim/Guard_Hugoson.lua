function event_say()
diagOptions = {}
    npcDialogue = "We dwarves don't just stay in one spot. We tend ta spread out and look for new opportunities. One of my friends named Kahzum moved ta the hidden valley village o' Chiasta ta the east. I visit him from time to time when I need a new axe or a reinforced shield."
SendDialogue(mySession, npcDialogue, diagOptions)
end