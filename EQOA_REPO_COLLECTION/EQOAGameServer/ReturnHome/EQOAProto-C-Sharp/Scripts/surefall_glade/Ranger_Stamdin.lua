function event_say()
diagOptions = {}
    npcDialogue = "The Shamblers in this area have been becoming a nuisance as of late. We need to clear them out before they multiply too quickly."
SendDialogue(mySession, npcDialogue, diagOptions)
end