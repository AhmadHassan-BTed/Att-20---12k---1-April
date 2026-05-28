function event_say()
diagOptions = {}
    npcDialogue = "We breed centipedes here in The Pit to cultivate the potent toxin they make, which can be used to stun our enemies in battle.  We also feed the centipedes any traitors, weaklings or lizardmen that we find. Take care not to get bitten if you wander down there, playerName."
SendDialogue(mySession, npcDialogue, diagOptions)
end