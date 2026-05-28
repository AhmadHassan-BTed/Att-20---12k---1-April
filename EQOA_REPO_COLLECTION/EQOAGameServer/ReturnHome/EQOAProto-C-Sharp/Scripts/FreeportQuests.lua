local items = require('Scripts/items')
local quests = {
    --Mage(10) Human(0) Freeport(1) QuestLineNumber
    [10010] = {
        [0] = { log = "You must purchase an iron ring from Merchant Yulia, then return to Malsis." },
        [1] = { xp = 430 }
    },
    [10011] = {
        [0] = { log = "Go speak to Spiritmaster Alshan." },
        [1] = { log = "Go speak to Coachman Ronks at the Stable." },
        [2] = { log = "Return to Malsis." },
        [3] = { xp = 2200 }
    },
    [10012] = {
        [0] = { log = "Return two cracked ant pincers to Kellina" },
        [1] = {
            xp = 6900,
            rewards = { { item = items.SMOLDERING_AURA, qty = 1 } },
            requirements = { { items.CRACKED_ANT_PINCER, 2 } }
        }
    },
    [10013] = {
        [0] = {
            log = "Kellina needs a plain robe from Merchant Yulia, a silk cord from Merchant Yesam, and a ruined bat wing."
        },
        [1] = {
            xp = 17000,
            rewards = { { items.BLUE_ROBE, 1 } },
            requirements = { { items.PLAIN_ROBE, 1 }, { items.SILK_CORD, 1 }, { items.RUINED_BAT_WING, 1 } }
        }
    },
    [10014] = {
        [0] = {
            log = "Travel west from Freeport to find the highwaymen and retrieve from them the stolen goods. Return to Kellina."
        },
        [1] = {
            xp = 36500,
            rewards = { { items.MOTIVATE, 1 } },
            requirements = { { items.STOLEN_GOODS, 1 } }
        }
    },
    [10015] = {
        [0] = { log = "Bring Kellina a chichan eel venom sac. You will find it north of Freeport, along the river." },
        [1] = {
            xp = 157474,
            rewards = { { 8446, 1 }, { 8447, 1 } },
            requirements = { { 8332, 1 } }
        }
    },
    [10016] = {
        [0] = { log = "Guard Sareken is in need of assistance. Please find him at the north entrance of Freeport." },
        [1] = { log = "Return to Guard Sareken with 147 tunar and three tough pike scales." },
        [2] = {
            log = "Go investigate the old house north of Freeport for the orc ransacker, destroy it and bring Guard Sareken its club as proof."
        },
        [3] = { xp = 556753 }
    },
    [10017] = {
        [0] = { log = "See Ilenar in the Shining Shield guild hall." },
        [1] = {
            log = "Travel north along the river until you reach Bobble-by-Water. Find Delwin Stitchfinger, and have him repair the robe."
        },
        [2] = { log = "Go see Grocer Fritz, purchase some fine chocolate and return to Delwin Stitchfinger." },
        [3] = { log = "Return to Delwin after a little while" },
        [4] = { log = "Return the robe to Ilenar Crelwin." },
        [5] = {
            log = "Give the special poisoned chocolate to Delwin Stitchfinger. Be sure to let him know Ilenar says thanks."
        },
        [6] = { log = "Return to Ilenar Crelwin. Tell him of Delwin's fate." },
        [7] = { xp = 550698 }
    },
    [10018] = {
        [0] = { log = "Journey to Temby and arrange for nightworm roots through Dagget Klem." },
        [1] = { log = "Kill a bloodfin mother, bring Dagget Klem one of its teeth as proof and 260 tunar." },
        [2] = { log = "Return to Dagget Klem to buy your case of nightworm root." },
        [3] = { log = "Return to Ilenar Crelwin." },
        [4] = { log = "Find madmen at ruins south of Freeport. Return to Ilenar Crelwin with some of their blood." },
        [5] = { log = "Return to Kellina." },
        [6] = { xp = 883791 }
    },
    --Enchanter(12) Human(0) Freeport(1)
    [12010] = {
        [0] = { log = "You must purchase Bronze Ring from Merchant Yulia, then return to Azlynn." },
        [1] = { xp = 430 }
    },
    [12011] = {
        [0] = { log = "Go speak to Spiritmaster Keika." },
        [1] = { log = "Go speak to Coachman Ronks at the Stable." },
        [2] = { log = "Return to Corious Slaerin." },
        [3] = { xp = 2200 }
    },
    [12012] = {
        [0] = { log = "Return two cracked ant pincers to Kellina" },
        [1] = {
            xp = 6900,
            rewards = { { item = 8373, qty = 1 } },
            requirements = { { 4866, 2 } }
        }
    },
    [12013] = {
        [0] = {
            log = "Kellina needs a plain robe from Merchant Yulia, a silk cord from Merchant Yesam, and a ruined bat wing."
        },
        [1] = {
            xp = 17000,
            rewards = { { 4927, 1 } },
            requirements = { { 5002, 1 }, { 8314, 1 }, { 4891, 1 } }
        }
    },
    [12014] = {
        [0] = {
            log = "Travel west from Freeport to find the highwaymen and retrieve from them the stolen goods. Return to Kellina."
        },
        [1] = {
            xp = 36500,
            rewards = { { 8445, 1 } },
            requirements = { { 8321, 1 } }
        }
    },
    [12015] = {
        [0] = { log = "Bring Kellina a chichan eel venom sac. You will find it north of Freeport, along the river." },
        [1] = {
            xp = 157474,
            rewards = { { 8446, 1 }, { 8447, 1 } },
            requirements = { { 8332, 1 } }
        }
    },
    [12016] = {
        [0] = { log = "Guard Sareken is in need of assistance. Please find him at the north entrance of Freeport." },
        [1] = { log = "Return to Guard Sareken with 147 tunar and three tough pike scales." },
        [2] = {
            log = "Go investigate the old house north of Freeport for the orc ransacker, destroy it and bring Guard Sareken its club as proof."
        },
        [3] = { xp = 556753 }
    },
    [12017] = {
        [0] = { log = "See Ilenar in the Shining Shield guild hall." },
        [1] = {
            log = "Travel north along the river until you reach Bobble-by-Water. Find Delwin Stitchfinger, and have him repair the robe."
        },
        [2] = { log = "Go see Grocer Fritz, purchase some fine chocolate and return to Delwin Stitchfinger." },
        [3] = { log = "Return to Delwin after a little while" },
        [4] = { log = "Return the robe to Ilenar Crelwin." },
        [5] = {
            log = "Give the special poisoned chocolate to Delwin Stitchfinger. Be sure to let him know Ilenar says thanks."
        },
        [6] = { log = "Return to Ilenar Crelwin. Tell him of Delwin's fate." },
        [7] = { xp = 550698 }
    },
    [12018] = {
        [0] = { log = "Journey to Temby and arrange for nightworm roots through Dagget Klem." },
        [1] = { log = "Kill a bloodfin mother, bring Dagget Klem one of its teeth as proof and 260 tunar." },
        [2] = { log = "Return to Dagget Klem to buy your case of nightworm root." },
        [3] = { log = "Return to Ilenar Crelwin." },
        [4] = { log = "Find madmen at ruins south of Freeport. Return to Ilenar Crelwin with some of their blood." },
        [5] = { log = "Return to Kellina." },
        [6] = { xp = 883791 }
    },
    --Necromancer(11) Human(0) Freeport(1)
    [11010] = {
        [0] = { log = "You must purchase Bone Earring from Merchant Gilgash, then return to Corious Slaerin." },
        [1] = { xp = 430 }
    },
    [11011] = {
        [0] = { log = "Go speak to Spiritmaster Keika." },
        [1] = { log = "Go speak to Coachman Ronks at the Stable." },
        [2] = { log = "Return to Corious Slaerin." },
        [3] = { xp = 2200 }
    },
    [11012] = {
        [0] = { log = "Return two damaged dragonfly wings to Rathei Slaerin." },
        [1] = {
            xp = 6900,
            rewards = { { item = 8532, qty = 1 } },
            requirements = { { 4873, 2 } }
        }
    },
    [11013] = {
        [0] = {
            log = "Rathei Slaerin needs a plain robe from Merchant Yulia, a silk cord from Merchant Yesam, and a sliver of snake meat"
        },
        [1] = {
            xp = 17000,
            rewards = { { 4922, 1 } },
            requirements = { { 5002, 1 }, { 8314, 1 }, { 4879, 1 } }
        }
    },
    [11014] = {
        [0] = {
            log = "Travel north from the north gate to the grassy area. Find the highwaymen and retrieve from them the stolen goods. Return to Rathei Slaerin."
        },
        [1] = {
            xp = 36500,
            rewards = { { 8533, 1 } },
            requirements = { { 8321, 1 } }
        }
    },
    [11015] = {
        [0] = { log = "Find Janxt in Temby to the north. Bring Corious Slaerin the cracked symbol of Tunare." },
        [1] = {
            xp = 157474,
            rewards = { { 8534, 1 }, { 8525, 1 } },
            requirements = { { 8524, 1 } }
        }
    },
    [11016] = {
        [0] = { log = "Follow the riverbank north of Freeport until you reach a broken bridge. Wait until nightfall and slay Charlik Novandear. Bring Corious his warm heart as proof." },
        [1] = { requirements = { { 8526, 1 } },
            log = "Purchase bottle of lantern oil from Merchant Landi and return to Corious Slaerin." },
        [2] = { xp = 556753,
            requirements = { { 8431, 1 } },
            rewards = { { 8528, 1 } }
        }
    },
    [11017] = {
        [0] = { log = "Go see  Agent Wilkenson. He is on the docks. " },
        [1] = { requirement = { { 8433, 1 } },
            log = "Go find Duminven. Look for him at Saerk's Tower." },
        [2] = { log = "Go see Grocer Fritz, purchase some fine chocolate and return to Delwin Stitchfinger." },
        [3] = { log = "Return to Delwin after a little while" },
        [4] = { log = "Return the robe to Ilenar Crelwin." },
        [5] = {
            log = "Give the special poisoned chocolate to Delwin Stitchfinger. Be sure to let him know Ilenar says thanks."
        },
        [6] = { log = "Return to Ilenar Crelwin. Tell him of Delwin's fate." },
        [7] = { xp = 550698 }
    },
    [11018] = {
        [0] = { log = "Journey to Temby and arrange for nightworm roots through Dagget Klem." },
        [1] = { log = "Kill a bloodfin mother, bring Dagget Klem one of its teeth as proof and 260 tunar." },
        [2] = { log = "Return to Dagget Klem to buy your case of nightworm root." },
        [3] = { log = "Return to Ilenar Crelwin." },
        [4] = { log = "Find madmen at ruins south of Freeport. Return to Ilenar Crelwin with some of their blood." },
        [5] = { log = "Return to Kellina." },
        [6] = { xp = 883791 }
    },
}

return quests
