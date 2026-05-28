function event_say()
diagOptions = {}
    npcDialogue = "Occasionally, a wandering gnoll will attempt to slip into Qeynos through the back gates. If you hold perfectly still, they sometimes don't even notice you are there. One swift and true thrust from my pike ends their intrusion. The fun part is catapulting them out to the ocean as far as I can."
SendDialogue(mySession, npcDialogue, diagOptions)
end