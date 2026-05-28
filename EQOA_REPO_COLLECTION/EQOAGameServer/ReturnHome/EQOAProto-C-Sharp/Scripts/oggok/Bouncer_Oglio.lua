function event_say()
diagOptions = {}
    npcDialogue = "Here, your path as a warrior is decided, playerName. Either Rallos Zek wants you alive, or he wants you squashed under the boot of someone more deserving."
SendDialogue(mySession, npcDialogue, diagOptions)
end