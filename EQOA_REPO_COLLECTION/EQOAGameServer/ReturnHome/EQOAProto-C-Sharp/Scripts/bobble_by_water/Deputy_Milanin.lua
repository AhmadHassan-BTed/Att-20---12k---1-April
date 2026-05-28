function event_say()
diagOptions = {}
    npcDialogue = "Mind yer path if yer headin' south. Orcs live in these forests. Best you stay close to the river if yer tryin' to reach Freeport. Directly south from here is the village of Hodstock, but they are not overly fond of strangers. Just to the east of there is the tower of Noctix, where several of our kin have wandered off to and never returned."
SendDialogue(mySession, npcDialogue, diagOptions)
end