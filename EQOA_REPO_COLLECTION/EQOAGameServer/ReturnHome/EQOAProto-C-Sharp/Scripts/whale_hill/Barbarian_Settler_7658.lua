function event_say()
diagOptions = {}
    npcDialogue = "Careful of the druid ring near the beach to the south, it has been cursed by a foul magic. The Cadaver there may fool you with their small size, but possess deadly disease and formidable strength."
SendDialogue(mySession, npcDialogue, diagOptions)
end