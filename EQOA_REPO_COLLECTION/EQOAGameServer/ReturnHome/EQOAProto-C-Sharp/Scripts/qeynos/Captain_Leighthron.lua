function event_say()
diagOptions = {}
    npcDialogue = "The situation at Qeynos Prison has gotten out of hand. The prisoners have escaped the dungeon cells, and have taken over the prison. Apparently the key to the prison is missing, and the guards have fled. I'm currently arranging a plan to send reinforcements."
SendDialogue(mySession, npcDialogue, diagOptions)
end