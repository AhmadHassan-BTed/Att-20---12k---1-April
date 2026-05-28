function event_say()
diagOptions = {}
    npcDialogue = "If you knew what I've done playerName, perhaps you wouldn't have come up here. If you knew who I've killed, perhaps you wouldn't even speak to me. However if you were to listen, then I would tell you that self determination, at whatever the cost may be, is your path to true freedom, and true power. Don't let the pious, the righteous, and the old dogmatic beliefs take from you that which is your birthright. You have only to reach out and take what is yours. However, you must choose it."
SendDialogue(mySession, npcDialogue, diagOptions)
end