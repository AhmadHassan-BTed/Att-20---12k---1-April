function event_say()
diagOptions = {}
    npcDialogue = "I hope they don't find me here. I was supposed to be hung to death last month. I don't want to die, but more than that, I don't want to become one of the mindless skeletons that haunt Hangman's Hill to the south. Please, don't tell them I am here. I am just looking for a little food, and I'll be on my way."
SendDialogue(mySession, npcDialogue, diagOptions)
end