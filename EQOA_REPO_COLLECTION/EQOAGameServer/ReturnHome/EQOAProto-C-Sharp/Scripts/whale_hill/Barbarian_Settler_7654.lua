function event_say()
diagOptions = {}
    npcDialogue = "If you come to a swamp just on the other side of Whale Hill to the southeast, steer clear of it. There are living oozes in the water that are not at all natural, and may dissolve your skin on contact."
SendDialogue(mySession, npcDialogue, diagOptions)
end