function event_say()
diagOptions = {}
    npcDialogue = "Please get me out of here, playerName. I thought this was a normal school for enchanters, but everyone here seems just a little bit ...deranged. When I try to leave they all start acting strange and don't let me reach the door. Please. Help me."
SendDialogue(mySession, npcDialogue, diagOptions)
end