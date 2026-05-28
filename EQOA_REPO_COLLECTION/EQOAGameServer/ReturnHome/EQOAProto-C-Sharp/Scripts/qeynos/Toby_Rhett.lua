function event_say()
diagOptions = {}
    npcDialogue = "There is quite the stench coming from those careless alchemists next door! Who let them move in here? What kind of dangerous concoctions are they producing in there? I shall file a complaint with King Bayle!"
SendDialogue(mySession, npcDialogue, diagOptions)
end