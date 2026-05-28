function event_say()
diagOptions = {}
    npcDialogue = "I've spotted a thriving troll village in the swamps to the south. Trolls are vile, deadly creatures. They may be dumb, but I believe they have the strength to knock you out with one blow of their spiked clubs. Be safe, playerName."
SendDialogue(mySession, npcDialogue, diagOptions)
end