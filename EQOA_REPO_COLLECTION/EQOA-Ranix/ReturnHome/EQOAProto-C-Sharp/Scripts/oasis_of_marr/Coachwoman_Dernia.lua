function event_say()
    local ch = tostring(choice)
    if(ch:find("Blackwater"))
    then
    npcDialogue = "Going to Blackwater it is"
    TeleportPlayer(mySession, 0, 15165.59766, 54.12599564, 22663.25195, -1.390874982)
    elseif(ch:find("Solace"))
    then
    npcDialogue = "Going to Dark Solace then"
    TeleportPlayer(mySession, 0, 12535.52637, 83.88361359, 21185.85547, 3.127047777)
    elseif(ch:find("Hazinak"))
    then
    npcDialogue = "Going to the Hazinak Temple then"
    TeleportPlayer(mySession, 0, 24882.7168, 74.52381134, 27598.14844, -0.689268708)
    elseif(ch:find("Garden"))
    then
    npcDialogue = "Going to the Muniel's Tea Garden then"
    TeleportPlayer(mySession, 0, 23359.685546875, 54.12599563598633, 19635.919921875, 1.325449824333191)
    else
    npcDialogue = "Where would you like to go?:::Get me a horse to Blackwater%Get me a horse to Dark Solace%Get me a horse to Hazinak%Get me a horse to Muniel's Tea Garden:::"
    end
end
