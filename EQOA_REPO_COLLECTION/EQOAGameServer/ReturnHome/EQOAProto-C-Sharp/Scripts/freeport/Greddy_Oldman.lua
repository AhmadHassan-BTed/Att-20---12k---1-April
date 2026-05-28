function event_say()
diagOptions = {}
    npcDialogue = "Those filthy creatures have been released into Tunaria to spread their disease. They are everywhere, and never stay in one place too long, so you ought to be careful. You never know which one of them is going to infect you!"
SendDialogue(mySession, npcDialogue, diagOptions)
end