function event_say()
diagOptions = {}
    npcDialogue = "I was part of a regiment of dwarves that traveled far to the south. We founded a new mining operation there dubbed \"Gerntar Mines\". Mining has gone well this last several years, and recently we made an incredible discovery. We mined clear through the Rathe mountains and found ourselves on the other side. To our surprise was a strange land filled with ogres, cyclops and many other strange things. All was going well until the mine collapsed, with some of my brethren trapped on the far side of the mountain. I was sent to Moradhim to deliver the news to our elders. I am now awaiting a response from them, and I will surely return to the mines. I sure hope the lost dwarves are ok."
SendDialogue(mySession, npcDialogue, diagOptions)
end