function event_say()
diagOptions = {}
    npcDialogue = "I've noticed some activity from deadly corrosions nearby. I may need to send someone out there to deal with them. Be ready for my summons, playerName."
SendDialogue(mySession, npcDialogue, diagOptions)
end