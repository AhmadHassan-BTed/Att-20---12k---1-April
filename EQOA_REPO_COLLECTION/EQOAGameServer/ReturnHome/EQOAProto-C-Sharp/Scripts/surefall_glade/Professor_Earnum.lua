function event_say()
diagOptions = {}
    npcDialogue = "Hello playerName. It is safe here in Surefall Glade, but I have been gathering information about some very concerning behavior from wild animals outside the glade. I'll need some time to sort this out. Perhaps we could speak again later."
SendDialogue(mySession, npcDialogue, diagOptions)
end