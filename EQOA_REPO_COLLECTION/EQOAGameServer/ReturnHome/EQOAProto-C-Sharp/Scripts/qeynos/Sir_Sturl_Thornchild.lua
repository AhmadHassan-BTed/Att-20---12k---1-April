function event_say()
diagOptions = {}
    npcDialogue = "Though we may not provide healing as our fellow clerics do, paladins are able to channel a holy skill known as \"Lay Hands\". It is a powerful act of healing, that is to be used in only the most dire of moments."
SendDialogue(mySession, npcDialogue, diagOptions)
end