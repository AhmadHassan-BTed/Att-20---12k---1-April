function event_say()
diagOptions = {}
    npcDialogue = "This tunnel will lead you out of Rivervale and into the wilds of the world. The deputies of Rivervale will no longer be able to protect you once you depart."
SendDialogue(mySession, npcDialogue, diagOptions)
end