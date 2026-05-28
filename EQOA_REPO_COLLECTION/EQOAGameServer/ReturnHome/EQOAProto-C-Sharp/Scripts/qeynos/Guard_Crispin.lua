function event_say()
diagOptions = {}
    npcDialogue = "Qeynos prides itself as a city of law. Violators sentenced to prison will be rewarded with a free ride to Qeynos Prison to the south, where all manor of foul and distasteful lowlifes serve out their glorious sentences. It's locked up tightly, no one ever escapes. Consider that if you are looking to cause any trouble, playerName. Now, please do have a nice day..."
SendDialogue(mySession, npcDialogue, diagOptions)
end