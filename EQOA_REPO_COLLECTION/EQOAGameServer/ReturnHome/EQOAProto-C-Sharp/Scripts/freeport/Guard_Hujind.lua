function event_say()
diagOptions = {}
    npcDialogue = "Often times, we get travelers who want to rob the place. The commander stationed me here to keep an eye on the place. Hey, if you spot any funny business playerName, let me know immediately!"
SendDialogue(mySession, npcDialogue, diagOptions)
end