function event_say()
diagOptions = {}
    npcDialogue = "It is very rare, but in the occasional gnoll raid a single gnoll will slip through the tunnel. Those filthy gnolls will bring disease to our precious glade. We quickly show them point of our blade when arrows miss their target."
SendDialogue(mySession, npcDialogue, diagOptions)
end