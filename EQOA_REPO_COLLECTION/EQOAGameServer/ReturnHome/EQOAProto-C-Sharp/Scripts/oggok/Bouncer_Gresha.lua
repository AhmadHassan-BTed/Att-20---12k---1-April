function event_say()
diagOptions = {}
    npcDialogue = "If you aren't an ogre, a troll, a gnome or one of those dark skinned elves, it will only be a moment before you lose your head if you enter our great city of Oggok."
SendDialogue(mySession, npcDialogue, diagOptions)
end