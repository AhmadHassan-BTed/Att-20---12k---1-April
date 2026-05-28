local Util   = require("frontiers_forge.util")
local Player = require("frontiers_forge.player")

local function DisplayCharacterOverview()
    if not Util.IsInGame() then return end

    ImGui.SetNextWindowSize(600, 320, ImGuiCond.FirstUseEver)
    if ImGui.Begin("Character Overview") then

        -- ===== SUMMARY ROW =====
        ImGui.Text(string.format(
            "%s  |  Lv %d  |  HP %d / %d  |  PWR %d / %d  |  AC %d",
            Player.GetName(),
            Player.GetLevel(),
            Player.GetCurrentHp(),
            Player.GetMaxHp(),
            Player.GetCurrentPwr(),
            Player.GetMaxPwr(),
            Player.GetAc()
        ))

        ImGui.Separator()

        -- ===== THREE COLUMNS =====
        ImGui.Columns(3, "StatsBuffsExp", true)

        -- Make column 2 (Resists) tighter
        ImGui.SetColumnOffset(1, 150)
        ImGui.SetColumnOffset(2, 260)

        ---------------------------------------------------------
        -- COLUMN 1: BASE + TOTAL STATS
        ---------------------------------------------------------
        ImGui.Text(string.format("STR: %d (%d)", Player.GetTotalStr(), Player.GetBaseStr()))
        ImGui.Text(string.format("STA: %d (%d)", Player.GetTotalSta(), Player.GetBaseSta()))
        ImGui.Text(string.format("AGI: %d (%d)", Player.GetTotalAgi(), Player.GetBaseAgi()))
        ImGui.Text(string.format("DEX: %d (%d)", Player.GetTotalDex(), Player.GetBaseDex()))
        ImGui.Text(string.format("WIS: %d (%d)", Player.GetTotalWis(), Player.GetBaseWis()))
        ImGui.Text(string.format("INT: %d (%d)", Player.GetTotalInt(), Player.GetBaseInt()))
        ImGui.Text(string.format("CHA: %d (%d)", Player.GetTotalCha(), Player.GetBaseCha()))

        ImGui.NextColumn()

        ---------------------------------------------------------
        -- COLUMN 2: RESISTS
        ---------------------------------------------------------
        local baseresist = Player.GetBaseResist()

        ImGui.Text(string.format("FR: %d", (Player.GetFireResistBuff() + baseresist)))
        ImGui.Text(string.format("CR: %d", (Player.GetColdResistBuff() + baseresist)))
        ImGui.Text(string.format("LR: %d", (Player.GetLightningResistBuff() + baseresist)))
        ImGui.Text(string.format("AR: %d", (Player.GetArcaneResistBuff() + baseresist)))
        ImGui.Text(string.format("PR: %d", (Player.GetPoisonResistBuff() + baseresist)))
        ImGui.Text(string.format("DR: %d", (Player.GetDiseaseResistBuff() + baseresist)))

        ImGui.NextColumn()

        ---------------------------------------------------------
        -- COLUMN 3: EXPERIENCE
        ---------------------------------------------------------
        local level        = Player.GetLevel()
        local currentExp   = Player.GetExp()
        local expRequired  = Util.GetExpRequiredForLevel(level)
        local expRemaining = expRequired - currentExp
        local expDebt = Player.GetExpDebt()

        ImGui.Text("Current EXP: " .. currentExp)
        ImGui.Text("EXP to Level: " .. expRequired)
        ImGui.Text(string.format("Exp Remaining: %d (%d)", expRemaining, expDebt))

        local progress = 0
        if expRequired > 0 then
            progress = currentExp / expRequired
        end

        ImGui.Spacing()
        ImGui.ProgressBar(
            progress,
            -1,
            0,
            string.format("%.1f%%", progress * 100)
        )

        -- >>> ADDED: CMs under EXP bar <<<
        ImGui.Spacing()

        local CMs   = Player.GetCMs()
        local CMsSpent   = Player.GetCMsSpent()
        local CMPct = Player.GetCMPct()

        ImGui.Text(string.format("CM %%: %d / %d", CMs, CMsSpent))
        ImGui.Text(string.format("CM Pct: %.1f%%", CMPct))

        -- Reset layout
        ImGui.Columns(1)
    end

    ImGui.End()
end

DisplayCharacterOverview()
