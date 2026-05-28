function event_say()
diagOptions = {}
    npcDialogue = "That is a very nice weapon you have there. So, I know someone who could enchant it, and make it THREE times as powerful, for only a very small fee of two tunar. Just hand me your weapon, and I'll go take care of it, and come right back! I promise. What do you say playerName, do we have a deal?"
SendDialogue(mySession, npcDialogue, diagOptions)
end