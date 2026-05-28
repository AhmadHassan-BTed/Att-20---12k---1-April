function event_say()
diagOptions = {}
    npcDialogue = "All this time I have remained faithful to my Lord Cazic Thule. In the chamber of the Eyes of Cazic, he rewards me with visions of the frogloks and their many hiding places. We must crush the froglok before it is too late. If I send you to destroy them, will you, playerName serve me as your lord and master? Will you fulfil your duty, even if it means your death?"
SendDialogue(mySession, npcDialogue, diagOptions)
end