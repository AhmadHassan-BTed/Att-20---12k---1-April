function event_say()
diagOptions = {}
    npcDialogue = "I've just received word about a portal appearing south of Stonehaven village. A mob of goblins was seen pouring through, and invading the land. This will have to be delt with. Perhaps we could spare a paladin or two to seal the portal..."
SendDialogue(mySession, npcDialogue, diagOptions)
end