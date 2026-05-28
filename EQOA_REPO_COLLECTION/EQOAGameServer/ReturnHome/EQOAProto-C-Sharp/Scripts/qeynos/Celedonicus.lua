function event_say()
diagOptions = {}
    npcDialogue = "You should hear the songs of the elven bards, it is quite harmonious and melodic. They have a way of transporting your heart far away with the sweetest of notes...sweet, yet sad. Those poor elves, they were driven out of their old home, Takish-Hiz which used to be in the great Elddar Forest. Then Solusek Ro summoned the sun to dry up the forest with relentless heat, and the elves were forced to flee. playerName, you should visit Tethelin someday. The elves there may teach you of their mastery of survival over tragedy."
SendDialogue(mySession, npcDialogue, diagOptions)
end