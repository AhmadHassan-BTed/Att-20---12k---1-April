function event_say()
diagOptions = {}
    npcDialogue = "I am in search of our ancient records. There is a strange creature that clings to a book near the abandoned outpost to the south. That book may be a record of some kind. I'll need to send someone to acquire it."
SendDialogue(mySession, npcDialogue, diagOptions)
end