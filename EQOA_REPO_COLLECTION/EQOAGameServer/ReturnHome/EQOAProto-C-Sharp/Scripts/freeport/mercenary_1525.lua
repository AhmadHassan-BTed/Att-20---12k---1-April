function event_say()
diagOptions = {}
    npcDialogue = "Unless you have some nefarious activity to report, step aside playerName."
SendDialogue(mySession, npcDialogue, diagOptions)
end