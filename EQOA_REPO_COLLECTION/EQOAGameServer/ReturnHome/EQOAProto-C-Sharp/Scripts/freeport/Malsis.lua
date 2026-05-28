local ch = tostring(choice)
local quests = require('Scripts/FreeportQuests')
local items = require('Scripts/items')
function event_say()
    local diagOptions = {}
    local questText = ""
    local npcDialogue = ""
    local questRewards = {}
    if
        (class == "Magician" and race == "Human" and humanType == "Eastern" and
        GetPlayerFlags(mySession, "10010") == "noFlags")
    then
        SetPlayerFlags(mySession, "10010", "0")
    end
    if (GetPlayerFlags(mySession, "10010") == "0") then
        if (ch:find("apprentice")) then
            multiDialogue = {
                "Malsis: Oooohh, well we do require that a prospective apprentice complete a number of tasks before the enrollment.",
                "Malsis: Your first task is to acquire an iron ring from the merchant outside. Her name is Yulia. She wont charge you for the ring.",
                "Malsis: When you have the iron ring, return to me and I'll send you on your second task.",
                "You have received a quest!" }
            SendMultiDialogue(mySession, multiDialogue)
            StartQuest(mySession, 10010, quests[10010][0].log)
        else
            npcDialogue = "Say what you must, I haven't got all day."
            diagOptions = { "I wish to become an apprentice" }
        end
    elseif (GetPlayerFlags(mySession, "10010") == "1") then
        if (CheckQuestItem(mySession, items.IRON_RING, 1)) then
            if (ch:find("actually")) then
                npcDialogue =
                "I will need an item to get started. You will need the iron ring from Merchant Yulia, then return to me."
            elseif (ch:find("Yes")) then
                multiDialogue = {
                    "Malsis: Wonderful. That is no ordinary ring. A small amount of power has been infused into the metal. We'll discuss more of that later.",
                    "Malsis: Take some rest now. Return when you are ready and you shall have your next task." }
                SendMultiDialogue(mySession, multiDialogue)
                CompleteQuest(mySession, 10010, quests[10010][1].xp)
            else
                npcDialogue = "I take it you have the ring I sent you for?"
                diagOptions = { "Yes I do.", "Well, actually, no." }
            end
        else
            npcDialogue =
            "I will need an item to get started. You will need the iron ring from Merchant Yulia, then return to me."
        end
    elseif (GetPlayerFlags(mySession, "10011") == "0") then
        if (ch:find("task")) then
            multiDialogue = {
                "I have no time to offer odd jobs to every transient that decides to waltz into the Academy!!! I'll have you….........oh wait",
                "That's right, I remember you now. I apologize. You must forgive my temper. Time inevitably takes its toll upon an elementalist.",
                "Oh yes! So you're ready for your next task. I need you to speak to Spiritmaster Alshan.",
                "You can find him just outside the Academy, near the bottom of the stairs. Return only when you complete any tasks he gives you.",
                "You have received a quest!" }
            SendMultiDialogue(mySession, multiDialogue)
            StartQuest(mySession, 10011, quests[10011][0].log)
        else
            npcDialogue = "Say what you must, I haven't got all day."
            diagOptions = { "I seek my next task." }
        end
    elseif (GetPlayerFlags(mySession, "10011") == "3") then
        if (ch:find("completed")) then
            multiDialogue = {
                "Malsis: Ahh, wonderful. Be sure to have yourself bound often. It is quite inconvenient to be defeated far from your last binding.",
                "Malsis: Now that you've completed that, I have another task for you. Go see Kellina, she will assist you.",
                "Malsis: You can find Kellina just outside the temple doorway.",
                "You have finished a quest!"
            }
            SendMultiDialogue(mySession, multiDialogue)
            CompleteQuest(mySession, 10011, quests[10011][3].xp)
        elseif (ch:find("sorry")) then
            npcDialogue = "Don't stand around wasting my time"
        else
            npcDialogue = "Malsis: Didn't I send you to do something for me?"
            diagOptions = { "Yes, it is completed.", "Yes, sorry. I'll be on my way" }
        end
    else
        npcDialogue =
        "Malsis: I have no time to offer odd jobs to every transient that decides to waltz into the Academy!!! I'll have you know, We are quite busy."
    end
    SendDialogue(mySession, npcDialogue, diagOptions)
end
