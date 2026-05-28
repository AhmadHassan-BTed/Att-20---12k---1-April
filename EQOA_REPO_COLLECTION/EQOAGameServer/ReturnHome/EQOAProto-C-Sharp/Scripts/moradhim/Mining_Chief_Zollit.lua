function event_say()
diagOptions = {}
    npcDialogue = "Now, ya can't just go mining precious ore with any old pickaxe. I keep a few forged steel pickaxes around for just such an occasion. I know, they look terrible, but I assure you they are 100 percent reliable."
SendDialogue(mySession, npcDialogue, diagOptions)
end