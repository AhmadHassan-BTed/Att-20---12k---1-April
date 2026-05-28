function event_say()
diagOptions = {}
    npcDialogue = "If you witness any villainy playerName, be sure to report it immediately. We do not tolerate transgression."
SendDialogue(mySession, npcDialogue, diagOptions)
end