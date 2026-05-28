function event_say()
diagOptions = {}
    npcDialogue = "We have many ships arriving with goods from all over. Sometimes, we have to watch out for stowaways and thieves who are looking for an opportunity to relieve folks of their tunar. Keep your eyes open. We can't catch them all."
SendDialogue(mySession, npcDialogue, diagOptions)
end