function event_say()
diagOptions = {}
    npcDialogue = "playerName, if you witness any villainy, be sure to report it immediately. We do not tolerate transgression."
SendDialogue(mySession, npcDialogue, diagOptions)
end