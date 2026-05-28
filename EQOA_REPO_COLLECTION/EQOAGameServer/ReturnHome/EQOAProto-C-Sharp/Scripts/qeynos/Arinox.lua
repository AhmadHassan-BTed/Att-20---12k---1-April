function event_say()
diagOptions = {}
    npcDialogue = "Master Uzara has left for Hagley Village. Something in the local tomb has become a point of concern. I am tending to his duties while he is out. Perhaps you could check back later if you need to speak to him, playerName."
SendDialogue(mySession, npcDialogue, diagOptions)
end