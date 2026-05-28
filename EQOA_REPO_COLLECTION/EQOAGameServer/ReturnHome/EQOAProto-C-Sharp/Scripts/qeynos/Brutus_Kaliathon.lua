function event_say()
diagOptions = {}
    npcDialogue = "The gnolls in the area have been increasing their attacks on caravans and citizens. They have a new leader named Welsish Darkpaw. He is ruthless, cunning, and it's going to be difficult to travel outside the city unless someone puts a stop to him!"
SendDialogue(mySession, npcDialogue, diagOptions)
end