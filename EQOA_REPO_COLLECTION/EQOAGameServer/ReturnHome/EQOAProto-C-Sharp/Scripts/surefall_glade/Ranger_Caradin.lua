function event_say()
diagOptions = {}
    npcDialogue = "While we are keeping the glade protected, we also send supplies to Wymondham to the east. We don't dare cut down any of the trees here in the glade though. We search out fallen trees in the world as our source of wood for bows and arrows."
SendDialogue(mySession, npcDialogue, diagOptions)
end