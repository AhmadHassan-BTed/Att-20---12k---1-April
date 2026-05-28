local ch = tostring(choice)
function event_say()
    local diagOptions = {}
    local questText = ""
    local npcDialogue = ""
    local quests = require("Scripts/FreeportQuests")
    if (GetPlayerFlags(mySession, "10017") == "1") then
        if (ch:find("day")) then
            multiDialogue = {
                "Ilenar Crelwin: Oh I am not having a pleasant time at all. Not one bit I tell you. First my robe was damaged on the way here, and now I find sand everywhere in my quarters...",
                "Ilenar Crelwin: No matter, you will serve my needs just fine. Do you see this awful tear in my robe? It must be stitched at once. I can't be seen around Freeport without it like some vagabond from Qeynos.",
                "Ilenar Crelwin: The only one with the skill to fix this is all the way in Bobble-by-Water. You must take my robe to him.",
                "Ilenar Crelwin: Travel north along the river until you reach Bobble-by-Water. Find Delwin Stitchfinger, and have him repair the robe. Make haste and return to me.",
                "You have received a damaged robe.",
                "You have received a quest!"
            }
            SendMultiDialogue(mySession, multiDialogue)
            GrantItem(mySession, 8457, 1)
            ContinueQuest(mySession, 10017, quests[10017][1].log)
        else
            diagOptions = {"...And how is your day?", "Oh sorry, I must be in the wrong building."}
            npcDialogue =
                "Ilenar Crelwin: You had better be the one sent from The Academy of Arcane Science. I must me tended to at once!"
            SendDialogue(mySession, npcDialogue, diagOptions)
        end
    elseif (GetPlayerFlags(mySession, "10017") == "5") then
        if (CheckQuestItem(mySession, 8458, 1)) then
            multiDialogue = {
                "Ilenar Crelwin: Where have you been? While you have been taking your sweet time, I've had to deal with the indecency of being seen without my official robe!",
                "Ilenar Crelwin: ...I'll take my robe now….What is this? It is covered in chocolate! This is preposterous! What idiot ruined my precious robe?",
                "Ilenar Crelwin: This is the sloppy work of Delwin Stitchfinger! I am furious!...He will soon know what it means to cross me.",
                "Ilenar Crelwin: I have another mission for you. One even more serious. We will send him a 'thank you'' along with something he holds so dear.",
                "Ilenar Crelwin: Please return to Delwin with this box of 'special' chocolates. Be sure to let him know Ilenar 'appreciates' his service. Return to me when this is done.",
                "You have finished a quest!",
                "You have given away a chocolate stained robe.",
                "You have received a quest!",
                "You have received poisoned chocolates."
            }
            SendMultiDialogue(mySession, multiDialogue)
            TurnInItem(mySession, 8458, 1)
            GrantItem(mySession, 8337, 1)
            ContinueQuest(mySession, 10017, quests[10017][5].log)
        else
            multiDialogue =
                "Ilenar Crelwin: Where have you been? Why have you returned empty handed? You must bring me my robe at once!"
            SendMultiDialogue(mySession, multiDialogue)
        end
    elseif (GetPlayerFlags(mySession, "10017") == "7") then
        if (ch:find("chocolate")) then
            multiDialogue = {
                "Ilenar Crelwin: Ah yes. The sweet taste of revenge. These moments must be savored, you know. They wont come often enough.",
                "Ilenar Crelwin: You have pleased me for now. I will let Kellina know of my satisfaction.",
                "Ilenar Crelwin: I will have more work for you after I've sorted a few things out, so check back with me later. For your reward, take this powerful scroll.",
                "You have finished a quest!",
                "You have received Lava Wind."
            }
            SendMultiDialogue(mySession, multiDialogue)
            GrantItem(mySession, 8450, 1)
            CompleteQuest(mySession, 10017, quests[10017][7].xp)
        elseif (ch:find("mad")) then
            multiDialogue = {
                "Ilenar Crelwin: If you think you can escape my command, I assure you, I will have you hunted, and brought to justice. My justice."
            }
            SendMultiDialogue(mySession, multiDialogue)
        else
            diagOptions = {"I gave Delwin the chocolate. He's dead.", "You are mad, I am leaving."}
            npcDialogue = "Tell me at once what you have done."
            SendDialogue(mySession, npcDialogue, diagOptions)
        end
    elseif (GetPlayerFlags(mySession, "10018") == "0") then
        if (level >= 15) then
            if (ch:find("back")) then
                multiDialogue = {
                    "Ilenar Crelwin: You served me well before, so I will forgive your tardiness, but don’t let it happen again.",
                    "Ilenar Crelwin: I am working on a new spell, something never before attempted. But I need some rare ingredients.",
                    "Ilenar Crelwin: Nightworm roots are banned in most cities. It is a rare plant that grows only in the fetid marshes of the south, and their poisonous properties make them illegal.",
                    "Ilenar Crelwin: Fortunately I have a contact, Dagget Klem, who can get some. Klem runs a smuggling ring in a small fishing village called Temby, along the coast not far north of Freeport.",
                    "Ilenar Crelwin: Journey to Temby and arrange for the roots through Dagget Klem. Return to me when you have them.",
                    "You have received a quest!"
                }
                SendMultiDialogue(mySession, multiDialogue)
                StartQuest(mySession, 10018, quests[10018][0].log)
            elseif (ch:find("rather")) then
                multiDialogue = {
                    "Ilenar Crelwin: If you think you can escape my command, I assure you, I will have you hunted, and brought to justice. My justice."
                }
                SendMultiDialogue(mySession, multiDialogue)
            else
                diagOptions = {"I am back for another mission.", "Actually, I'd rather not talk to you right now..."}
                npcDialogue = "So you finally decided to grace me with your presence?"
                SendDialogue(mySession, npcDialogue, diagOptions)
            end
        end
    elseif (GetPlayerFlags(mySession, "10018") == "4") then
        if (CheckQuestItem(mySession, 8340, 1)) then
            multiDialogue = {
                "Ilenar Crelwin: The roots! You have them. I would recognize that smell anywhere. No one saw you with them right?",
                "Ilenar Crelwin: Now then, on to the next task. I'll need something quite dangerous to for you to fetch. I need the blood of madmen.",
                "Ilenar Crelwin: Along the coast, not too far south of Freeport, you will find the ruins of a great stone monolith. Search there for madmen.",
                "Ilenar Crelwin: You might need help with this. Those madmen are deadly. Don't return to me till you have some of their blood.",
                "You have finished a quest!",
                "You have given away a case of nightworm roots.",
                "You have received a quest!"
            }
            SendMultiDialogue(mySession, multiDialogue)
            TurnInItem(mySession, 8340, 1)
            ContinueQuest(mySession, 10018, quests[10018][4].log)
        else
            multiDialogue =
                "Ilenar Crelwin: I need nightworm root from Dagget Klem in Temby. What are you doing wasting time here?"
            SendMultiDialogue(mySession, multiDialogue)
        end
    elseif (GetPlayerFlags(mySession, "10018") == "5") then
        if (CheckQuestItem(mySession, 8341, 1)) then
            multiDialogue = {
                "Ilenar Crelwin: I can see by the sand on your boots that you’ve been to the desert. And there it is...Blood of madmen.",
                "Ilenar Crelwin: Once again, you returned to me alive, when almost any other young assistant would have perished.",
                "Ilenar Crelwin: These items you have brought me will do wonders for my research. My magic will one day be the talk of Tunaria. You may take pride in knowing that you have played a small part in that story.",
                "Ilenar Crelwin: I am done with you. I believe Kellina has something for you at this moment. Be gone with you now!",
                "You have finished a quest!",
                "You have given away blood of madman.",
                "You have received a quest!"
            }
            SendMultiDialogue(mySession, multiDialogue)
            TurnInItem(mySession, 8341, 1)
            ContinueQuest(mySession, 10018, quests[10018][5].log)
        else
            multiDialogue =
                "Ilenar Crelwin:  Find madmen at ruins south of Freeport. Return to me with some of their blood. Stop dallying!"
            SendMultiDialogue(mySession, multiDialogue)
        end
    else
        npcDialogue =
            "If you weren't sent to me, then you will leave at once or I will call the guards to have you removed."
    end
    SendDialogue(mySession, npcDialogue, diagOptions)
end
