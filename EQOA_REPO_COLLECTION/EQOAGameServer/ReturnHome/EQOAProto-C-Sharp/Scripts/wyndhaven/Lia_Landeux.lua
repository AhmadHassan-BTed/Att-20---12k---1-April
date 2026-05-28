function event_say()
diagOptions = {}
    npcDialogue = "I arrived with the other barbarians to the south. I've grown tired of roughing it in the wild. I need a more comfortable home, somewhere I could call my own. It does get lonely sometimes. It's nice to talk to you, playerName."
SendDialogue(mySession, npcDialogue, diagOptions)
end