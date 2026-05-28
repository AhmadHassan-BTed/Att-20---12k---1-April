function event_say()
diagOptions = {}
    npcDialogue = "Just because we dwarves prefer the comfort of an underground city doesn't mean we can't handle ourselves in the brutal elements. Why, just a few weeks ago I returned from long journey in the frozen north. The worst part is tryin' ta drink from yer growler of ale. The ale becomes quite slushy, lumpy and distasteful. It's best to bring a small barrel of red wine to the north instead."
SendDialogue(mySession, npcDialogue, diagOptions)
end