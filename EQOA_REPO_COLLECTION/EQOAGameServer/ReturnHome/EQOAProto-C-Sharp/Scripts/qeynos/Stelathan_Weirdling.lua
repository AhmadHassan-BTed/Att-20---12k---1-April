function event_say()
diagOptions = {}
    npcDialogue = "The Darkpaw Gnolls are wrecking havoc on our supplies. Their camps continue to appear in the fields to the northeast. We will likely need to take care of it ourselves as the city guard is slow to respond to such pests."
SendDialogue(mySession, npcDialogue, diagOptions)
end