local ch = tostring(choice)
function event_say()
   local diagOptions = {}
   local questText = ""
   local npcDialogue = ""
   SetPlayerFlags(mySession, "EasternMagician0", true)
   if
   (class == "Magician" and race ==  "Human" and humanType == "Freeport" and
   GetPlayerFlags(mySession, "EasternMagician0") and not GetPlayerFlags(mySession, "EasternMagician1"))
   then
      npcDialogue = "Say what you must, I haven't got all day."
      diagOptions = {"I wish to become an apprentice"}
      if (ch:find("apprentice")) then
         diagOptions = {}
         npcDialogue =
         "Malsis: Oooohh, well we do require that a prospective apprentice complete a number of tasks before the enrollment."
         SendDialogue(mySession, npcDialogue, diagOptions)
         npcDialogue =
         "Malsis: Your first task is to acquire an iron ring from the merchant outside. Her name is Yulia. She wont charge you for the ring."
         SendDialogue(mySession, npcDialogue, diagOptions)
         npcDialogue = "Malsis: When you have the iron ring, return to me and I'll send you on your second task."
         SetPlayerFlags(mySession, "EasternMagician1", true)
         local questText = "You must purchase an iron ring from Merchant Yulia, then return to Malsis."
         AddQuestLog(mySession, 0, questText)
         npcDialogue = "You have received a quest!"
      end
   elseif (GetPlayerFlags(mySession, "EasternMagician1") and not GetPlayerFlags(mySession, "EasternMagician2")) then
      if (CheckQuestItem(mySession, 31010, 1)) then
	    if(ch:find("actually"))then
        diagOptions = {}
        npcDialogue = "I will need an item to get started. You will need the iron ring from Merchant Yulia, then return to me."
        elseif(ch:find("Yes"))then
           diagOptions = {}
           npcDialogue = "Malsis: Wonderful. That is no ordinary ring. A small amount of power has been infused into the metal. We'll discuss more of that later."
	    SendDialogue(mySession, npcDialogue, diagOptions)
	    npcDialogue = "Malsis: Take some rest now. Return when you are ready and you shall have your next task."
           GrantXP(mySession, 430)
           DeleteQuestLog(mySession, 0)
	     SetPlayerFlags(mySession, "EasternMagician2", true)
	else
           npcDialogue = "I take it you have the ring I sent you for?"
           diagOptions = {"Yes I do.", "Well, actually, no."}
      	    SendDialogue(mySession, npcDialogue, diagOptions)
       end
     else
        diagOptions = {}
        npcDialogue = "I will need an item to get started. You will need the iron ring from Merchant Yulia, then return to me."
    end
    elseif (class == "Magician" and race == "Human" and humanType == "Freeport" and
          GetPlayerFlags(mySession, "EasternMagician2") and level >= 2)
      then
         if (ch:find("task")) then
            diagOptions = {}
            npcDialogue = "I have no time to offer odd jobs to every transient that decides to waltz into the Academy!!! I'll have you….........oh wait"
            SendDialogue(mySession, npcDialogue, diagOptions)
            npcDialogue = "That's right, I remember you now. I apologize. You must forgive my temper. Time inevitably takes its toll upon an elementalist."
            SendDialogue(mySession, npcDialogue, diagOptions)
            npcDialogue = "Oh yes! So you're ready for your next task. I need you to speak to Spiritmaster Alshan."
            SendDialogue(mySession, npcDialogue, diagOptions)
            npcDialogue = "You can find him just outside the Academy, near the bottom of the stairs. Return only when you complete any tasks he gives you."
            SendDialogue(mySession, npcDialogue, diagOptions)
            SetPlayerFlags(mySession, "EasternMagician3", true)
            questText = "Go speak to Spiritmaster Alshan."
            AddQuestLog(mySession, 0, questText)
            npcDialogue = "You have received a quest!"
      else
         npcDialogue = "Say what you must, I haven't got all day."
         diagOptions = { "I seek my next task." }
         end
   else
      npcDialogue =
      "Malsis: I have no time to offer odd jobs to every transient that decides to waltz into the Academy!!! I'll have you know, We are quite busy."
      end
      SendDialogue(mySession, npcDialogue, diagOptions)
end
