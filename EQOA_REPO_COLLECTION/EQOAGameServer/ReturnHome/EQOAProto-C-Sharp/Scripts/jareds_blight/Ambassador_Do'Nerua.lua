function event_say()
diagOptions = {}
    npcDialogue = "You'll have to forgive me, I cannot be distracted right now. I am quite certain someone is trying to murder me. I got into quite the political disagreement with Ambassador Kizden G'Lux, and he doesn't take being challenged by others well. Hopefully, I won't be found here at Darvar Manor. playerName, you must forget that you've seen me here."
SendDialogue(mySession, npcDialogue, diagOptions)
end