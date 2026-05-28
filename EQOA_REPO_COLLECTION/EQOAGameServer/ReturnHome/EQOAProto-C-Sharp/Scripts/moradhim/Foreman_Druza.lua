function event_say()
diagOptions = {}
    npcDialogue = "Greetins, I'm 1st assistant to the Mining Chief Zollit who is in charge of running the mining operations here. I thought I spied a glint of gold in yer eyes. Glad to have ye aboard! Miners Guild 231 serves as the scouts and spies of the city, using our craft to provide valuable scouting for the other guilds."
SendDialogue(mySession, npcDialogue, diagOptions)
end