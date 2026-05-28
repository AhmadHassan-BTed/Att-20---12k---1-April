-- SimpleInventoryHelper.lua
local Util      = require("frontiers_forge.util")
local Inventory = require("frontiers_forge.inventory")

local TOTAL_INVENTORY_SLOTS = 40

local function DisplaySimpleInventoryHelper()
    if not Util.IsInGame() then return end

    ImGui.SetNextWindowSize(300, 80, ImGuiCond.FirstUseEver)
    local open = true
    if ImGui.Begin("Simple Inventory Helper", open) then
        local tunar = (type(Inventory.GetTunar) == "function" and Inventory.GetTunar()) or 0
        local used  = (type(Inventory.SlotsUsed) == "function" and Inventory.SlotsUsed()) or 0

        ImGui.Text(string.format("Tunar: %s", tostring(tunar)))
        ImGui.Text(string.format(
            "Inventory Used: %d / %d",
            used,
            TOTAL_INVENTORY_SLOTS
        ))
    end
    ImGui.End()
end

DisplaySimpleInventoryHelper()
