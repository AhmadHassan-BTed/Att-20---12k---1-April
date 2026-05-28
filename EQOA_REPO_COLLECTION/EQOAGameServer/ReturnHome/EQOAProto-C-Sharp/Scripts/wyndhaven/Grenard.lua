function event_say()
diagOptions = {}
    npcDialogue = "My scouts have reported a growing number of gnolls near Jethro's Lake to the east. I am concerned for the safety of the citizens of Wyndhaven. We will have to send over some footman to thin out their numbers."
SendDialogue(mySession, npcDialogue, diagOptions)
end