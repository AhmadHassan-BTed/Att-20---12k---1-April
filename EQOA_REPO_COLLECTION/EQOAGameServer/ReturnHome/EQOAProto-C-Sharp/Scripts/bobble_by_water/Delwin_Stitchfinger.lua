local ch = tostring(choice)
function event_say()
    local diagOptions = {}
    local questText = ""
    local npcDialogue = ""
    local quests = require("Scripts/FreeportQuests")
    if (GetPlayerFlags(mySession, "10017") == "2") then
        if (ch:find("tending")) then
            multiDialogue = {
                "Delwin Stitchfinger: Oh I see...yes this may take me a heartbeat to fix. The only problem is...",
                "Delwin Stitchfinger: I'm having a hankerin' somethin' fierce for some chocolate. I can't really focus without it...",
                "Would you be good enough to fetch me some chocolate?"
            }
            SendMultiDialogue(mySession, multiDialogue)
            npcDialogue = "Would you be good enough to fetch me some chocolate?"
            diagOptions = {"This must be terrible for you. Sure...", "Sorry, I shouldn't be around chocolate..."}
            SendDialogue(mySession, npcDialogue, diagOptions)
        elseif (ch:find("terrible")) then
            if (CheckQuestItem(mySession, 8457, 1)) then
                multiDialogue = {
                    "Delwin Stitchfinger: Wonderful! Please go see Grocer Fritz and purchase some fine chocolate. I'll see about starting on this robe...",
                    "You have given away a damaged robe.",
                    "You have finished a quest!",
                    "You have received a quest!"
                }
                SendMultiDialogue(mySession, multiDialogue)
                TurnInItem(mySession, 8457, 1)
                ContinueQuest(mySession, 10017, quests[10017][2].log)
            else
                multiDialogue = {"I'd like to get started but you don't seem to have the damaged robe"}
                SendMultiDialogue(mySession, multiDialogue)
            end
        elseif (ch:find("Sorry")) then
            multiDialogue = {
                "Delwin Stitchfinger: I know what you mean. It is a lavish delacacy, but one that I can't live without."
            }
            SendMultiDialogue(mySession, multiDialogue)
        elseif (ch:find("view")) then
            multiDialogue = {"Delwin Stitchfinger: Welp, I hope you have a pleasant day."}
            SendMultiDialogue(mySession, multiDialogue)
        else
            multiDialogue = {"Delwin Stitchfinger: Hmm..."}
            SendMultiDialogue(mySession, multiDialogue)
            diagOptions = {"I have a robe that needs tending...", "I am just here for the view."}
            npcDialogue = "Well hello there, have you something I can mend?"
            SendDialogue(mySession, npcDialogue, diagOptions)
        end
    elseif (GetPlayerFlags(mySession, "10017") == "3") then
        if (CheckQuestItem(mySession, 8335, 1)) then
            if (ch:find("Sorry")) then
                npcDialogue =
                    "Delwin Stitchfinger: Hmm...Go see Grocer Fritz, purchase some fine chocolate and return to me."
            elseif (ch:find("Yes")) then
                multiDialogue = {
                    "Delwin Stitchfinger: Oh...Hello! Uh...err, is that the chocolate? I'll just take it, here...*NOM, NOM*",
                    "Delwin Stitchfinger: Oh yes, Heavenly. Thank you. I am ok now. I believe I can now focus on this fancy robe . Please give me a little time and check back.",
                    "Delwin Stitchfinger: You have given away "
                }
                SendMultiDialogue(mySession, multiDialogue)
                TurnInItem(mySession, 8335, 1)
                ContinueQuest(mySession, 10017, quests[10017][3].log)
            else
                npcDialogue = "Were you able to procure the chocolate?"
                diagOptions = {"Yes, here you are.", "Sorry, not yet"}
            end
        else
            npcDialogue =
                "Delwin Stitchfinger: Hmm...Go see Grocer Fritz, purchase some fine chocolate and return to me."
        end
    elseif (GetPlayerFlags(mySession, "10017") == "4") then
        multiDialogue = {
            "Delwin Stitchfinger: Well then, I'm all finished here. Goodness, looks like I've smeared a wee bit of chocolate on the robe. Not to worry, a quick wipe down and...",
            "Delwin Stitchfinger: ...Should be good as new. Oh dear, where are my glasses? Anyway, you best be off to return that fancy robe to it's master.",
            "You have finished a quest!",
            "You have received a quest!",
            "You have received a chocolate stained robe."
        }
        SendMultiDialogue(mySession, multiDialogue)
        GrantItem(mySession, 8458, 1)
        ContinueQuest(mySession, 10017, quests[10017][4].log)
    elseif (GetPlayerFlags(mySession, "10017") == "6") then
        if (CheckQuestItem(mySession, 8337, 1)) then
            if (ch:find("nevermind")) then
                npcDialogue =
                    "Delwin Stitchfinger: Hmm...Go see Grocer Fritz, purchase some fine chocolate and return to me."
            elseif (ch:find("thank")) then
                multiDialogue = {
                    "Delwin Stitchfinger: Oh my stars, what a delectable selection. They look irresistible. Tell Ilenar that I was happy to be of service.",
                    "Delwin Stitchfinger: I simply must try one...lets see...*NOM NOM NOM*. Oh so good, I must have another…*NOM NOM* I have never had a...*COUGH* *GASP* *GAG*",
                    "You have finished a quest!",
                    "You have given away poisoned chocolates.",
                    "You have received a quest!"
                }
                SendMultiDialogue(mySession, multiDialogue)
                TurnInItem(mySession, 8337, 1)
                --thisNPC.Animation = 0x0e
                ContinueQuest(mySession, 10017, quests[10017][6].log)
            else
                npcDialogue = "Well hello there, have you something I can mend?"
                diagOptions = {"Here's some 'special' chocolate from Ilenar to say thank you.", "Uh...err, nevermind."}
            end
        end
    else
        npcDialogue = "Delwin Stitchfinger: Welp, I hope you have a pleasant day."
    end
    SendDialogue(mySession, npcDialogue, diagOptions)
end
