function event_say()
diagOptions = {}
    npcDialogue = "I'm always looking for better materials to make armor with. In a forest to the southwest, I witnessed an ancient stone golem travel by. I didn't dare go near it, but I am curious if it's properties could be combined with traditional materials for an improved piece of armor."
SendDialogue(mySession, npcDialogue, diagOptions)
end