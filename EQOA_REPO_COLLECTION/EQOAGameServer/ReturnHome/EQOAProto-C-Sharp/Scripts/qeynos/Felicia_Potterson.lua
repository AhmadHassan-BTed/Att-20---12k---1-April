function event_say()
diagOptions = {}
    npcDialogue = "We're here to make sure Qeynos is clear of those cursed were-beasts. Let us know if you see anything, playerName."
SendDialogue(mySession, npcDialogue, diagOptions)
end