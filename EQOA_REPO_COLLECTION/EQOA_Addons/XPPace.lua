local Util   = require("frontiers_forge.util")
local Player = require("frontiers_forge.player")

-- ========= Config =========
local SMOOTHING_ENABLED = true
local SMOOTHING_ALPHA   = 0.15
local WARMUP_SECONDS    = 20
local WARMUP_KILLS      = 5

-- ========= Persistent state =========
_G.__XP_PACE_STATE = _G.__XP_PACE_STATE or {
    lastExp        = nil,
    lastLevel      = nil,

    totalXp        = 0,
    totalKills     = 0,

    activeSeconds  = 0,
    lastTickTime   = nil,
    paused         = false,

    smoothXpPerMin  = nil,
    smoothXpPerKill = nil,

    lastDrawFrame  = nil,
    __frameCounter = 0,
}
local S = _G.__XP_PACE_STATE

local function nowSec()
    return os.time()
end

local function resetSession()
    S.totalXp         = 0
    S.totalKills      = 0
    S.activeSeconds   = 0
    S.lastTickTime    = nowSec()
    S.smoothXpPerMin  = nil
    S.smoothXpPerKill = nil
end

local function formatTime(seconds)
    if seconds == nil or seconds < 0 then return "N/A" end

    local s = math.floor(seconds + 0.5)
    local h = math.floor(s / 3600); s = s - h * 3600
    local m = math.floor(s / 60);   s = s - m * 60

    if h > 0 then
        return string.format("%dh %dm %ds", h, m, s)
    end
    return string.format("%dm %ds", m, s)
end

local function ema(prev, value, alpha)
    if prev == nil then return value end
    return prev + alpha * (value - prev)
end

local function DisplayXPPaceBox()
    if not Util.IsInGame() then return end

    -- Prevent double-draw
    local frame
    if ImGui.GetFrameCount then
        frame = ImGui.GetFrameCount()
    else
        S.__frameCounter = S.__frameCounter + 1
        frame = S.__frameCounter
    end
    if S.lastDrawFrame == frame then return end
    S.lastDrawFrame = frame

    local tNow = nowSec()
    local level      = Player.GetLevel()
    local currentExp = Player.GetExp()

    -- Init
    if S.lastExp == nil then
        S.lastExp      = currentExp
        S.lastLevel    = level
        S.lastTickTime = tNow
        resetSession()
    end

    -- Level-up / rollover reset
    if S.lastLevel ~= level or currentExp < S.lastExp then
        S.lastLevel = level
        S.lastExp   = currentExp
        S.paused    = false
        resetSession()
    end

    -- Active time (exclude pauses)
    local dt = tNow - (S.lastTickTime or tNow)
    if dt < 0 then dt = 0 end
    if dt > 5 then dt = 5 end
    S.lastTickTime = tNow

    if not S.paused then
        S.activeSeconds = S.activeSeconds + dt
    end

    -- XP tracking
    local dx = currentExp - S.lastExp
    if dx < 0 then dx = 0 end

    if not S.paused and dx > 0 then
        S.totalXp    = S.totalXp + dx
        S.totalKills = S.totalKills + 1
    end
    S.lastExp = currentExp

    -- Session averages
    local instXpPerMin  = nil
    local instXpPerKill = nil

    if S.activeSeconds > 0 and S.totalXp > 0 then
        instXpPerMin = (S.totalXp / S.activeSeconds) * 60
    end
    if S.totalKills > 0 then
        instXpPerKill = S.totalXp / S.totalKills
    end

    -- Smoothed values
    local xpPerMin  = instXpPerMin
    local xpPerKill = instXpPerKill
    if SMOOTHING_ENABLED then
        if instXpPerMin then
            S.smoothXpPerMin = ema(S.smoothXpPerMin, instXpPerMin, SMOOTHING_ALPHA)
            xpPerMin = S.smoothXpPerMin
        end
        if instXpPerKill then
            S.smoothXpPerKill = ema(S.smoothXpPerKill, instXpPerKill, SMOOTHING_ALPHA)
            xpPerKill = S.smoothXpPerKill
        end
    end

    local warmedUp = (S.activeSeconds >= WARMUP_SECONDS) and (S.totalKills >= WARMUP_KILLS)

    -- Level math
    local expRequired  = Util.GetExpRequiredForLevel(level)
    local expRemaining = expRequired - currentExp
    if expRemaining < 0 then expRemaining = 0 end

    local pctRemaining = 0
    if expRequired > 0 then
        pctRemaining = (expRemaining / expRequired) * 100
        if pctRemaining < 0 then pctRemaining = 0 end
        if pctRemaining > 100 then pctRemaining = 100 end
    end

    -- Estimates
    local secondsToLevel = nil
    if expRemaining == 0 then
        secondsToLevel = 0
    elseif warmedUp and xpPerMin and xpPerMin > 0 then
        secondsToLevel = (expRemaining / xpPerMin) * 60
    end

    local killsToLevel = nil
    if expRemaining == 0 then
        killsToLevel = 0
    elseif warmedUp and xpPerKill and xpPerKill > 0 then
        killsToLevel = math.ceil(expRemaining / xpPerKill)
    end

    -- ========= UI =========
    ImGui.SetNextWindowSize(360, 130, ImGuiCond.FirstUseEver)
    if ImGui.Begin("XP Pace##xp_pace") then
        if ImGui.Button("Reset") then
            resetSession()
        end
        ImGui.SameLine()
        local pauseLabel = S.paused and "Resume" or "Pause"
        if ImGui.Button(pauseLabel) then
            S.paused = not S.paused
            S.lastTickTime = nowSec()
        end
        ImGui.SameLine()
        ImGui.Text(S.paused and "(Paused)" or "")

        ImGui.Separator()

        if xpPerMin then
            ImGui.Text(string.format("Avg XP / minute: %.1f", xpPerMin))
        else
            ImGui.Text("Avg XP / minute: N/A")
        end

        if xpPerKill then
            ImGui.Text(string.format("Avg XP / kill: %.1f", xpPerKill))
        else
            ImGui.Text("Avg XP / kill: N/A")
        end

        if not warmedUp and expRemaining > 0 then
            ImGui.Text(string.format(
                "Warming up... (%ds/%ds, %d/%d kills)",
                math.floor(S.activeSeconds), WARMUP_SECONDS,
                S.totalKills, WARMUP_KILLS
            ))
        end

        if killsToLevel ~= nil then
            ImGui.Text(string.format(
                "Est. kills to level (%.1f%%): %d",
                pctRemaining,
                killsToLevel
            ))
        else
            ImGui.Text("Est. kills to level: N/A")
        end

        ImGui.Text("Est. time to level: " .. formatTime(secondsToLevel))
    end
    ImGui.End()
end

DisplayXPPaceBox()
