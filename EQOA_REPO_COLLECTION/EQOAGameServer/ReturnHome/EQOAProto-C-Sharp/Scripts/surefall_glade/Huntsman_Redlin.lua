function event_say()
diagOptions = {}
    npcDialogue = "Bowhunting is more than just managing to hit your target. As your skill increases, you will discover that *where* you hit your target matters. Over time, you will manage critical hits, which can mean the difference between life and death when an enemy is closing in on you."
SendDialogue(mySession, npcDialogue, diagOptions)
end