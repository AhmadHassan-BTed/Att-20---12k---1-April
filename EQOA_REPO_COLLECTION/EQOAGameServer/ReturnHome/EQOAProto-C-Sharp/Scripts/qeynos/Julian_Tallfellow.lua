function event_say()
diagOptions = {}
    npcDialogue = "We are looking for a thief who stole a gear from the clocktower. The thief had brown hair, wearing padded armor and carrying a staff. Witnesses say he traveled east, and had a dog with him. He called out the dogs name as \"Lemet\" when he left the front gate. Let us know if you see anything, playerName."
SendDialogue(mySession, npcDialogue, diagOptions)
end