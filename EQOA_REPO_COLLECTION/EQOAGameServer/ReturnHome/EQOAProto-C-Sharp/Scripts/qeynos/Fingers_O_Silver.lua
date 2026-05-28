function event_say()
diagOptions = {}
    npcDialogue = "When does the next ship arrive? I've got to get out of this city before the guards catch me. I can't be caught now, I've almost tracked down the treasure. If you want to leave everything behind playerName, you can set sail with me. We'll commandeer the next ship, and go where our hearts take us. I could use a first mate like you. Let me know!"
SendDialogue(mySession, npcDialogue, diagOptions)
end