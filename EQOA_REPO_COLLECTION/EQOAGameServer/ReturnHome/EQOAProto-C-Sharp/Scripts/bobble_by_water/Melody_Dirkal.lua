function event_say()
diagOptions = {}
    npcDialogue = "This isn't just any old windmill. It has been anointed by the druids of our kind. It infuses the air with an ancient remedy to a long forgotten curse. It is in the form of a mist, and quite undetectable. It just so happens that any dark elf that finds themselves in this mist will have trouble breathing, and moving properly. It's one of the things that we do to help protect our town."
SendDialogue(mySession, npcDialogue, diagOptions)
end