local ch = tostring(choice)
function event_say()
    local diagOptions = {}
    local questText = ""
    local npcDialogue = ""
    if (class == "Magician" and race == "Human" and humanType == "Freeport" and
    GetPlayerFlags(mySession, "EasternMagician3") and not GetPlayerFlags(mySession, "EasternMagician4"))
    then
        npcDialogue = "Hello."
        diagOptions = { "Sorry to bother, but Malsis sent me.", "Good day!" }
        if(ch:find("Good")) then
        diagOptions = {}
        npcDialogue = "Spiritmaster Alshan: It is best that you not wonder off if your guildmaster has put you to a task. Guildmasters expect focus and punctuality."
        elseif(ch:find("Sorry")) then
            diagOptions = {}
            npcDialogue = "Spiritmaster Alshan: You were sent to me for binding, or rather, the binding of your spirit."
            SendDialogue(mySession, npcDialogue, diagOptions)
            npcDialogue = "Spiritmaster Alshan: When a spirit is bound to a location, that spirit will return to the last location it was bound to if it's body is slain."
            SendDialogue(mySession, npcDialogue, diagOptions)
            npcDialogue = "Spiritmaster Alshan: The body will rematerialize there along with all of it's equipment. I will bind your spirit to this location now."
            SendDialogue(mySession, npcDialogue, diagOptions)
            npcDialogue = "Spiritmaster Alshan: Before I send you back to Malsis you must speak with Coachman Ronks. He runs the stables to the south."
            SendDialogue(mySession, npcDialogue, diagOptions)
            npcDialogue = "Spiritmaster Alshan: You can find him just passed the southern exit of Freeport. Head out the doorway southeast of here, then south along the midroad, then southwest to the stables."
            questText = "Go speak to Coachman Ronks at the Stable."
            AddQuestLog(mySession, 0, questText)
            SetPlayerFlags(mySession, "EasternMagician4", true)
        end
    else
        npcDialogue = "Would you like me to bind your spirit to this location, child?"
    end
    SendDialogue(mySession, npcDialogue, diagOptions)
end

