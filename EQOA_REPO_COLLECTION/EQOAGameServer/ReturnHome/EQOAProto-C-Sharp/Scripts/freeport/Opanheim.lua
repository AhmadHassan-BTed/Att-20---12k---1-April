local ch = tostring(choice)
function event_say()
   local diagOptions = {}
   local questText = ""
   local npcDialogue = ""
if(GetPlayerFlags(mySession, "12012") == "0") then
  if(ch:find("Azlynn")) then
      multiDialogue = { "Opanheim: A yes, fresh talent. It is my pleasure to see what you are made of. But I suppose you'll need some training first.",
      "Opanheim: Equip whatever mighty weapon you may have and beetles. Bring me 2 beetle carapace fragments as proof of your heroic deed.",
      "Opanheim: After I receive the carapaces, I will reward you with a Crawling Skin Scroll, an enchanter's specialty.",
      "Opanheim: Run along now. I'll be watching you with great interest." }
      SendMultiDialogue(mySession, multiDialogue)
      SetPlayerFlags(mySession, "12012", "1")
      questText = "Return two beetle carapace fragments to Opanheim."
      AddQuestLog(mySession, 0, questText)
  else
      diagOptions = { "Azlynn sent me to you."}
      npcDialogue = "Opanheim: Hello there young one."
      SendDialogue(mySession, npcDialogue, diagOptions)
  end
elseif (GetPlayerFlags(mySession, "12012") == "1") then
    if (CheckQuestItem(mySession, 4861, 2)) then
	  if(ch:find("excuse"))then
         npcDialogue = "It's important that you bring me what I have asked from you. I need two beetle carapace fragments."
      elseif(ch:find("beetle"))then
         multiDialogue = { "Opanheim: I find your work to be quite satisfactory. Maybe I will play with you a bit longer.",
         "Opanheim: As I said, you will be rewarded. Take this scroll and study it well. The spell is trivial compared to my power, but it's a start.",
         "You have finished a quest!", "You have given away a beetle carapace fragment.", "You have given away a beetle carapace fragment.",
         "You have received a Crawling Skin Scroll." ,
         "Opanheim: I'm just putting the finishing touches on an invisibility spell. Check back with me in few moments little one." }
	     SendMultiDialogue(mySession, multiDialogue)
         GrantXP(mySession, 6900)
         DeleteQuestLog(mySession, 0)
	     SetPlayerFlags(mySession, "12012", "99")
         SetPlayerFlags(mySession, "12013", "0")
	   else
         npcDialogue = "Ah, my little friend has returned."
         diagOptions = {"I have pieces of beetle.", "Oh, excuse me."}
       end
    else
    npcDialogue = "It's important that you bring me what I have asked from you. I need two beetle carapace fragments."
    end
else
npcDialogue = "I am quite busy with my students right now. Are you sure you're in the right place?"
end
SendDialogue(mySession, npcDialogue, diagOptions)
end
