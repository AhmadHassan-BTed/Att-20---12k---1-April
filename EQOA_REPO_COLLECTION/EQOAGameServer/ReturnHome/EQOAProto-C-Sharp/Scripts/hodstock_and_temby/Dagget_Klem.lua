local ch = tostring(choice)
function event_say()
    local diagOptions = {}
    local questText = ""
    local npcDialogue = ""
    local quests = require("Scripts/FreeportQuests")
    if (GetPlayerFlags(mySession, "10018") == "1") then
        if (ch:find("nightworm")) then
            multiDialogue = {
                "Dagget Klem: Ah yes, Ilenar. One of my best customers. Those roots are not easy to pull from the swamps. But I happen to have a shipment for sale.",
                "Dagget Klem: The only problem is, these bloody sharks are making it impossible for ships to dock here.",
                "Dagget Klem: It's a new species of shark, they're called bloodfins. And they've been using the coast as a spawning ground.",
                "Dagget Klem: If you could kill a mother bloodfin, I could get a boat through. To lure a mother out, you'll have to kill the smaller bloodfin sharks in great numbers.",
                "Dagget Klem: Once you've killed a bloodfin mother, bring me one of its teeth as proof and 260 tunar, then I can help.",
                "You have finished a quest!",
                "You have received a quest!"
            }
            SendMultiDialogue(mySession, multiDialogue)
            ContinueQuest(mySession, 10018, quests[10018][1].log)
        elseif (ch:find("ate")) then
            multiDialogue = {"Dagget Klem: More for me then, I suppose."}
            SendMultiDialogue(mySession,multiDialogue)
        else
            diagOptions = {"Ilenar sent me for nightworm root.", "No thanks, I just ate."}
            npcDialogue = "Interested in some fish?"
            SendDialogue(mySession, npcDialogue, diagOptions)
        end
    elseif (GetPlayerFlags(mySession, "10018") == "2") then
    ---Needs bloodfin tooth
        if (CheckQuestItem(mySession, 8339, 1)) then
            multiDialogue = {
                "Dagget Klem: You look a bit doused, but it seems you've managed to take out a mother bloodfin. Impressive! Lets see that tooth...",
                "Dagget Klem: Very nice. Well, I'll be able to get that ship docked here shortly. We will get you that shipment of nightworm roots you asked for.",
                "Dagget Klem: Come back and see me later on and we I will take good care you.",
                "You have finished a quest!",
                "You have given away a bloodfin tooth.",
                "You have received a quest!"
                }
            SendMultiDialogue(mySession, multiDialogue)
            TurnInItem(mySession, 8339, 1)
            ContinueQuest(mySession, 10018, quests[10018][2].log)
        else
            multiDialogue = "Dagget Klem: Bring these items to me as soon as you possibly can. I'll need 260 Tunar to complete our transaction."
            SendMultiDialogue(mySession, multiDialogue)
        end
    elseif (GetPlayerFlags(mySession, "10018") == "3") then
        if(mySession.MyCharacter.Inventory.Tunar >= 260) then
            if (ch:find("Tunar")) then
                multiDialogue = {
                    "Dagget Klem: Ok then. Here they are. Pleasure doing business. Oh, and best not let them spill out of that case. They are quite deadly...",
                    "You have finished a quest!",
                    "You have given away 260 Tunar.",
                    "You have received a quest!",
                    "You have received a case of nightworm roots."
                }
                SendMultiDialogue(mySession, multiDialogue)
                RemoveTunar(mySession, 260)
                GrantItem(mySession, 8340,1)
                ContinueQuest(mySession, 10018, quests[10018][3].log)
            elseif(ch:find("yet")) then
                multiDialogue = {"Dagget Klem: Well, I can't wait around for all day. Figure it out and let's get this deal done."}
                SendMultiDialogue(mySession, multiDialogue)
            else
                diagOptions = {"Yes. Here is 260 Tunar.", "Not quite yet."}
                npcDialogue = "Dagget Klem: I'll need 260 Tunar to complete our transaction."
                SendDialogue(mySession, npcDialogue, diagOptions)
            end
        else
        npcDialogue =
                "Dagget Klem: Well, I can't wait around for all day. Figure it out and let's get this deal done."
        end
    else
        npcDialogue = "Did the boss send you? NO?... WELL, then EXCUSE ME, I am working here."
    end
    SendDialogue(mySession, npcDialogue, diagOptions)
end
