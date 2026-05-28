function event_say()
diagOptions = {}
    npcDialogue = "The boss will be here shortly. I am certain you don't want to be here when he arrives. It's best you don’t mention what you've seen here to anyone."
SendDialogue(mySession, npcDialogue, diagOptions)
end