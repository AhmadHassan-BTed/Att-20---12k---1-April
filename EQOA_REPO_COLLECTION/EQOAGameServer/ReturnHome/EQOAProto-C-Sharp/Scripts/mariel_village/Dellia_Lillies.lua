function event_say()
diagOptions = {}
    npcDialogue = "I am on a sharp lookout for bandit activity. A few of them have wandered this way, and they were met with a swift arrow in their chest. Eventually, the other bandits may come looking for them."
SendDialogue(mySession, npcDialogue, diagOptions)
end