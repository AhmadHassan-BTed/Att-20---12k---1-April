function event_say()
diagOptions = {}
    npcDialogue = "Every warrior must train with a basic war axe before we let them out on their own. Without the proper trainin', their liable to accidentally cut off a limb!"
SendDialogue(mySession, npcDialogue, diagOptions)
end