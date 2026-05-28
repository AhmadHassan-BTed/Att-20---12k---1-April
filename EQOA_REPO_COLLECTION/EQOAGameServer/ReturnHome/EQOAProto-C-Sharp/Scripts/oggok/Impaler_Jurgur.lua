function event_say()
diagOptions = {}
    npcDialogue = "A band of those greedy dwarves dug their way through the mountain across the lake to the northeast. The tunnels collapsed, but it hasn't stopped them from quickly building a mining operation on this side of the mountain. I hate dwarves. Don't you, playerName? They drove us out of our home long ago on the continent of Faydewer. Now they threaten our home once again!"
SendDialogue(mySession, npcDialogue, diagOptions)
end