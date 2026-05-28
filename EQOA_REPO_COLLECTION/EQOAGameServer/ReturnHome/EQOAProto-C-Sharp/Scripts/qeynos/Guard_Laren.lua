function event_say()
diagOptions = {}
    npcDialogue = "We are here to prevent any cheating of course... And yes, some people still try to get away with it in front of us. The penalty for cheating in an official match is 1 day of confinement at Qeynos Prison. That should be enough to deter even you, playerName. *Guard Laren looks at you suspiciously, pauses for a moment, then lets out a lighthearted chuckle.*"
SendDialogue(mySession, npcDialogue, diagOptions)
end