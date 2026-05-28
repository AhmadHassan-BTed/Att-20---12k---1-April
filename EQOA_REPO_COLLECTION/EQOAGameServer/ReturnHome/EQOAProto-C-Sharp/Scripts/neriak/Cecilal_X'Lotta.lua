function event_say()
diagOptions = {}
    npcDialogue = "Your arrival bodes well for us, playerName. Have you come to sell your wares to the grocers above, or have you simply come to chat with yours truly?"
SendDialogue(mySession, npcDialogue, diagOptions)
end