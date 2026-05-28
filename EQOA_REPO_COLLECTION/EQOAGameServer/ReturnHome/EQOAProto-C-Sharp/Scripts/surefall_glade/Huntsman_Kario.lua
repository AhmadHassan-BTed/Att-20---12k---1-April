function event_say()
diagOptions = {}
    npcDialogue = "Here in the archery range we practice our technique by measuring accuracy. In the wild however, it is your wit that is tested. Living, breathing, moving targets require stealth, anticipation and a degree of nimbleness. You must keep absolute presence as well as a steady hand if your aim is to be true."
SendDialogue(mySession, npcDialogue, diagOptions)
end