function event_say()
diagOptions = {}
    npcDialogue = "I've just returned from a journey to the Gerntar Mines. The dwarves their had mined all the way through to the other side of the mountain, but alas, a cave-in has separated those on the other side from us. I hope they are okay. Hearty folk, those dwarves are."
SendDialogue(mySession, npcDialogue, diagOptions)
end