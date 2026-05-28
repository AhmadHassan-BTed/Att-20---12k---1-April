function event_say()
diagOptions = {}
    npcDialogue = "Sometimes, I sense someone passing by on their way into Surefall Glade as though they were invisible. I have been known to pierce an enemy that is covered in a veil of invisibility with my arrow, not with any magic, but purely by sound, and vibrations in the ground. I can only imagine the shock on their faces, but unfortunately they are already dead when their invisibility wears off so I don't get to see it."
SendDialogue(mySession, npcDialogue, diagOptions)
end