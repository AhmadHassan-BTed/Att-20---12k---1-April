function event_say()
diagOptions = {}
    npcDialogue = "Hello playerName, I am the Anagogical Order's expert seamstress. I provide our apprentices with the proper robes of their profession, for a small fee of course. Let me know if you need a robe mended."
SendDialogue(mySession, npcDialogue, diagOptions)
end