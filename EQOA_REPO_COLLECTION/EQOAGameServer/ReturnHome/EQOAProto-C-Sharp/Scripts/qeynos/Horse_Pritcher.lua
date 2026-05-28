function event_say()
diagOptions = {}
    npcDialogue = "I really must get this feed to the stables. So many travelers coming and going these days. Coachman Billfer will have my hide if I'm late! Hopefully he won't find out about the ale or two that I stopped by the inn for. Oh! And I stopped by the Beggars District to pay off a few debts. I also needed to say a prayer at the Church of Marr... Oh yes, I guess I did catch a fish at the Docks, and caught up with my 'ol buddy Fisherman Trails. I guess I've been a bit sidetracked today..."
SendDialogue(mySession, npcDialogue, diagOptions)
end