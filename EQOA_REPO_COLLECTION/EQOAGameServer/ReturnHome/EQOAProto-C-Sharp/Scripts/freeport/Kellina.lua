local ch = tostring(choice)
local quests = require('Scripts/FreeportQuests')
local items = require('Scripts/items')
function event_say()
    local diagOptions = {}
    local questText = ""
    local npcDialogue = ""
    if (GetPlayerFlags(mySession, "10012") == "0") then
        if (ch:find("Malsis")) then
            multiDialogue = {
                "Kellina: Oohh, now that isn't that disappointing. Well I suppose I should instruct you. But first you must prove your worth.",
                "Kellina: Equip whatever mighty weapon you may have and slay ants. Bring me 2 cracked ant pincers as proof of your valiant deed.",
                "Kellina: After I receive the pincers I will reward you with a scroll of smoldering aura, a magician's specialty.",
                "Kellina: Now be off. I've wasted enough of my time with you."
            }
            SendMultiDialogue(mySession, multiDialogue)
            StartQuest(mySession, 10012, quests[10012][0].log)
        else
            diagOptions = {"Actually, Malsis sent me."}
            npcDialogue = "Kellina: I don't have time for chit chat, dear."
            SendDialogue(mySession, npcDialogue, diagOptions)
        end
    elseif (GetPlayerFlags(mySession, "10012") == "1") then
        if (CheckQuestItem(mySession, items.CRACKED_ANT_PINCER, 2)) then
            if (ch:find("nevermind")) then
                npcDialogue =
                    "Kellina: It's important that you bring me what I have asked for. What was it now… Ah yes, I need two cracked ant pincers."
            elseif (ch:find("pincers")) then
                multiDialogue = {
                    "Kellina: I suppose I can disregard your folly. You are new after all. And you did complete the task I assigned you.",
                    "Kellina: I am a woman of my word. Take this scroll and study it well. The spell is paltry compared to my power, but it's a start.",
                    "You have finished a quest!",
                    "You have given away a cracked ant pincer.",
                    "You have given away a cracked ant pincer.",
                    "You have received a Smoldering Aura Scroll.",
                    "Kellina: Unfortunately I must assign you another task, but I haven't the energy to deal with a novice right now. Come back later."
                }
                SendMultiDialogue(mySession, multiDialogue)
                TurnInItem(mySession, items.CRACKED_ANT_PINCER, 2)
                GrantItem(mySession, items.SMOLDERING_AURA, 1)
                CompleteQuest(mySession, 10012, quests[10012][1].xp)
            else
                npcDialogue = "I don't remember calling for you."
                diagOptions = {"I apologize Lady, but I have the pincers.", "Yes, but...I...nevermind, sorry."}
            end
        else
            npcDialogue =
                "Kellina: It's important that you bring me what I have asked for. What was it now...Ah yes, I need two cracked ant pincers."
        end
    elseif (GetPlayerFlags(mySession, "10013") == "0") then
        if (level >= 4) then
            if (ch:find("continue")) then
                multiDialogue = {
                    "Kellina: Apprenticeship? You must at least wear the blue robe of our caste first if you wish to call yourself my apprentice.",
                    "Kellina: Luckily for you I'm feeling somewhat generous today. I'll enchant the robe for you if you bring me the components."
                }
                SendMultiDialogue(mySession, multiDialogue)
                npcDialogue = "Go on, fetch them quickly"
                diagOptions = {"But...I don't know what they are?"}
                SendDialogue(mySession, npcDialogue, diagOptions)
            elseif (ch:find("know")) then
                multiDialogue = {
                    "Kellina: Soulusek's eye, do I even have to think for you now? Nevermind. What I require is a plain robe, and a silk cord.",
                    "Kellina: Plain robes can be purchased from Merchant Yulia just outside this building.",
                    "Kellina: Merchant Yesam has silk cords for sale just inside the eastern city gate nearest the docks.",
                    "Kellina: I'll also need a ruined bat wing. I'm sure the guards wouldn't mind if you collected the wing from the bat pests outside.",
                    "Kellina: Now shoo. And don't come back until you have everything in proper order.",
                    "You have received a quest!"
                }
                SendMultiDialogue(mySession, multiDialogue)
                questText =
                    "Kellina needs a plain robe from Merchant Yulia, a silk cord from Merchant Yesam, and a ruined bat wing."
                StartQuest(mySession, 10013, quests[10013][0].log)
            else
                npcDialogue = "I don't have time for chit chat, dear."
                diagOptions = {"I wish to continue my apprenticeship."}
            end
        else
            npcDialogue =
                "Kellina: Unfortunately I must assign you another task, but I haven't the energy to deal with a novice right now. Come back later."
        end
    elseif (GetPlayerFlags(mySession, "10013") == "1") then
        if
            (CheckQuestItem(mySession, items.PLAIN_ROBE, 1) and CheckQuestItem(mySession, items.SILK_CORD, 1) and
                CheckQuestItem(mySession, items.RUINED_BAT_WING, 1))
         then
            if (ch:find("No")) then
                multiDialogue = {
                    "Kellina: You'll need to prove yourself by gathering these items. I need 1 plain robe, 1 silk cord, and 1 ruined bat wing."
                }
                SendMultiDialogue(mySession, multiDialogue)
            elseif (ch:find("have")) then
                multiDialogue = {
                    "Kellina: Thank you, the enchantment on this robe shall serve to strengthen your defenses against elemental cold.",
                    "Kellina: You may now be strong enough to help with a task given to me, get some rest first though and come back when you're ready.",
                    "You have finished a quest!",
                    "You have given away plain robe",
                    "You have given away silk cord",
                    "You have given away ruined bat wing",
                    "You have received blue robe",
                    "Kellina: You will find as you use your weapons and armor in battle, they will begin to wear down and lose their effectiveness.",
                    "Kellina: If you visit a blacksmith they will repair your weapons and armor for a price."
                }
                SendMultiDialogue(mySession, multiDialogue)
                TurnInItem(mySession, items.PLAIN_ROBE, 1)
                TurnInItem(mySession, items.SILK_CORD, 1)
                TurnInItem(mySession, items.RUINED_BAT_WING, 1)
                GrantItem(mySession, items.BLUE_ROBE, 1)
                CompleteQuest(mySession, 10013, quests[10013][1].xp)
            else
                npcDialogue = "Have you collected the components?"
                diagOptions = {"I have.", "No."}
            end
        else
            npcDialogue =
                "Kellina: You'll need to prove yourself by gathering these items. I need 1 plain robe, 1 silk cord, and 1 ruined bat wing."
        end
    elseif (GetPlayerFlags(mySession, "10014") == "0") then
        if (level >= 5) then
            if (ch:find("task")) then
                multiDialogue = {
                    "Kellina: I truly hope you are my dear, for the task I have for you is necessarily fraught with peril.",
                    "Kellina: Our Academy depends on caravan shipments from the west for a great deal of our research supplies.",
                    "Kellina: Just recently, a band of highwaymen have been hijacking caravans into Freeport in both the deserts and grasslands."
                }
                SendMultiDialogue(mySession, multiDialogue)
                npcDialogue =
                    "Will you use the knowledge we have taught you thus far to recover the stolen goods and bring these criminals to justice?"
                diagOptions = {"I will.", "Sorry, I can't help you with this"}
                SendDialogue(mySession, npcDialogue, diagOptions)
            elseif (ch:find("Sorry")) then
                multiDialogue = {
                    "Kellina: Do you fancy yourself your own master? You had best reconsider getting this done or we will have to reconsider your enrollment here."
                }
                SendMultiDialogue(mySession, multiDialogue)
            elseif (ch:find("will")) then
                multiDialogue = {
                    "Kellina: Great knowledge will be the reward for your bravery. However this is no challenge to take lightly or alone.",
                    "Kellina: For this you should seek the company of other adventurers in Freeport. The Smiling Serpent tavern is a good place to start.",
                    "Kellina: I am sure that we are not the only guild being made to suffer by these criminals, and that there are others who will help.",
                    "Kellina: Go now my apprentice, and return when you've recovered the stolen goods.",
                    "You have received a quest!"
                }
                SendMultiDialogue(mySession, multiDialogue)
                StartQuest(mySession, 10014, quests[10014][0].log)
            else
                npcDialogue = "I don’t have time for chit chat, dear."
                diagOptions = {"I am ready for my next task."}
            end
        else
            npcDialogue =
                "Kellina: Unfortunately I must assign you another task, but I haven't the energy to deal with a novice right now. Come back later."
        end
    elseif (GetPlayerFlags(mySession, "10014") == "1") then
        if
            (CheckQuestItem(mySession, items.STOLEN_GOODS, 1))
         then
            if (ch:find("Not")) then
                npcDialogue =
                    "Kellina: Get your head together, and focus on the task at hand. Find the highwaymen and retrieve from them the stolen goods."
            elseif (ch:find("have")) then
                multiDialogue = {
                    "Kellina: I see...! To be honest, I wasn't sure if I would ever see you again, but here you are proving yourself more useful…",
                    "Kellina: The Academy, along with the other guilds might owe you a debt. I'm not sure what we would have done without these supplies.",
                    "Kellina: For this, I am proud to have you as a true Magician of The Academy of Arcane Science. I award you with this Motivate Scroll.",
                    "Kellina: I will certainly have more tasks for you, but I need some time to look into a few things. Come see me again later.",
                    "You have given away stolen goods.",
                    "You have finished a quest!",
                    "You have received Motivate."
                }
                SendMultiDialogue(mySession, multiDialogue)
                TurnInItem(mySession, items.STOLEN_GOODS, 1)
                GrantItem(mySession, items.MOTIVATE, 1)
                CompleteQuest(mySession, 10014, quests[10014][1].xp)
            else
                npcDialogue = "Have you retrieved the stolen goods?"
                diagOptions = {"I have. Here they are...", "Not quite yet."}
            end
        else
            npcDialogue =
                "Kellina: Get your head together, and focus on the task at hand. Find the highwaymen and retrieve from them the stolen goods."
        end
    elseif (GetPlayerFlags(mySession, "10015") == "0") then
        if (level >= 7) then
            if (ch:find("Nothing")) then
                multiDialogue = {
                    "Kellina: You'll need to learn to speak up for yourself, or you won't get far in this city."
                }
                SendMultiDialogue(mySession, multiDialogue)
            elseif (ch:find("ready")) then
                multiDialogue = {
                    "Kellina: You've returned not a moment too soon. I have a task that needs attention but I am preoccupied at the moment.",
                    "Kellina: I've just received word of a dangerous creature spotted near freeport. The Chichan is a poisonous eel that preys upon unwitting travelers.",
                    "Kellina: The thing is, the eels are not native of this region. Someone must have brought it to freeport intentionally.",
                    "Kellina: I'll need a piece of the eel to investigate further. Bring to me it's venom sac. You will find it north of Freeport, along the river.",
                    "Kellina: You'll need to learn to speak up for yourself, or you won't get far in this city.",
                    "You have received a quest!"
                }
                SendMultiDialogue(mySession, multiDialogue)
                StartQuest(mySession, 10015, quests[10015][0].log)
            else
                npcDialogue = "I am quite busy, what could you possibly need?"
                diagOptions = {"I am ready for my next assignment.", "Nothing, really."}
            end
        else
            npcDialogue =
                "Kellina: Unfortunately I must assign you another task, but I haven't the energy to deal with a novice right now. Come back later."
        end
    elseif (GetPlayerFlags(mySession, "10015") == "1") then
        if (CheckQuestItem(mySession, items.EEL_VENOM_SAC, 1)) then
            if (ch:find("sorry")) then
                npcDialogue =
                    "Bring Kellina a chichan eel venom sac. You will find it north of Freeport, along the river."
            elseif (ch:find("here")) then
                multiDialogue = {
                    "Kellina: Well done. You might be a bright spot in this otherwise dreary place.",
                    "Kellina: I must investigate this venom sac immediately.",
                    "Kellina: As for your reward, take this Infusion scroll and this pair of blackened leggings.",
                    "Kellina: Thank you for your services. Come see me later, I will have more for you.",
                    "You have given away venom sac",
                    "You have received Infusion",
                    "You have received blackened leggings",
                    "You have finished a quest!"
                }
                SendMultiDialogue(mySession, multiDialogue)
                TurnInItem(mySession, items.EEL_VENOM_SAC, 1)
                GrantItem(mySession, items.INFUSION, 1)
                GrantItem(mySession, items.BLACKENED_LEGGINGS, 1)
                CompleteQuest(mySession, 10015, quests[10015][1].xp)
            else
                npcDialogue = "Do you have the venom sac?"
                diagOptions = {"It is right here.", "Oh sorry, not yet."}
            end
        else
            npcDialogue =
                "Kellina: Do you have the eel venom sac yet?!"
        end
    elseif (GetPlayerFlags(mySession, "10016") == "0") then
        if (level >= 10) then
            if (ch:find("later")) then
                multiDialogue = {
                    "Kellina: Declining to help your teacher will not look good on your records, I assure you."
                }
                SendMultiDialogue(mySession, multiDialogue)
            elseif (ch:find("am")) then
                multiDialogue = {
                    "Kellina: I have so many requests for assistance now I can barely keep up. I must now have you help me with this next task.",
                    "Kellina: Guard Sareken is in need of assistance. Please find him at the north entrance of Freeport.",
                    "Kellina: I may call upon you later, so please check back when you can.",
                    "You have received a quest!"
                }
                SendMultiDialogue(mySession, multiDialogue)
                StartQuest(mySession, 10016, quests[10016][0].log)
            else
                npcDialogue = "Please tell me you are here to help..."
                diagOptions = {"I am.", "Maybe later..."}
            end
        else
            npcDialogue =
                "Kellina: Unfortunately I must assign you another task, but I haven't the energy to deal with a novice right now. Come back later."
        end
    elseif (GetPlayerFlags(mySession, "10017") == "0") then
        if (level >= 13) then
            if (ch:find("nevermind")) then
                multiDialogue = {
                    "Kellina: You'll need to learn to speak up for yourself, or you won't get far in this city."
                }
                SendMultiDialogue(mySession, multiDialogue)
            elseif (ch:find("returned")) then
                multiDialogue = {
                    "Kellina: Yes of course, I can use you to...Oh nevermind that.",
                    "Kellina: It is good you have returned. A colleague of mine has returned to Freeport. To be honest, I cant stand him. He's had a bit of a mishap, and needs our...your help.",
                    "Kellina: See to him at once. I must warn you, he can be a bit...demanding. See to it that his wishes are fulfilled.",
                    "Kellina: You can find Ilenar in the Shining Shield Guild Hall. Head out through the midroad, and to the southeast a little, head through the Smiling Serpent Inn.",
                    "Kellina: To the east of the inn is the Shining Shield Mercenaries. Watch your step there...",
                    "You have received a quest!"
                }
                SendMultiDialogue(mySession, multiDialogue)
                StartQuest(mySession, 10017, quests[10017][0].log)
            else
                npcDialogue = "What do you want? I really can't..."
                diagOptions = {"I have returned to learn more.", "Oh nevermind..."}
            end
        else
            npcDialogue =
                "Kellina: Unfortunately I must assign you another task, but I haven't the energy to deal with a novice right now. Come back later."
        end
    elseif (GetPlayerFlags(mySession, "10018") == "6") then
        multiDialogue = {"Kellina: I've received word that the venerable Ilenar Crelwin is pleased with your work as of late. You have proved yourself a worthy agent.",
        "Kellina: For all of your accomplishments, it is time you wore something befitting of your rank. I therefore award you with a Summoners Garb.",
        "Kellina: I also award you with this Endure Fire Scroll. May these things protect you, in dangerous places.",
        "Kellina: I believe it is time for you to explore the world a bit more. But do check back later, I may yet have another quest to fit your skills.",
        "You have finished a quest!",
        "You have received Endure Fire.",
        "You have received a summoners garb."
        }
        SendMultiDialogue(mySession, multiDialogue)
        GrantItem(mySession, items.ENDURE_FIRE, 1)
        GrantItem(mySession, items.SUMMONERS_GARB, 1)
        CompleteQuest(mySession, 10018, quests[10018][6].xp)
    else
        npcDialogue = "I am quite busy with my students right now. Are you sure you're in the right place?"
    end
    SendDialogue(mySession, npcDialogue, diagOptions)
end
