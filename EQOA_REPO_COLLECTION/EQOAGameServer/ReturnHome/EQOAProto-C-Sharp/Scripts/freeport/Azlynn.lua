local ch = tostring(choice)
local quests = require('Scripts/FreeportQuests')

function event_say()
    local diagOptions = {}
    local questText = ""
    local npcDialogue = ""
    if
        (class == "Enchanter" and race == "Human" and humanType == "Eastern" and
            GetPlayerFlags(mySession, "12010") == "noFlags")
     then
        SetPlayerFlags(mySession, "12010", "0")
    end
    if (GetPlayerFlags(mySession, "12010") == "0") then
        if (ch:find("enchanter")) then
            multiDialogue = { "Azlynn: Most who seek enchantments do not realize how much that they will be trading in their old life for the new one. This is much more than mastering illusions.",
            "Azlynn: But if you insist, I will expect you to complete a number of tasks before you will earn the title of enchanter.",
            "Azlynn: Your first task is to acquire a Bronze Ring from the merchant Yulia. Your fee for this will be waived.",
            "Azlynn: When you have the Bronze Ring return to me and I'll send you on your second task.",
            "You have received a quest!" }
            SendMultiDialogue(mySession, multiDialogue)
            StartQuest(mySession, 12010, quests[12010][0].log)
        else
            npcDialogue = "What brings you to this house of magic?"
            diagOptions = {"I wish to be a enchanter of The Academy of Arcane Science."}
        end
    elseif (GetPlayerFlags(mySession, "12010") == "1") then
        if (CheckQuestItem(mySession, 8305, 1)) then
            if (ch:find("nevermind")) then
                npcDialogue = "I will need an item to get started. You will need the iron ring from Merchant Yulia, then return to me."
            elseif (ch:find("ring")) then
                multiDialogue = { "Azlynn: It's going to take more than running errands to learn this power. We are going to have to test your wit and your will…",
                "Azlynn: I will have your next task ready in a few moments. Don’t wander off now…" }
                SendMultiDialogue(mySession, multiDialogue)
                CompleteQuest(mySession, 12010, quests[12010][1].xp)
            else
                npcDialogue = "You have much to learn about making an entrance."
                diagOptions = {"I have the ring.", "Oh, nevermind."}
            end
        else
            npcDialogue = "I will need an item to get started. You will need the Bronze Ring from Merchant Yulia, then return to me."
        end
    elseif (GetPlayerFlags(mySession, "12011") == "0") then
        if (ch:find("am")) then
            multiDialogue = { "Azlynn: No class is as tricky and deceptive as the Enchanter. Able to disguise themselves with magic spells, they can slip into areas where they would normally be killed on sight and interact freely with their enemies.",
            "Azlynn: They can also toy with the mind, such as strengthening the resolve of those around them or forcing their enemies to switch sides and fight for the enchanter.",
            "Azlynn: Enchanters are poor fighters and die easily in combat, but they are a valuable support class. The enchanter's spells help keep the party's power full.",
             "Azlynn: As an Enchanter, you will be able to diffuse serious situations by controlling your enemies.",
             "Azlynn: Now listen carefully. I need you to speak to Spiritmaster Ashlan.",
             "Azlynn: You can find him just outside this building. Return only when you complete any tasks he gives you.",
            "You have received a quest!" }
            SendMultiDialogue(mySession, multiDialogue)
            SetPlayerFlags(mySession, "12011", "1")
            questText = "Go speak to Spiritmaster Alshan."
            StartQuest(mySession, 12011, quests[12011][0].log)
        else
            npcDialogue = "Are you ready for what is next?"
            diagOptions = {"I am."}
        end
    elseif (GetPlayerFlags(mySession, "12011") == "3") then
        if (ch:find("done")) then
            CompleteQuest(mySession, 12011, quests[12011][3].xp)
            multiDialogue = {
                "Azlynn: Ahh, wonderful. Be sure to have yourself bound often. It is quite inconvenient to be defeated far from your last binding.",
                "Azlynn: Now that you've completed that, I have another task for you. Go see Opanheim, he will assist you.",
                "Azlynn: You can find Opanheim on the other side if this ramp..",
                "You have finished a quest!"
            }
            SendMultiDialogue(mySession, multiDialogue)
        else
            npcDialogue = "Azlynn: Didn't I send you to do something for me?"
            diagOptions = {"Yes, it is done."}
        end
    else
        npcDialogue =
            "Azlynn: I have no time to offer odd jobs to every transient that decides to waltz into the Academy!!! I'll have you know, We are quite busy."
    end
    SendDialogue(mySession, npcDialogue, diagOptions)
end
