function event_say()
diagOptions = {}
    npcDialogue = "My partner Ling and I traveled here from the farms north of Qeynos. Ling says someone here owes us a lot of money. She's the accountant in our group, I'm just a mule hand."
SendDialogue(mySession, npcDialogue, diagOptions)
end