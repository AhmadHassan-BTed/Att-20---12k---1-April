function event_say()
diagOptions = {}
    npcDialogue = "I am keeping an eye out for were beasts. That is my business. I am currently tracking a vampire that may be summoning the undead. playerName if you see anything like this, let me know."
SendDialogue(mySession, npcDialogue, diagOptions)
end