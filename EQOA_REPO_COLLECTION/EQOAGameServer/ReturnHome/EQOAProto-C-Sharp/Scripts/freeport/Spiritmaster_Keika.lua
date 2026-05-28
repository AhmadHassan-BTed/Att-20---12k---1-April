local ch = tostring(choice)
local quests = require('Scripts/FreeportQuests')
function event_say()
    local diagOptions = {}
    local questText = ""
    local npcDialogue = ""
    --Necromancer Quest
    if (GetPlayerFlags(mySession, "11011") == "1")
    then
        if(ch:find("Farewell")) then
        diagOptions = {}
        npcDialogue = "Spiritmaster Keika: It is best that you not wonder off if your guildmaster has put you to a task. Guildmasters expect focus and punctuality."
        elseif(ch:find("Sorry")) then
        diagOptions = {}
        npcDialogue = ""
            multiDialogue = {"Spiritmaster Keika: Ahh, you must be a new member of the House Slaerin. All new recruits come to me for binding.",
            "Spiritmaster Keika: I am a Spiritmaster. I have been trained to bind a person's Soul to a certain location if they so wish it.",
            "Spiritmaster Keika: When you are slain, your spirit will return to where it is bound. There your body and possessions will rematerialize.",
            "Spiritmaster Keika: Only when the gods deem that your destiny has been fulfilled will you truly die. Until then you will always return.",
            "Spiritmaster Keika: As per Corious Slaerin's request, I will bind you as I bind all new necromancers.",
            "Spiritmaster Keika: Before I send you back to Corious Slaerin, you must speak with Coachman Ronks.",
            "Spiritmaster Keika: You can find him just west of here passed the docks..",
            "You have finished a quest!",
            "You have received a quest!"
            }
            SendMultiDialogue(mySession, multiDialogue)
            ContinueQuest(mySession, 11011, quests[11011][1].log)
        else
        npcDialogue = "Hello."
        diagOptions = { "Sorry to bother, but Corious Slaerin sent me.", "Farewell." }
        end
    --Cleric Quest
    elseif (GetPlayerFlags(mySession, "09011") == "1")
    then
        npcDialogue = "Hello."
        diagOptions = { "Sorry to bother, but Azlynn sent me.", "Farewell" }
        if(ch:find("Farewell")) then
        npcDialogue = "Spiritmaster Keika: It is best that you not wonder off if your guildmaster has put you to a task. Guildmasters expect focus and punctuality."
        elseif(ch:find("Sorry")) then
            multiDialogue = "Spiritmaster Keika: You were sent to me for binding, or rather, the binding of your spirit.",
            "Spiritmaster Keika: When a spirit is bound to a location, that spirit will return to the last location it was bound to if it's body is slain.",
            "Spiritmaster Keika: The body will rematerialize there along with all of it's equipment. I will bind your spirit to this location now.",
            "Spiritmaster Keika: Before I send you back to Azlynn you must speak with Coachman Ronks. He runs the stables to the south.",
            "Spiritmaster Keika: You can find him just passed the southern exit of Freeport. Head out the doorway southeast of here, then south along the midroad, then southwest to the stables."
            ContinueQuest(mySession, 09011, quests[][1].log)
        end
    else
        npcDialogue = "Would you like me to bind your spirit to this location, child?"
    end
    SendDialogue(mySession, npcDialogue, diagOptions)
end

