function event_say()
diagOptions = {}
    npcDialogue = "For many years I kept my post at Forkwatch. Now, I wish to try a life at sea. Cool crisp air, new lands to discover...strange creatures...sunken treasure. I wonder if there are any folks here in Qeynos that would join me on my adventure. We may have to commandeer a ship first, I think. OH! Excuse, me, don't mind the ramblings of an old drunk."
SendDialogue(mySession, npcDialogue, diagOptions)
end