local ch = tostring(choice)
local quests = require('Scripts/FreeportQuests')
function event_say()
    local diagOptions = {}
    local questText = ""
    local npcDialogue = ""
    if (GetPlayerFlags(mySession, "10011") == "1")
    then
        if(ch:find("Good")) then
        diagOptions = {}
        npcDialogue = "Spiritmaster Alshan: It is best that you not wonder off if your guildmaster has put you to a task. Guildmasters expect focus and punctuality."
        elseif(ch:find("Sorry")) then
        diagOptions = {}
        npcDialogue = ""
            multiDialogue = {"Spiritmaster Alshan: You were sent to me for binding, or rather, the binding of your spirit.",
            "Spiritmaster Alshan: When a spirit is bound to a location, that spirit will return to the last location it was bound to if it's body is slain.",
            "Spiritmaster Alshan: The body will rematerialize there along with all of it's equipment. I will bind your spirit to this location now.",
            "Spiritmaster Alshan: Before I send you back to Malsis you must speak with Coachman Ronks. He runs the stables to the south.",
            "Spiritmaster Alshan: You can find him just passed the southern exit of Freeport. Head out the doorway southeast of here, then south along the midroad, then southwest to the stables."
            }
            SendMultiDialogue(mySession, multiDialogue)
            ContinueQuest(mySession, 10011, quests[10011][1].log)
        else
        npcDialogue = "Hello."
        diagOptions = { "Sorry to bother, but Malsis sent me.", "Good day!" }
        end
    elseif (GetPlayerFlags(mySession, "12011") == "1")
    then
        npcDialogue = "Hello."
        diagOptions = { "Sorry to bother, but Azlynn sent me.", "Farewell" }
        if(ch:find("Farewell")) then
        npcDialogue = "Spiritmaster Alshan: It is best that you not wonder off if your guildmaster has put you to a task. Guildmasters expect focus and punctuality."
        elseif(ch:find("Sorry")) then
            multiDialogue = "Spiritmaster Alshan: You were sent to me for binding, or rather, the binding of your spirit.",
            "Spiritmaster Alshan: When a spirit is bound to a location, that spirit will return to the last location it was bound to if it's body is slain.",
            "Spiritmaster Alshan: The body will rematerialize there along with all of it's equipment. I will bind your spirit to this location now.",
            "Spiritmaster Alshan: Before I send you back to Azlynn you must speak with Coachman Ronks. He runs the stables to the south.",
            "Spiritmaster Alshan: You can find him just passed the southern exit of Freeport. Head out the doorway southeast of here, then south along the midroad, then southwest to the stables."
            ContinueQuest(mySession, 10011, quests[12011][1].log)
        end
    else
        npcDialogue = "Would you like me to bind your spirit to this location, child?"
    end
    SendDialogue(mySession, npcDialogue, diagOptions)
end

