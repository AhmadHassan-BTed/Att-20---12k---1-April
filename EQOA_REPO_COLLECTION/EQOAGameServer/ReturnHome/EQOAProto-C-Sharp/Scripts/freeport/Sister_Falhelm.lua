function event_say()
diagOptions = {}
    npcDialogue = "To survive as a cleric here you must endure many painful trials. Don't think that just because you have a healing spell means that getting wounded in battle isn't still agonizingly painful."
SendDialogue(mySession, npcDialogue, diagOptions)
end