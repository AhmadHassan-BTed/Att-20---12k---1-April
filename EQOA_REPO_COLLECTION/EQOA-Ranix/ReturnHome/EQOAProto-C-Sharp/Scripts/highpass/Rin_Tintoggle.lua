function event_say()
diagOptions = {}
    npcDialogue = "Ahh, yes.  I'd love to sit and chat with you but as you can see, my wagon is broken and I'm waiting on a wheel to arrive so I can fix it.  Apologies but I must keep an eye out for my delivery."
SendDialogue(mySession, npcDialogue, diagOptions)
end