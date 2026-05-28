function event_say()
diagOptions = {}
    npcDialogue = "The deathfists still roam these dunes.  I advise you to watch yourself out there.  If you get surrounded, run back to the city and we'll handle any who were foolish enough to follow you back."
SendDialogue(mySession, npcDialogue, diagOptions)
end