function event_say()
diagOptions = {}
    npcDialogue = "I traveled here from Freeport to visit my relatives in Sunnyvale just down the road to the west. I was shocked to find them all dead...but not dead. Oh playerName, what do I do??"
SendDialogue(mySession, npcDialogue, diagOptions)
end