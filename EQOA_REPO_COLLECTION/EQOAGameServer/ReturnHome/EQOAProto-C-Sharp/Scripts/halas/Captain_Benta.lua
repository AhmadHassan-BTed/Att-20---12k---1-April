function event_say()
diagOptions = {}
    npcDialogue = "If you're here about the bracelet, Sools, at Castle Lightwolf, has my report. If not, well that is classified information and you should forget we ever spoke, playerName, if you know what's good for you. They're always watching."
SendDialogue(mySession, npcDialogue, diagOptions)
end