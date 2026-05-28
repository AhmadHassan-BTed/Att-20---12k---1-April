function event_say()
diagOptions = {}
    npcDialogue = "There is a strange clockwork that keeps passing by the gate. Not sure who it belongs to. It must be malfunctioning. I'd go examine it myself but I of course can't leave my post. Besides, it doesn't seem to be harming anything."
SendDialogue(mySession, npcDialogue, diagOptions)
end