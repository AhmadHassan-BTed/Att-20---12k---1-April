function event_say()
diagOptions = {}
    npcDialogue = "I traveled to the Bogman village on the coast to the north a few weeks ago. Just outside the village is a floating temple. There is a dark, penetrating magic coming from that place. Something ancient. It smells of death and despair. Beware that place, playerName."
SendDialogue(mySession, npcDialogue, diagOptions)
end