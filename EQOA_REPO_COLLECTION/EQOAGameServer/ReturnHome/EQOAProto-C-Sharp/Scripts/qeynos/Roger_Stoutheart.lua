function event_say()
diagOptions = {}
    npcDialogue = "This is the Qeynos Guardhouse, where we train all the warriors and guards to serve and protect the city. Protecting Qeynos from the darker elements of Tunaria isn't an easy task. It's not too late for you to join us, playerName...we could use every sword arm available."
SendDialogue(mySession, npcDialogue, diagOptions)
end