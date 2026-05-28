function event_say()
diagOptions = {}
    npcDialogue = "Last month, one of our convicted criminals by the name of Tarson Baldfoot escaped before facing his sentence of being hanged for high crimes to the people of Bobble-by-Water. playerName, if you see this fugitive, please let us know."
SendDialogue(mySession, npcDialogue, diagOptions)
end