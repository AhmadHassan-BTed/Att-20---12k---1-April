local Util      = require("frontiers_forge.util")
local Player    = require("frontiers_forge.player")
local EntityList = require("frontiers_forge.entity_list") -- use whatever your EntityList module path is

local function DisplayTargetLevel()
    if not Util.IsInGame() then return end

    -- Small window like your other UIForge boxes
    ImGui.SetNextWindowSize(220, 80, ImGuiCond.FirstUseEver)
    if ImGui.Begin("Target Level") then
        local entity = EntityList.GetEntityById(Player.GetTargetEntityId())
        if entity == nil then
            ImGui.Text("Target: (none)")
            ImGui.Text("Level: --")
        else
            ImGui.Text("Target: " .. entity.name)
            ImGui.Separator()
            ImGui.Text("Level: " .. tostring(entity.level))
        end
    end
    ImGui.End()
end

DisplayTargetLevel()
