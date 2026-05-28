function event_say()
diagOptions = {}
    npcDialogue = "Sorry, but due to a rat infestation, our tavern is not accepting customers at this time."
SendDialogue(mySession, npcDialogue, diagOptions)
end