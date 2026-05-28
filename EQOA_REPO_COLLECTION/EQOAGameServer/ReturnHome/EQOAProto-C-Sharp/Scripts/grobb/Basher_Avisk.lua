function event_say()
diagOptions = {}
    npcDialogue = "My friend from Basher Enclave spoke of lizardman camp nearby. He says they stay put during da day and sneak into village at night. playerName, we will need to send help to eradicate them."
SendDialogue(mySession, npcDialogue, diagOptions)
end