function event_say()
diagOptions = {}
    npcDialogue = "To the south is a cursed and dangerous desert, playerName. The desert of Ro. There you will find people that want to rob you, orcs that want to eat you, and undead that want to turn you. If you stay on the road, it will eventually fork. When it does, take the eastern path. It will lead you to Muniel's Tea Garden, a safe haven, in this deadly desert."
SendDialogue(mySession, npcDialogue, diagOptions)
end