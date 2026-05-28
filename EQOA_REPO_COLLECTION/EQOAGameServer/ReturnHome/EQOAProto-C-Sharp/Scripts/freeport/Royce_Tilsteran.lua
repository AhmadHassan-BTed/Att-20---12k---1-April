function event_say()
diagOptions = {}
    npcDialogue = "We've tried to be welcoming to all races of Tunaria. There are plenty of shady characters that pass through Freeport though, and staying out of trouble can be a challenge. A lot of the people that live here could be seen as despicable, but honestly I think most of them are just lost or misguided. Still, I have hope for this city."
SendDialogue(mySession, npcDialogue, diagOptions)
end