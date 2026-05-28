function event_say()
diagOptions = {}
    npcDialogue = "Some bandits that have taken over the ruins in the hills to the east have been sneaking down here to swipe the merchants supplies. We have to keep a sharp eye out, playerName."
SendDialogue(mySession, npcDialogue, diagOptions)
end