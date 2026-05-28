function event_say()
diagOptions = {}
    npcDialogue = "Hail, Adventurer!  I can see you have traveled a great distance to come here.  If your feet grow tired, you should stop in at the temple to rest."
SendDialogue(mySession, npcDialogue, diagOptions)
end