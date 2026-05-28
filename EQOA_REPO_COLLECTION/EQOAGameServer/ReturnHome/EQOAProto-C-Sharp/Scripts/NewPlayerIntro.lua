local ch = tostring(choice)
function event_say()
    local diagOptions = {}
    local questText = ""
    local npcDialogue = ""
    --Human
    if (race == "Human") then
        --Freeport/Eastern
        if (humanType == "Eastern") then
            npcDialogue =
                "Welcome to Freeport, playerName. This is the city of trade and the eastern home of the humans."
            if (class == "Magician") then
                multiDialogue = {
                    "As a new magician you been invited to join the Academy of Arcane Science. They are the Magician's guild here in Freeport.",
                    "You are now ready to move on to your first quest. Speak to Malsis, he is the guildmaster standing before you."
                }
            elseif (class == "Necromancer") then
                multiDialogue = {
                    "As a new necromancer, you have been invited to join the House Slaerin. They are the necromancer's guild here in Freeport.",
                   "Then you are ready to move on to your first quest. Speak to Corious Slaerin, he is the guildmaster standing before you."
                }
            elseif (class == "Alchemist") then
                multiDialogue = {
                    "As a new alchemist, you have been invited to join the Academy of Arcane Science. They are the alchemist's guild here in Freeport.",
                    "Then you are ready to move on to your first quest. Speak to Dilina McNerian, she is the guildmaster standing before you."
                }
            elseif (class == "Bard") then
                multiDialogue = {
                    "As a new bard, you have been invited to join The Silken Gauntlet. They are the bard's guild here in Freeport.",
                    "Then you are ready to move on to your first quest. Speak to William Corufost, he is the guildmaster standing before you."
                }
            elseif (class == "Cleric") then
                multiDialogue = {
                    "As a new cleric, you have been invited to join The Spiteful Shield. They are the cleric's guild here in Freeport.",
                    "Then you are ready to move on to your first quest. Speak to Denouncer Alshea, she is the guildmaster standing before you."
                }
            elseif (class == "Enchanter") then
                multiDialogue = {
                    "As a new enchanter, you have been invited to join the Academy of Arcane Science. They are the enchanter's guild here in Freeport.",
                    "Then you are ready to move on to your first quest. Speak to Azlynn, she is the guildmaster standing before you."
                }
            elseif (class == "Rogue") then
                multiDialogue = {
                    "As a new rogue, you have been invited to join The Spiteful Shield. They are the rogue's guild here in Freeport.",
                    "Then you are ready to move on to your first quest. Speak to Necorik the Ghost, he is the guildmaster standing before you."
                }
            elseif (class == "ShadowKnight") then
                multiDialogue = {
                    "As a new shadowknight, you have been invited to join The Spiteful Shield. They are the shadowknight's guild here in Freeport.",
                    "Then you are ready to move on to your first quest. Speak to Malethai Crimsonhand, he is the guildmaster standing before you."
                }
            elseif (class == "Warrior") then
                multiDialogue = {
                    "As a new warrior, you have been invited to join The Freeport Militia. They are the warrior's guild here in Freeport.",
                    "Then you are ready to move on to your first quest. Speak to Commander Nothard, he is the guildmaster standing before you."
                }
            elseif (class == "Wizard") then
                multiDialogue = {
                    "As a new wizard, you have been invited to join the Academy of Arcane Science. They are the wizard's guild here in Freeport.",
                    "Then you are ready to move on to your first quest. Speak to Sivrendesh, he is the guildmaster standing before you."
                }
            end
        elseif (humanType == "Western") then
                    --Qeynos/Western
            npcDialogue =
                "Welcome to Qeynos, playerName. This is the western home of the humans and the newly crowned king, Antonius Bayle II."
            if (class == "Alchemist") then
                multiDialogue = {
                    "As a new alchemist, you have been invited to join the Anagogical Order. They are the alchemist's guild here in Qeynos.",
                    "Then you are ready to move on to your first quest. Speak to Jergish Anaebarum, he is the guildmaster standing before you."
                }
            elseif (class == "Bard") then
                multiDialogue = {
                    "As a new bard, you have been invited to join the Qeynos Troupe. They are the bard's guild here in Qeynos.",
                    "Then you are ready to move on to your first quest. Speak to Thrush Baird, he is the guildmaster standing before you."
                }
            elseif (class == "Cleric") then
                multiDialogue = {
                    "As a new cleric, you have been invited to join the Defenders of Erollisi Marr. They are the cleric's guild here in Qeynos.",
                    "Then you are ready to move on to your first quest. Speak to Vedilion Brithstar, he is the guildmaster standing before you."
                }
            elseif (class == "Druid") then
                multiDialogue = {
                    "Welcome to Surefall Glade playerName, sacred home to the human rangers and druids of Tunaria.",
                    "As a new druid, you have been invited to join the Jaggedpine Treefolk. They are the druid's guild here in Surefall Glade.",
                    "Then you are ready to move on to your first quest. Speak to Shiol'Anara, she is the guildmaster standing before you."
                }
            elseif (class == "Enchanter") then
                multiDialogue = {
                    "As a new enchanter, you have been invited to join the Anagogical Order. They are the enchanter's guild here in Qeynos.",
                    "Then you are ready to move on to your first quest. Speak to Shiassa Radian, she is the guildmaster standing before you."
                }
            elseif (class == "Magician") then
                multiDialogue = {
                    "As a new magician, you have been invited to join the Anagogical Order. They are the magician's guild here in Qeynos.",
                    "Then you are ready to move on to your first quest. Speak to Ammathor Lithkin, he is the guildmaster standing before you."
                }
            elseif (class == "Monk") then
                multiDialogue = {
                    "As a new monk, you have been invited to join the Order of the Silent Fist. They are the monk's guild here in Qeynos.",
                    "Then you are ready to move on to your first quest. Speak to Master T'an Chen, he is the guildmaster standing before you."
                }
            elseif (class == "Paladin") then
                multiDialogue = {
                    "As a new paladin, you have been invited to join the Crusaders of Mithaniel Marr. They are the paladin's guild here in Qeynos.",
                    "Then you are ready to move on to your first quest. Speak to Sir Hanst Breach, he is the guildmaster standing before you."
                }
            elseif (class == "Ranger") then
                multiDialogue = {
                    "Welcome to Surefall Glade playerName, sacred home to the human rangers and druids of Tunaria.",
                    "As a new ranger, you have been invited to join the Protectors of the Pine. They are the ranger's guild here in Surefall Glade.",
                    "Then you are ready to move on to your first quest. Speak to Sigmor Fallbourne, he is the guildmaster standing before you."
                }
            elseif (class == "Rogue") then
                multiDialogue = {
                    "As a new rogue, you have been invited to join the The Heavy Purse. They are the rogue's guild here in Qeynos.",
                    "Then you are ready to move on to your first quest. Speak to Snyde Cragsmear, he is the guildmaster standing before you."
                }
            elseif (class == "Warrior") then
                multiDialogue = {
                    "As a new warrior, you have been invited to join the The Qeynos Guard. They are the warrior's guild here in Qeynos.",
                    "Then you are ready to move on to your first quest. Speak to Roger Stoutheart, he is the guildmaster standing before you."
                }
            elseif (class == "Wizard") then
                multiDialogue = {
                    "As a new wizard, you have been invited to join the Anagogical Order. They are the wizard's guild here in Qeynos.",
                    "Then you are ready to move on to your first quest. Speak to Gadenon Flamefist, he is the guildmaster standing before you."
                }
            end
        end
    elseif (race == "Barbarian") then
        --Barbarian
        npcDialogue = "Welcome to Halas, playerName. This is the city of Barbarians, here on Tunaria."
        if (class == "Rogue") then
            multiDialogue = {
                "As a new rogue, you have been invited to join the Eyes of the Tribunal. They are the rogue's guild here in Halas.",
                "Then you are ready to move on to your first quest. Speak to Juno Felligan, she is the guildmaster standing before you."
            }
        elseif (class == "Shaman") then
            multiDialogue = {
                "As a new shaman, you have been invited to join the Seers of the Tribunal. They are the shaman's guild here in Halas.",
                "Then you are ready to move on to your first quest. Speak to Beril O'Leary, he is the guildmaster standing before you."
            }
        elseif (class == "Warrior") then
            multiDialogue = {
                "As a new warrior, you have been invited to join The Wolves of the North. They are the warrior's guild here in Halas.",
                "Then you are ready to move on to your first quest. Speak to Marik McPherson, he is the guildmaster standing before you."
            }
        end
    elseif (race == "Dark_Elf") then
        --Dark Elf
        npcDialogue = "Welcome to Neriak, playerName. This is the home of the Teir'Dal and it is wrought with hate."
        if (class == "Alchemist") then
            multiDialogue = {
                "As a new alchemist, you have been invited to join The Spurned. They are the alchemist's guild here in Neriak.",
                "Then you are ready to move on to your first quest. Speak to Raemiss D`Mariji, he is the guildmaster standing before you."
            }
        elseif (class == "Cleric") then
            multiDialogue = {
                "As a new cleric, you have been invited to join the Church of Innoruuk. They are the cleric's guild here in Neriak.",
                "Then you are ready to move on to your first quest. Speak to Theologist S`Tai, he is the guildmaster standing before you."
            }
        elseif (class == "Enchanter") then
            multiDialogue = {
                "As a new enchanter, you have been invited to join The Spurned. They are the enchanter's guild here in Neriak.",
                "Then you are ready to move on to your first quest. Speak to Dominary K`Jartan, he is the guildmaster standing before you."
            }
        elseif (class == "Magician") then
            multiDialogue = {
                "As a new magician, you have been invited to join The Spurned. They are the magician's guild here in Neriak.",
                "Then you are ready to move on to your first quest. Speak to Elementalist R`Virr, he is the guildmaster standing before you."
            }
        elseif (class == "Necromancer") then
            multiDialogue = {
                "As a new necromancer, you have been invited to join the Lodge of the Dead. They are the necromancer's guild here in Neriak.",
                "When you are ready to move on to your first quest. Speak to Grand Defiler J`Narus, he is the guildmaster standing before you."
            }
        elseif (class == "Rogue") then
            multiDialogue = {
                "As a new rogue, you have been invited to join The Ebon Mask. They are the rogue's guild here in Neriak.",
                "Then you are ready to move on to your first quest. Speak to Kriyn, she is the guildmaster standing before you."
            }
        elseif (class == "ShadowKnight") then
            multiDialogue = {
                "As a new shadowknight, you have been invited to join the Lodge of the Dead. They are the shadowknight's guild here in Neriak.",
                "Then you are ready to move on to your first quest. Speak to Zethril Do`Vexis, he is the guildmaster standing before you."
            }
        elseif (class == "Warrior") then
            multiDialogue = {
                "As a new warrior, you have been invited to join the The Indigo Brotherhood. They are the warrior's guild here in Neriak.",
                "Then you are ready to move on to your first quest. Speak to Scornmaster U`Dedne, he is the guildmaster standing before you."
            }
        elseif (class == "Wizard") then
            multiDialogue = {
                "As a new wizard, you have been invited to join The Spurned. They are the wizard's guild here in Neriak.",
                "Then you are ready to move on to your first quest. Speak to Sorceress X`Lottl, she is the guildmaster standing before you."
            }
        end
    elseif (race == "Dwarf") then
    --Dwarf
        npcDialogue = "Welcome to Moradhim, playerName, home of the stout dwarves and the fiercest brew of these lands."
        if (class == "Cleric") then
            multiDialogue = {
                "As a new cleric, you have been invited to join The Church of Below. They are the cleric's guild here in Moradhim",
                "Then you are ready to move on to your first quest. Speak to Bishop Gundar, he is the guildmaster standing before you."
            }
        elseif (class == "Paladin") then
            multiDialogue = {
                "As a new paladin, you have been invited to join The Doomseekers. They are the paladin's guild here in Moradhim.",
                "Then you are ready to move on to your first quest. Speak to Ungrist the Prophet, he is the guildmaster standing before you."
            }
        elseif (class == "Rogue") then
            multiDialogue = {
                "As a new rogue, you have been invited to join the Miners Guild 231. They are the rogue's guild here in Moradhim.",
                "Then you are ready to move on to your first quest. Speak to Foreman Druza, she is the guildmaster standing before you."
            }
        elseif (class == "Warrior") then
            multiDialogue = {
                "As a new warrior, you have been invited to join The Stone Guard. They are the warrior's guild here in Moradhim.",
                "Then you are ready to move on to your first quest. Speak to Field General Oxfist, he is the guildmaster standing before you."
            }
        end
    elseif (race == "Elf") then
    --Elves
        npcDialogue = "Welcome to Fayspires playerName. This is the last bastion of hope for the Highborne elves of Tunaria."
        if (class == "Rogue" or class == "Ranger" or class == "Druid" or class == "Bard") then
           npcDialogue = "Welcome to Tethelin playerName. This is the home of the woodland elves, here in Tunaria."
        end
        if (class == "Rogue") then
            multiDialogue = {
                "As a new rogue, you have been invited to join The Scouts of Tunare. They are the rogue's guild here in Tethelin.",
                "Then you are ready to move on to your first quest. Speak to Eterin Nitegazer, he is the guildmaster standing before you."
            }
        elseif (class == "Ranger") then
            multiDialogue = {
                "As a new ranger, you have been invited to join The Scouts of Tunare. They are the ranger's guild here in Tethelin.",
                "Then you are ready to move on to your first quest. Speak to Lythen Trueshot, he is the guildmaster standing before you."
            }
        elseif (class == "Druid") then
            multiDialogue = {
                "As a new druid, you have been invited to join the Keepers of the Glade. They are the druid's guild here in Tethelin.",
                "Then you are ready to move on to your first quest. Speak to Dawnseer Mistwelder, he is the guildmaster standing before you."
            }
        elseif (class == "Bard") then
            multiDialogue = {
                "As a new bard, you have been invited to join the Songweavers of Tunare. They are the bard's guild here in Tethelin.",
                "Then you are ready to move on to your first quest. Speak to Torenia Eaglesong, she is the guildmaster standing before you."
            }
        elseif (class == "Alchemist") then
            multiDialogue = {
                "As a new alchemist, you have been invited to join the College of High Magic. They are the alchemist's guild here in Fayspires.",
                "Then you are ready to move on to your first quest. Speak to Silnea Aesiowe, she is the guildmaster standing before you."
            }
        elseif (class == "Cleric") then
            multiDialogue = {
                "As a new cleric, you have been invited to join the Church of Tunare. They are the cleric's guild here in Fayspires.",
                "Then you are ready to move on to your first quest. Speak to Tessarina Starshimmer, she is the guildmaster standing before you."
            }
        elseif (class == "Enchanter") then
            multiDialogue = {
                "As a new enchanter, you have been invited to join the College of High Magic. They are the enchanter's guild here in Fayspires.",
                "Then you are ready to move on to your first quest. Speak to Casalandria Lyssia, she is the guildmaster standing before you."
            }
        elseif (class == "Magician") then
            multiDialogue = {
                "As a new magician, you have been invited to join the College of High Magic. They are the magician's guild here in Fayspires.",
                "Then you are ready to move on to your first quest. Speak to Fethinar Silspin, he is the guildmaster standing before you."
            }
        elseif (class == "Paladin") then
            multiDialogue = {
                "As a new paladin, you have been invited to join The Paladins of Tunare. They are the paladin's guild here in Fayspires.",
                "Then you are ready to move on to your first quest. Speak to Sir Lothwin Galiel, he is the guildmaster standing before you."
            }
        elseif (class == "Wizard") then
            multiDialogue = {
                "As a new wizard, you have been invited to join the College of High Magic. They are the wizard's guild here in Fayspires.",
                "Then you are ready to move on to your first quest. Speak to Lyriam Kaelean, he is the guildmaster standing before you."
            }
        end
    elseif (race == "Erudite") then
    --Highbourne
        npcDialogue = "Welcome to Highbourne playerName. This is the beautiful home of the Erudite, here on Tunaria."
        if (class == "Alchemist") then
            multiDialogue = {
                "As a new alchemist, you have been invited to join The Alchemist's Guild. They are the alchemist's guild here in Highbourne.",
                "Then you are ready to move on to your first quest. Speak to Melina Quiscellin, she is the guildmaster standing before you."
            }
        elseif (class == "Cleric") then
            multiDialogue = {
                "As a new cleric, you have been invited to join The Hand of Quellious. They are the cleric's guild here in Highbourne.",
                "Then you are ready to move on to your first quest. Speak to Arch Bishop Erah, he is the guildmaster standing before you."
            }
        elseif (class == "Enchanter") then
            multiDialogue = {
                "As a new enchanter, you have been invited to join the Craft Keepers. They are the enchanter's guild here in Highbourne.",
                "Then you are ready to move on to your first quest. Speak to Master Delar, he is the guildmaster standing before you."
            }
        elseif (class == "Magician") then
            multiDialogue = {
                "As a new magician, you have been invited to join the Gate Callers. They are the magician's guild here in Highbourne.",
                "Then you are ready to move on to your first quest. Speak to Master Veljhan, he is the guildmaster standing before you."
            }
        elseif (class == "Necromancer") then
            multiDialogue = {
                "As a new necromancer, you have been invited to join The Hidden. They are the necromancer's guild here in Highbourne.",
                "Then you are ready to move on to your first quest. Speak to Retainer Alishai, he is the guildmaster standing before you."
            }
        elseif (class == "Paladin") then
            multiDialogue = {
                "As a new paladin, you have been invited to join The Peacekeepers. They are the paladin's guild here in Highbourne.",
                "Then you are ready to move on to your first quest. Speak to Zulan Sunshield, she is the guildmaster standing before you."
            }
        elseif (class == "ShadowKnight") then
            multiDialogue = {
                "As a new shadowknight, you have been invited to join The Hidden. They are the shadowknight's guild here in Highbourne.",
                "Then you are ready to move on to your first quest. Speak to Desh the Harvester, he is the guildmaster standing before you."
            }
        elseif (class == "Wizard") then
            multiDialogue = {
                "As a new wizard, you have been invited to join the Crimson Hand. They are the wizard's guild here in Highbourne.",
                "Then you are ready to move on to your first quest. Speak to Master Rayne, she is the guildmaster standing before you."
            }
        end
    elseif (race == "Gnome") then
    --Gnomes
        npcDialogue = "Welcome to Klick' Anon playerName. This will be your home while on Tunaria."
        if (class == "Alchemist") then
            multiDialogue = {
                "As a new alchemist, you have been invited to join the Mechanamagical College. They are the alchemist's guild here in Klick' Anon.",
                "Then you are ready to move on to your first quest. Speak to Lanlin Ogledmaggen, he is the guildmaster standing before you."
            }
        elseif (class == "Cleric") then
            multiDialogue = {
                "As a new cleric, you have been invited to join the Temple of Brell. They are the cleric's guild here in Klick' Anon.",
                "Then you are ready to move on to your first quest. Speak to Teka Harnswoof, she is the guildmaster standing before you."
            }
        elseif (class == "Enchanter") then
            multiDialogue = {
                "As a new enchanter, you have been invited to join the Mechanamagical College. They are the enchanter's guild here in Klick' Anon.",
                "Then you are ready to move on to your first quest. Speak to Professor Temwiddle, she is the guildmaster standing before you."
            }
        elseif (class == "Magician") then
            multiDialogue = {"As a new magician, you have been invited to join the Mechanamagical College. They are the magician's guild here in Klick' Anon.",
            "Then you are ready to move on to your first quest. Speak to Werlib Quackook, he is the guildmaster standing before you."
            }
        elseif (class == "Necromancer") then
            multiDialogue = {
                "As a new necromancer, you have been invited to join the Necrological Society. They are the necromancer's guild here in Klick' Anon.",
                "Then you are ready to move on to your first quest. Speak to Grear Hosrottle, he is the guildmaster standing before you."
            }
        elseif (class == "Rogue") then
            multiDialogue = {
                "As a new rogue, you have been invited to join The Junk Mongers. They are the rogue's guild here in Klick' Anon.",
                "Then you are ready to move on to your first quest. Speak to Mistress Briva, she is the guildmaster standing before you."
            }
        elseif (class == "Warrior") then
            multiDialogue = {
                "As a new warrior, you have been invited to join the Klick`Anon Watch. They are the warrior's guild here in Klick' Anon.",
                "Then you are ready to move on to your first quest. Speak to Captain Buntattle, he is the guildmaster standing before you."
            }
        elseif (class == "Wizard") then
            multiDialogue = {
                "As a new wizard, you have been invited to join the Mechanamagical College. They are the wizard's guild here in Klick' Anon.",
                "Then you are ready to move on to your first quest. Speak to Pazelfun Pansoof, he is the guildmaster standing before you."
            }
        end
    elseif (race == "Halfling") then
    --Halfling
        npcDialogue = "Welcome to Rivervale playerName, home of the mischievous halflings and haphazard happenings."
        if (class == "Cleric") then
            multiDialogue = {
                "As a new cleric, you have been invited to join the Temple of Bristlebane. They are the cleric's guild here in Rivervale.",
                "Then you are ready to move on to your first quest. Speak to Pora Meepup, she is the guildmaster standing before you."
            }
        elseif (class == "Druid") then
            multiDialogue = {
                "As a new druid, you have been invited to join The Stormreapers. They are the druid's guild here in Rivervale.",
                "Then you are ready to move on to your first quest. Speak to Deke Gabbins, he is the guildmaster standing before you."
            }
        elseif (class == "Rogue") then
            multiDialogue = {
                "As a new rogue, you have been invited to join The Deep Pockets. They are the rogue's guild here in Rivervale.",
                "Then you are ready to move on to your first quest. Speak to Walt Deeppockets, he is the guildmaster standing before you."
            }
        elseif (class == "Warrior") then
            multiDialogue = {
                "As a new warrior, you have been invited to join The Guardians of the Vale. They are the warrior's guild here in Rivervale.",
                "Then you are ready to move on to your first quest. Speak to Sheriff Hopper, he is the guildmaster standing before you."
            }
        end
    elseif (race == "Ogre") then
    --Ogre
        npcDialogue = "Welcome, playerName, to the city of Oggok."
        if (class == "Necromancer") then
            multiDialogue = {
                "As a new necromancer, you have been invited to join the Temple of Greenblood. They are the necromancer's guild here in Oggok.",
                "Then you are ready to move on to your first quest. Speak to Asogwe Greth, she is the guildmaster standing before you."
            }
        elseif (class == "ShadowKnight") then
            multiDialogue = {
                "As a new shadowknight, you have been invited to join the Temple of Greenblood. They are the shadowknight's guild here in Oggok.",
                "Then you are ready to move on to your first quest. Speak to Greenblood Yurgat, he is the guildmaster standing before you."
            }
        elseif (class == "Shaman") then
            multiDialogue = {
                "As a new shaman, you have been invited to join the Chosen of Gunthak. They are the shaman's guild here in Oggok.",
                "Then you are ready to move on to your first quest. Speak to Gunthak, he is the guildmaster standing before you."
            }
        elseif (class == "Warrior") then
            multiDialogue = {
                "As a new warrior, you have been invited to join the Craknek Warriors. They are the warrior's guild here in Oggok.",
                "Then you are ready to move on to your first quest. Speak to Warlord Brogar, he is the guildmaster standing before you."
            }
        end
    elseif (race == "Troll") then
    --Troll
        npcDialogue = "Welcome to Grobb playerName, home of the mightiest trolls on all of Tunaria."
        if (class == "ShadowKnight") then
            multiDialogue = {
                "As a new shadowknight, you have been invited to join the Shadowknights of Nightkeep. They are the shadowknight's guild here in Grobb.",
                "Then you are ready to move on to your first quest. Speak to Underlord Solthe, he is the guildmaster standing before you."
            }
        elseif (class == "Shaman") then
            multiDialogue = {
                "As a new shaman, you have been invited to join the The Dark Ones. They are the shaman's guild here in Grobb.",
                "Then you are ready to move on to your first quest. Speak to Hierophant Koligo, he is the guildmaster standing before you."
            }
        elseif (class == "Warrior") then
            multiDialogue = {
                "As a new warrior, you have been invited to join the Da Bashers. They are the warrior's guild here in Grobb.",
                "Then you are ready to move on to your first quest. Speak to Warlord Jurglash, he is the guildmaster standing before you."
            }
        end
    end
    SetPlayerFlags(mySession, "NewPlayerIntro", "1")
    SendDialogue(mySession, npcDialogue, diagOptions)
    SendMultiDialogue(mySession, multiDialogue)
end
