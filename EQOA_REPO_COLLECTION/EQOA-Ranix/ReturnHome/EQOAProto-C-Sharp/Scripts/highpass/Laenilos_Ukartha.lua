function event_say()
diagOptions = {}
    npcDialogue = "I've traveled all the way from the city of Highbourne to be able to offer my knowledge in the arcane arts from the heart of Norrath.  I have met many strange faces along my travels but something is quite different about you.  Yes, something great lingers within you.  Power slumbering deep within you that most would not notice.  I have a feeling you've seen your fair share of combat and will only continue to get stronger as time passes.  Stay safe, wanderer, and may our paths collide again."
SendDialogue(mySession, npcDialogue, diagOptions)
end