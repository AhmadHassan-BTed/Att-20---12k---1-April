function event_say()
diagOptions = {}
    npcDialogue = "We've just received word from the barbarian village of Diren to the northwest. They've suffered a terrible attack from the Frosteye Orcs, and now a strange plague inflicts the villagers."
SendDialogue(mySession, npcDialogue, diagOptions)
end