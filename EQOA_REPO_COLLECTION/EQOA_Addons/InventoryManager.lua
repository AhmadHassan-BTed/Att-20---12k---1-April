local Util = require("frontiers_forge.util")
local Inventory  = require("frontiers_forge.inventory")
local Bank = require("frontiers_forge.bank")
local Gems = require("frontiers_forge.gems")


if Util.IsInGame() == 0 then return end

-- Bank slot id -> label
local BANK_SLOT_LABEL = {
    [1]="Head",[2]="Robe",[3]="Earring",[4]="Neck",[5]="Torso",
    [6]="Bracelet",[7]="2 Forearm",[8]="Ring",[9]="Waist",[10]="Legs",
    [11]="Boots",[12]="Weapon",[13]="Shield",[14]="Weapon",[15]="Weapon",
    [16]="Weapon",[17]="Weapon",[18]="Weapon",[19]="Gloves",
}

-- Rarity color coding:
-- Green for common, blue for uncommon, purple for rare, orange for ultra rare
-- (Used by Gems module; words like "Common", "Uncommon", etc.)
local RARITY_COLOR = {
    ["Common"]     = { 0.2, 1.0, 0.2, 1.0 },
    ["Uncommon"]   = { 0.3, 0.6, 1.0, 1.0 },
    ["Rare"]       = { 0.7, 0.4, 1.0, 1.0 },
    ["Ultra Rare"] = { 1.0, 0.6, 0.2, 1.0 },
}

-- Gear rarity color coding (from GearLog.txt codes)
-- C (common) = green, UC (uncommon) = blue, R (rare) = purple, UR (ultra rare) = orange
local GEAR_RARITY_COLOR = {
    ["C"]  = { 0.2, 1.0, 0.2, 1.0 },
    ["UC"] = { 0.3, 0.6, 1.0, 1.0 },
    ["R"]  = { 0.7, 0.4, 1.0, 1.0 },
    ["UR"] = { 1.0, 0.6, 0.2, 1.0 },
}

-- =========================
-- GearLog (UIForge root)
-- =========================
local function safe_trim(s)
    if not s then return "" end
    return (tostring(s):gsub("^%s+", ""):gsub("%s+$", ""))
end

local function file_exists(path)
    local f = io.open(path, "r")
    if f then f:close(); return true end
    return false
end

local function get_script_dir()
    -- Try to locate the directory of this script file (works in most Lua environments)
    local ok, info = pcall(function() return debug and debug.getinfo and debug.getinfo(1, "S") end)
    if ok and info and info.source then
        local src = tostring(info.source)
        -- strip leading '@' if present
        if src:sub(1,1) == "@" then src = src:sub(2) end
        -- normalize slashes
        src = src:gsub("\\", "/")
        -- directory
        local dir = src:match("^(.*)/[^/]*$")
        if dir and dir ~= "" then
            return dir
        end
    end
    return ""
end


local function get_gearlog_path()
    -- GearLog.txt lives in the UIForge root (not inside PCSX2 folder structure).

    -- OPTIONAL: If you want to hard-pin the path, set this to an absolute path.
    -- Example: local GEARLOG_ABS = "C:\\Path\\To\\UIForge\\GearLog.txt"
    local GEARLOG_ABS = ""

    if GEARLOG_ABS ~= "" and file_exists(GEARLOG_ABS) then
        return GEARLOG_ABS
    end

    local candidates = {}

    -- Prefer UIForge root if available.
    local ok_root, root = pcall(function()
        if Util and Util.GetUIForgeRoot then return Util.GetUIForgeRoot() end
        return nil
    end)

    if ok_root and root and tostring(root) ~= "" then
        local r = tostring(root)
        if r:sub(-1) == "\\" or r:sub(-1) == "/" then
            table.insert(candidates, r .. "GearLog.txt")
        else
            table.insert(candidates, r .. "/GearLog.txt")
            table.insert(candidates, r .. "\\GearLog.txt")
        end
    end

    -- Also try alongside this script (common in UIForge setups)
    local sd = get_script_dir()
    if sd ~= "" then
        table.insert(candidates, sd .. "/GearLog.txt")
        table.insert(candidates, sd .. "/../GearLog.txt")
        table.insert(candidates, sd .. "/../../GearLog.txt")
    end

    -- Relative fallbacks (current working dir)
    table.insert(candidates, "GearLog.txt")
    table.insert(candidates, "./GearLog.txt")
    table.insert(candidates, "../GearLog.txt")

    for _, p in ipairs(candidates) do
        if file_exists(p) then return p end
    end

    -- If none exist yet, return the preferred default path (UIForge root if known, else next to script, else relative)
    if ok_root and root and tostring(root) ~= "" then
        local r = tostring(root)
        if r:sub(-1) == "\\" or r:sub(-1) == "/" then
            return r .. "GearLog.txt"
        end
        return r .. "/GearLog.txt"
    end
    if sd ~= "" then
        return sd .. "/GearLog.txt"
    end
    return "GearLog.txt"
end

local function parse_delimited_line(line, sep)
    local out = {}
    local i, len = 1, #line
    local field = ""
    local in_quotes = false

    while i <= len do
        local ch = line:sub(i,i)
        if ch == '"' then
            if in_quotes and line:sub(i+1,i+1) == '"' then
                field = field .. '"'
                i = i + 1
            else
                in_quotes = not in_quotes
            end
        elseif ch == sep and not in_quotes then
            table.insert(out, safe_trim(field))
            field = ""
        else
            field = field .. ch
        end
        i = i + 1
    end
    table.insert(out, safe_trim(field))
    return out
end

local function load_gearlog()
    local path = get_gearlog_path()
    local f = io.open(path, "r")
    if not f then
        return {}, path, false
    end

    local header = nil
    local sep = nil
    local gear = {}

    local first = f:read("*l")
    if not first then
        f:close()
        return {}, path, true
    end

    -- Detect delimiter (tab preferred if present; else comma)
    if first:find("\t") then sep = "\t" else sep = "," end
    header = parse_delimited_line(first, sep)

    -- Build header index
    local idx = {}
    for i, h in ipairs(header) do
        idx[string.lower(h)] = i
    end

    -- Required columns (we key by Name + Level)
    local name_i = idx["name"]
    local level_i = idx["level"]
    local rarity_i = idx["rarity"]

    -- If it doesn't look like a header row, treat as pipe-style log format:
    -- Name|Level|Rarity|AC: 10 | HP: 40 | ...
    if not name_i or not level_i then
        -- reset and parse this as first data line with pipe style
        local function parse_pipe_line(line)
            local parts = {}
            for part in line:gmatch("([^|]+)") do
                table.insert(parts, safe_trim(part))
            end
            return parts
        end

        local function ingest_pipe(line)
            local parts = parse_pipe_line(line)
            local name = parts[1] or ""
            local lvl  = tonumber(parts[2] or "") or 0
            local rar  = (parts[3] or ""):upper()
            if rar == "U" then rar = "UC" end
            if name ~= "" and lvl > 0 then
                local stats = {}
                local source = ""
                for i = 4, #parts do
                    local tok = parts[i]
                    if tok and tok ~= "" then
                        local b = tok:match("^%((.+)%)$")
                        if b and safe_trim(b) ~= "" then
                            source = safe_trim(b)
                        else
                            table.insert(stats, tok)
                        end
                    end
                end
                if source ~= "" then
                    table.insert(stats, ("(%s)"):format(source))
                end
                gear[name .. "|" .. tostring(lvl)] = {
                    rarity = rar,
                    source  = source,
                    stats  = table.concat(stats, " | "),
                }
            end
        end

        ingest_pipe(first)
        for line in f:lines() do ingest_pipe(line) end
        f:close()
        return gear, path, true
    end

    -- For CSV/tab exports (like a saved copy of your spreadsheet),
    -- we build the stats list from known stat columns if present.
    local stat_order = { "AC","Damage","HP","PWR","POW","HoT","PoT","STA","STR","AGI","DEX","WIS","INT","CHA","FR","CR","LR","AR","PR","DR" }

    local source_i = idx["source"] or idx["biome"]

    for line in f:lines() do
        if line and safe_trim(line) ~= "" then
            local row = parse_delimited_line(line, sep)
            local name = row[name_i] or ""
            local lvl  = tonumber(row[level_i] or "") or 0
            if name ~= "" and lvl > 0 then
                local rar = ""
                if rarity_i and row[rarity_i] then
                    rar = safe_trim(row[rarity_i]):upper()
                end

                local stats = {}
                local source = ""
                if source_i and row[source_i] then source = safe_trim(row[source_i]) end

                for _, col in ipairs(stat_order) do
                    local ci = idx[string.lower(col)]
                    if ci and row[ci] and safe_trim(row[ci]) ~= "" then
                        local v = safe_trim(row[ci])
                        if v ~= "0" and v ~= "0.0" then
                            local label = col
                            if col == "POW" then label = "PWR" end
                            table.insert(stats, ("%s: %s"):format(label, v))
                        end
                    end
                end

                                if source ~= "" then table.insert(stats, ("(%s)"):format(source)) end

                gear[name .. "|" .. tostring(lvl)] = {
                    rarity = rar,
                    source  = source,
                    stats  = table.concat(stats, " | "),
                }
            end
        end
    end

    f:close()
    return gear, path, true
end

local function is_empty_name(name)
    return (not name) or name == "" or name == "(unnamed)" or name == "(unnamed item)"
end

local function fmt_name_amt(name, amt)
    if amt and amt > 1 then
        return ("%s (%d)"):format(name, amt)
    end
    return name
end

local function eq_display(name, amt, level, slot_label)
    return ("%s (Level %d) - %s")
        :format(fmt_name_amt(name, amt), level or 0, slot_label or "")
end

local function gearlog_key(name, level)
    return (name or "") .. "|" .. tostring(level or 0)
end

local function eq_display_with_gearlog(name, amt, level, slot_label, gearlog)
    local base = eq_display(name, amt, level, slot_label)

    if not gearlog then return { text = base } end
    local rec = gearlog[gearlog_key(name, level)]
    if not rec then return { text = base } end

    local stats = rec.stats or ""
    if stats ~= "" then
        base = base .. " | " .. stats
    end

    -- Ensure (Source) is displayed even if stats string omitted it
    local source = rec.source or ""
    if source ~= "" then
        local has = tostring(stats or ""):find("%(" .. source:gsub("([%^%$%(%)%%%.%[%]%*%+%-%?])","%%%1") .. "%)")
        if not has then
            base = base .. " | (" .. source .. ")"
        end
    end

    return { text = base, rarity = rec.rarity or "" }
end

local function sort_ci(a, b)
    return string.lower(a) < string.lower(b)
end

local function bank_is_gear(it)
    local sid = it.slot or 0
    return sid >= 1 and sid <= 19
end

local function inv_is_equippable_slot(slot_id)
    return slot_id and slot_id >= 1 and slot_id <= 19
end

local function build_gem_entry(name, amt)
    local g = Gems.Get(name)
    if not g then return nil end
    local text = ("%s - %s - %s"):format(
        fmt_name_amt(name, amt),
        g.stat or "",
        g.type or ""
    )
    return { text = text, rarity = g.rarity or "" }
end

-- =========================
-- Cached state (only rebuild on refresh)
-- =========================
local state = state or {}

-- =========================
-- Tracked Items (persisted)
-- =========================
_G.__FF_TRACKED_ITEMS = _G.__FF_TRACKED_ITEMS or { open = false, items = {} }
local tracked = _G.__FF_TRACKED_ITEMS

local function tracked_is_tracked(key)
    return tracked.items[key] ~= nil
end

local function tracked_base_name_from_text(s)
    s = safe_trim(tostring(s or ""))
    -- remove numbering "12. "
    s = s:gsub("^%s*%d+%s*%.%s*", "")
    -- remove trailing "(123)" (amount) or "x123"
    local n = s:match("^(.-)%s*%((%d+)%)%s*$")
    if n then return safe_trim(n) end
    n = s:match("^(.-)%s*[xX]%s*(%d+)%s*$")
    if n then return safe_trim(n) end
    -- remove trailing "(LV 12)" / "(Lvl 12)"
    n = s:match("^(.-)%s*%((?:LV|Lvl)%s*%d+%)%s*$")
    if n then return safe_trim(n) end
    return s
end

local function tracked_toggle(key, display_name, meta)
    if tracked.items[key] then
        tracked.items[key] = nil
    else
        tracked.items[key] = {
            key = key,
            name = display_name or key,
            kind = (meta and meta.kind) or "item",
        }
        tracked.open = true
    end
end

local function tracked_amount(rec)
    if not rec then return 0 end
    local total = 0

    -- Pull live data from the same modules InventoryManager uses
    local inv = Inventory.GetItems() or {}
    for _, it in ipairs(inv) do
        if it.name == rec.key then
            total = total + (tonumber(it.amount) or 0)
        end
    end

    local bank_items = Bank.GetItems() or {}
    for _, it in ipairs(bank_items) do
        if it.name == rec.key then
            total = total + (tonumber(it.amount) or 0)
        end
    end

    return total
end

local function render_tracked_window()
    local has_any = false
    for _ in pairs(tracked.items) do has_any = true break end
    if not has_any then
        tracked.open = false
        return
    end
    if not tracked.open then return end

    ImGui.SetNextWindowSize(360, 260)
    if ImGui.Begin("Tracked Items") then
        -- Slots used (like main window)
        ImGui.Text(("Inventory Slots Used: %d / 40"):format(Inventory.SlotsUsed() or 0))
        ImGui.Text(("Bank Slots Used: %d / 40"):format(Bank.SlotsUsed() or 0))
        ImGui.Separator()

        ImGui.Columns(2, "tracked_cols", false)
        ImGui.SetColumnWidth(1, 240)

        ImGui.Text("Name"); ImGui.NextColumn()
        ImGui.Text("Amount"); ImGui.NextColumn()
        ImGui.Separator()

        local keys = {}
        for k in pairs(tracked.items) do table.insert(keys, k) end
        table.sort(keys, function(a,b) return string.lower(a) < string.lower(b) end)

        for _, k in ipairs(keys) do
            local rec = tracked.items[k]
            ImGui.Text(rec.name); ImGui.NextColumn()
            ImGui.Text(tostring(tracked_amount(rec))); ImGui.NextColumn()
        end

        ImGui.Columns(1)
    end
    ImGui.End()
end

local function render_item_row_with_track(state, key, text, color)
    ImGui.Columns(2, "itemrow_" .. tostring(key), false)
    ImGui.SetColumnWidth(1, 90)
    if color then
        ImGui.TextColored(color[1], color[2], color[3], color[4], text)
    else
        ImGui.Text(text)
    end
    ImGui.NextColumn()
    local tracked_now = tracked_is_tracked(key)
    local btn = tracked_now and ("Untrack##" .. tostring(key)) or ("Track##" .. tostring(key))
    if ImGui.Button(btn) then
        tracked_toggle(key, text)
    end
    ImGui.NextColumn()
    ImGui.Columns(1)
end

state.built = state.built or false
state.last_refresh = state.last_refresh or ""
state.inv_used = state.inv_used or 0
state.bank_used = state.bank_used or 0

state.inv_equipped = state.inv_equipped or {}
state.inv_gems = state.inv_gems or {}
state.inv_other_gear = state.inv_other_gear or {}
state.inv_other_plain = state.inv_other_plain or {}

state.bank_gear = state.bank_gear or {}
state.bank_gems = state.bank_gems or {}
state.bank_other = state.bank_other or {}

state.gearlog = state.gearlog or {}
state.gearlog_path = state.gearlog_path or ""
state.gearlog_ok = state.gearlog_ok or false


local function now_string()
    -- os.date is usually available; if not, we’ll just show blank.
    if os and os.date then
        return os.date("%Y-%m-%d %H:%M:%S")
    end
    return ""
end

local function clamp_used(n)
    n = n or 0
    if n < 0 then n = 0 end
    if n > 40 then n = 40 end
    return n
end

local function rebuild()
    -- reset lists
    state.inv_equipped = {}
    state.inv_gems = {}
    state.inv_other_gear = {}
    state.inv_other_plain = {}
    state.bank_gear = {}
    state.bank_gems = {}
    state.bank_other = {}

    -- Load GearLog (cached per refresh)
    state.gearlog, state.gearlog_path, state.gearlog_ok = load_gearlog()


    -- counts
    local inv_used = Inventory.SlotsUsed and Inventory.SlotsUsed() or 0
    state.inv_used = clamp_used(inv_used)

    local bank_used = Bank.SlotsUsed and Bank.SlotsUsed() or 0
    state.bank_used = clamp_used(bank_used)

    -- inventory items via API
    local inv_items = Inventory.GetItems and Inventory.GetItems() or {}
    for _, it in ipairs(inv_items) do
        local name = it.name or ""
        local amt  = it.amount or 0

        if not is_empty_name(name) then
            local slot_id = it.slot or 0
            local level   = it.level_req or it.level or 0
            local equipped = false

            -- Prefer API field if present; fall back to status string if that’s what you expose.
            if it.equipped ~= nil then
                equipped = (it.equipped == true)
            elseif it.equipped_status then
                equipped = (it.equipped_status == "Equipped")
            end

            if equipped then
                local slot_label, order_key = Inventory.GetSlotLabelAndOrder(slot_id)
                local disp = eq_display_with_gearlog(name, amt, level, slot_label, state.gearlog)
                table.insert(state.inv_equipped, {
                    text   = disp.text,
                    rarity = disp.rarity,
                    order  = order_key,
                    slot   = slot_label,
                    idx    = it.idx or 0,
                    name   = name,
                    level  = level,
                    loc    = "Inventory (Equipped)",
                })

            elseif Gems.IsGem(name) then
                local entry = build_gem_entry(name, amt)
                if entry then table.insert(state.inv_gems, entry) end

            else
                if inv_is_equippable_slot(slot_id) then
                    local slot_label, order_key = Inventory.GetSlotLabelAndOrder(slot_id)
                    local disp = eq_display_with_gearlog(name, amt, level, slot_label, state.gearlog)
                    table.insert(state.inv_other_gear, {
                        text   = disp.text,
                        rarity = disp.rarity,
                        order  = order_key,
                        slot   = slot_label,
                        idx    = it.idx or 0,
                        name   = name,
                        level  = level,
                        loc    = "Inventory",
                    })
                else
                    table.insert(state.inv_other_plain, fmt_name_amt(name, amt))
                end
            end
        end
    end

    -- Sort inventory equipped
    table.sort(state.inv_equipped, function(a,b)
        if a.order ~= b.order then return a.order < b.order end
        if a.slot ~= b.slot then return a.slot < b.slot end
        return (a.idx or 0) < (b.idx or 0)
    end)

    -- Sort inventory other-gear
    table.sort(state.inv_other_gear, function(a,b)
        if a.order ~= b.order then return a.order < b.order end
        if a.slot ~= b.slot then return a.slot < b.slot end
        return (a.idx or 0) < (b.idx or 0)
    end)

    -- Sort inventory gems + other plain
    table.sort(state.inv_gems, function(a,b) return string.lower(a.text) < string.lower(b.text) end)
    table.sort(state.inv_other_plain, sort_ci)

    -- bank items via API
    for _, it in ipairs(Bank.GetItems() or {}) do
        local name, amt = it.name or "", it.amount or 0
        if not is_empty_name(name) then
            if bank_is_gear(it) then
                local sid = it.slot or 0
                local label = BANK_SLOT_LABEL[sid] or tostring(sid)

                local lvl = it.level or 0
                local disp = eq_display_with_gearlog(name, amt, lvl, label, state.gearlog)
                table.insert(state.bank_gear, {
                    text   = disp.text,
                    rarity = disp.rarity,
                    slot   = sid,
                    name   = name,
                    level  = lvl,
                    loc    = "Bank",
                })
            elseif Gems.IsGem(name) then
                local entry = build_gem_entry(name, amt)
                if entry then table.insert(state.bank_gems, entry) end
            else
                table.insert(state.bank_other, fmt_name_amt(name, amt))
            end
        end
    end

    table.sort(state.bank_gear, function(a,b)
        if a.slot ~= b.slot then return a.slot < b.slot end
        return string.lower(a.name) < string.lower(b.name)
    end)

    table.sort(state.bank_gems, function(a,b) return string.lower(a.text) < string.lower(b.text) end)
    table.sort(state.bank_other, sort_ci)

    state.last_refresh = now_string()
    state.built = true
end


-- =========================
-- Gear Stats Editor (inline)
-- =========================
_G.__FF_GEAR_STATS_EDITOR = _G.__FF_GEAR_STATS_EDITOR or {
    selected_idx = 1,
    last_key = "",
    message = "",
    message_color = {1,1,1,1},
    fields = {},
}

local _gse = _G.__FF_GEAR_STATS_EDITOR

local function gse_set_message(text, rgba)
    _gse.message = text or ""
    _gse.message_color = rgba or {1,1,1,1}
end

local function gse_clear_fields()
    for k,_ in pairs(_gse.fields) do _gse.fields[k] = "" end
    _gse.fields.RARITY = "C"
end

local function gse_ensure_fields()
    local defaults = {
        STR="", STA="", AGI="", DEX="", WIS="", INT="", CHA="",
        AR="", CR="", DR="", FR="", LR="", PR="",
        AC="", DAMAGE="", HP="", PWR="", HOT="", POT="",
        PROC="", SOURCE="", BIOME="", RARITY="C",
    }
    for k,v in pairs(defaults) do
        if _gse.fields[k] == nil then _gse.fields[k] = v end
    end
    if _gse.fields.SOURCE == "" and _gse.fields.BIOME ~= "" then _gse.fields.SOURCE = _gse.fields.BIOME end
    if _gse.fields.BIOME == "" and _gse.fields.SOURCE ~= "" then _gse.fields.BIOME = _gse.fields.SOURCE end

end

local function gse_parse_stats_string(stats)
    -- stats string is "AC: 10 | HP: 40 | Proc: Foo | (Arctic)"
    local out = { source = "", proc = "" }
    if not stats or stats == "" then return out end
    for tok in tostring(stats):gmatch("([^|]+)") do
        tok = safe_trim(tok)
        if tok ~= "" then
            local b = tok:match("^%((.+)%)$")
            if b and safe_trim(b) ~= "" then
                out.source = safe_trim(b)
            else
                local k, v = tok:match("^([^:]+):%s*(.+)$")
                if k and v then
                    k = safe_trim(k):upper()
                    v = safe_trim(v)
                    if k == "POW" then k = "PWR" end
                    if k == "PROC" then
                        out.proc = v
                    else
                        out[k] = v
                    end
                end
            end
        end
    end
    return out
end

local function gse_build_stats_tokens(fields)
    -- Canonical token order for saving
    local order = {
        "STR","STA","AGI","DEX","WIS","INT","CHA",
        "AR","CR","DR","FR","LR","PR",
        "AC","DAMAGE","HP","PWR","HOT","POT"
    }

    local tokens = {}
    for _, k in ipairs(order) do
        local v = safe_trim(fields[k] or "")
        if v ~= "" then
            local label = k
            if k == "DAMAGE" then label = "Damage" end
            if k == "HOT" then label = "HoT" end
            if k == "POT" then label = "PoT" end
            if k == "PWR" then label = "PWR" end
            table.insert(tokens, ("%s: %s"):format(label, v))
        end
    end

    local proc = safe_trim(fields.PROC or "")
    if proc ~= "" then
        table.insert(tokens, ("Proc: %s"):format(proc))
    end

    local source = safe_trim(fields.SOURCE or fields.BIOME or "")
    if source ~= "" then
        table.insert(tokens, ("(%s)"):format(source))
    end

    return tokens
end

local function gse_write_gearlog(path, gearlog)
    local f = io.open(path, "w")
    if not f then return false, "Unable to write GearLog.txt at: " .. tostring(path) end

    -- Sort by Name then Level
    local keys = {}
    for k,_ in pairs(gearlog or {}) do table.insert(keys, k) end
    table.sort(keys, function(a,b) return string.lower(a) < string.lower(b) end)

    for _, key in ipairs(keys) do
        local rec = gearlog[key]
        if rec and rec.name and rec.level then
            local parts = { rec.name, tostring(rec.level), rec.rarity or "" }

            if rec.stats and rec.stats ~= "" then
                for tok in tostring(rec.stats):gmatch("([^|]+)") do
                    tok = safe_trim(tok)
                    if tok ~= "" then table.insert(parts, tok) end
                end
            end

            -- Ensure (Source) token is present at end if we have it
            local source = rec.source or ""
            if source ~= "" then
                local has = tostring(rec.stats or ""):find("%(" .. source:gsub("([%^%$%(%)%%%.%[%]%*%+%-%?])","%%%1") .. "%)")
                if not has then
                    table.insert(parts, ("(%s)"):format(source))
                end
            end

            if parts[3] == "" then table.remove(parts, 3) end
            f:write(table.concat(parts, "|"))
            f:write("\n")
        end
    end
    f:close()
    return true
end

local function gse_build_item_options(state)
    local opts = {}
    local function add_from(list)
        for _, r in ipairs(list or {}) do
            if r.name and r.level and r.name ~= "" and tonumber(r.level) and tonumber(r.level) > 0 then
                local label = ("%s (Level %d) - %s"):format(r.name, tonumber(r.level), r.loc or "")
                table.insert(opts, {
                    label = label,
                    name  = r.name,
                    level = tonumber(r.level),
                    key   = r.name .. "|" .. tostring(tonumber(r.level)),
                })
            end
        end
    end
    add_from(state.inv_equipped)
    add_from(state.inv_other_gear)
    add_from(state.bank_gear)
    table.sort(opts, function(a,b) return string.lower(a.label) < string.lower(b.label) end)
    return opts
end

local function gse_prefill_from_gearlog(gearlog, item, do_clear)
    gse_ensure_fields()
    if do_clear then
        gse_clear_fields()
    end
    if not item then return end

    local rec = gearlog and gearlog[item.key] or nil
    if rec and rec.stats then
        local parsed = gse_parse_stats_string(rec.stats)
        _gse.fields.SOURCE = parsed.source or rec.source or ""
        _gse.fields.BIOME = _gse.fields.SOURCE
        _gse.fields.PROC = parsed.proc or parsed["PROC"] or ""
        _gse.fields.RARITY = safe_trim((rec.rarity or ""):upper())
        if _gse.fields.RARITY == "U" then _gse.fields.RARITY = "UC" end
        if _gse.fields.RARITY == "" then _gse.fields.RARITY = "C" end

        local map = {
            STR="STR", STA="STA", AGI="AGI", DEX="DEX", WIS="WIS", INT="INT", CHA="CHA",
            AR="AR", CR="CR", DR="DR", FR="FR", LR="LR", PR="PR",
            AC="AC", DAMAGE="DAMAGE", HP="HP", PWR="PWR", HOT="HOT", POT="POT",
        }

        for field_key, parsed_key in pairs(map) do
            if parsed[parsed_key] ~= nil then
                _gse.fields[field_key] = tostring(parsed[parsed_key])
            end
        end
    else
        -- No existing record: keep whatever is currently in the fields unless caller requested a clear.
        if do_clear then
            _gse.fields.RARITY = _gse.fields.RARITY or "C"
        end
    end
end

local function gse_input_text(label, field_key, width)
    -- UIForge ImGui.InputText returns the new string (no separate "changed" boolean).
    local v = tostring(_gse.fields[field_key] or "")

    ImGui.AlignTextToFramePadding()
    ImGui.Text(label)
    ImGui.SameLine()

    ImGui.PushItemWidth(width or 120)
    local newv = ImGui.InputText("##" .. tostring(field_key), v, 256)
    ImGui.PopItemWidth()

    if newv ~= v then
        _gse.fields[field_key] = newv
    end
end

local function gse_rarity_dropdown()
    local options = { "C", "UC", "R", "UR" }
    local current = (_gse.fields.RARITY or "C"):upper()
    if current == "U" then current = "UC" end
    local preview = current
    if ImGui.BeginCombo("Rarity", preview) then
        for _, opt in ipairs(options) do
            local selected = (opt == current)
            if ImGui.Selectable(opt, selected) then
                _gse.fields.RARITY = opt
            end
            if selected then ImGui.SetItemDefaultFocus() end
        end
        ImGui.EndCombo()
    end
end

local function render_gear_stats_editor(state)
    gse_ensure_fields()    ImGui.Text("Edit gear stats and save to GearLog.txt")
    local opts = gse_build_item_options(state)
    if #opts == 0 then
        ImGui.Text("No gear items found in inventory/bank to edit.")
        return
    end

    -- Dropdown (ImGui.Combo; UIForge uses 0-based indices, so store idx0 and convert to Lua 1-based)
    ImGui.Text("Item:")
    ImGui.SameLine()

    local idx0 = _gse.selected_idx0
    if idx0 == nil then idx0 = 0 end
    if idx0 < 0 then idx0 = 0 end
    if idx0 >= #opts then idx0 = 0 end

    local labels = {}
    for i, opt in ipairs(opts) do
        labels[i] = opt.label
    end

    local new_idx0 = ImGui.Combo("##GSE_ItemCombo", idx0, labels, #labels)
    if new_idx0 == nil then new_idx0 = idx0 end

    if new_idx0 ~= idx0 then
        idx0 = new_idx0
        _gse.selected_idx0 = idx0

        local sel = idx0 + 1
        if sel < 1 then sel = 1 end
        if sel > #opts then sel = 1 end

        gse_prefill_from_gearlog(state.gearlog, opts[sel], true)
        _gse.last_key = opts[sel].key
    else
        _gse.selected_idx0 = idx0
    end

    local sel = (_gse.selected_idx0 or 0) + 1
    if sel < 1 then sel = 1 end
    if sel > #opts then sel = 1 end

    local item = opts[sel]
    if item and _gse.last_key ~= item.key then
        gse_prefill_from_gearlog(state.gearlog, item, false)
        _gse.last_key = item.key
    end

    ImGui.Separator()


    ImGui.Columns(3, "gse_cols", false)

    gse_input_text("STR", "STR", 120)
    gse_input_text("STA", "STA", 120)
    gse_input_text("AGI", "AGI", 120)
    gse_input_text("DEX", "DEX", 120)
    gse_input_text("WIS", "WIS", 120)
    gse_input_text("INT", "INT", 120)
    gse_input_text("CHA", "CHA", 120)

    ImGui.NextColumn()

    gse_input_text("AR", "AR", 120)
    gse_input_text("CR", "CR", 120)
    gse_input_text("DR", "DR", 120)
    gse_input_text("FR", "FR", 120)
    gse_input_text("LR", "LR", 120)
    gse_input_text("PR", "PR", 120)

    ImGui.NextColumn()

    gse_input_text("AC", "AC", 120)
    gse_input_text("Damage", "DAMAGE", 120)
    gse_input_text("HP", "HP", 120)
    gse_input_text("POW", "PWR", 120)
    gse_input_text("HoT", "HOT", 120)
    gse_input_text("PoT", "POT", 120)
    gse_input_text("Proc", "PROC", 200)
    gse_input_text("Source", "SOURCE", 200)
    gse_rarity_dropdown()

    ImGui.Columns(1)

    ImGui.Spacing()
    if ImGui.Button("Save to GearLog.txt") then
        local path = state.gearlog_path or get_gearlog_path()
        local rarity = safe_trim((_gse.fields.RARITY or "C"):upper())
        if rarity == "U" then rarity = "UC" end

        local tokens = gse_build_stats_tokens(_gse.fields)
        local new_stats = table.concat(tokens, " | ")
        local source = safe_trim(_gse.fields.SOURCE or _gse.fields.BIOME or "")
        _gse.fields.BIOME = source

        state.gearlog[item.key] = {
            name   = item.name,
            level  = item.level,
            rarity = rarity,
            source = source,
            stats  = new_stats,
        }

        local ok, err = gse_write_gearlog(path, state.gearlog)
        if ok then
            gse_set_message("Saved to " .. tostring(path), {0.2,1.0,0.2,1.0})
            -- force rebuild so lists refresh with new display text
            state.built = false
        else
            gse_set_message(err or "Failed to save.", {1.0,0.2,0.2,1.0})
        end
    end

    ImGui.SameLine()
    if ImGui.Button("Clear Fields") then
        gse_clear_fields()
        gse_set_message("", {1,1,1,1})
    end

    if _gse.message and _gse.message ~= "" then
        local c = _gse.message_color or {1,1,1,1}
        ImGui.Spacing()
        ImGui.TextColored(c[1],c[2],c[3],c[4], _gse.message)
    end
end


-- =========================
-- UI helpers
-- =========================
local function render_list_numbered(list, start_index)
    local n = start_index or 1
    for _, v in ipairs(list) do
        local key = tracked_base_name_from_text(v)
        local tracked_now = tracked_is_tracked(key)
        local btn = tracked_now and ("Untrack##" .. tostring(key)) or ("Track##" .. tostring(key))
        if ImGui.SmallButton(btn) then
            tracked_toggle(key, key, { kind = "item" })
        end
        ImGui.SameLine()
        ImGui.Text(("%d. %s"):format(n, v))
        n = n + 1
    end
    ImGui.Spacing()
    return n
end

local function render_rows_numbered(rows, start_index)
    local n = start_index or 1
    for _, r in ipairs(rows) do
        local key = ""
        if r.name and r.level and tonumber(r.level or 0) and tonumber(r.level or 0) > 0 then
            key = r.name
        else
            key = r.name or tracked_base_name_from_text(r.text or tostring(n))
        end

        local tracked_now = tracked_is_tracked(key)
        local btn = tracked_now and ("Untrack##" .. tostring(key)) or ("Track##" .. tostring(key))
        if ImGui.SmallButton(btn) then
            tracked_toggle(key, r.name or key, { kind = "item" })
        end
        ImGui.SameLine()

        local rar = (r.rarity or ""):upper()
        local color = GEAR_RARITY_COLOR[rar]
        local line = ("%d. %s"):format(n, r.text)
        if color then
            ImGui.TextColored(color[1], color[2], color[3], color[4], line)
        else
            ImGui.Text(line)
        end
        n = n + 1
    end
    ImGui.Spacing()
    return n
end

local function render_gem_list(gem_rows)
    for i, row in ipairs(gem_rows) do
        local rarity = row.rarity or ""
        local color = RARITY_COLOR[rarity]

        if color then
            ImGui.TextColored(color[1], color[2], color[3], color[4],
                ("%d. %s"):format(i, row.text))
        else
            ImGui.Text(("%d. %s"):format(i, row.text))
        end
    end
    ImGui.Spacing()
end

-- =========================
-- UI
-- =========================
ImGui.SetNextWindowSize(900, 780)
if ImGui.Begin("Inventory Manager") then
    -- Build once, then only on Refresh button
    if not state.built then
        rebuild()
    end

    if ImGui.Button("Refresh") then
        rebuild()
    end
    ImGui.SameLine()
    if ImGui.Button("Gear Stats Editor##Open") then
        state.editor_open = true
        state._editor_popup_armed = true
    end
    ImGui.SameLine()
    if state.last_refresh ~= "" then
        ImGui.Text(("Last refreshed: %s"):format(state.last_refresh))
    else
        ImGui.Text("Last refreshed: (unknown)")
    end

    if state.gearlog_path and state.gearlog_path ~= "" then
        if state.gearlog_ok then
            ImGui.Text(("GearLog: %s"):format(state.gearlog_path))
        else
            ImGui.Text(("GearLog: NOT FOUND (%s)"):format(state.gearlog_path))
        end
    else
        ImGui.Text("GearLog: (not set)")
    end

    ImGui.Separator()
    ImGui.Text(("Inventory Slots Used: %d / 40"):format(state.inv_used))
    ImGui.Text(("Bank Slots Used: %d / 40"):format(state.bank_used))
    ImGui.Separator()

    if ImGui.CollapsingHeader("Inventory - Equipped Gear") then
        if #state.inv_equipped == 0 then
            ImGui.Text("None")
        else
            render_rows_numbered(state.inv_equipped, 1)
        end
    end

    if ImGui.CollapsingHeader("Inventory - Gems") then
        if #state.inv_gems == 0 then
            ImGui.Text("None")
        else
            render_gem_list(state.inv_gems)
        end
    end

    if ImGui.CollapsingHeader("Inventory - Other Items") then
        if #state.inv_other_gear == 0 and #state.inv_other_plain == 0 then
            ImGui.Text("None")
        else
            local n = 1
            if #state.inv_other_gear > 0 then
                n = render_rows_numbered(state.inv_other_gear, n)
            end
            if #state.inv_other_plain > 0 then
                n = render_list_numbered(state.inv_other_plain, n)
            end
        end
    end

    ImGui.Separator()

    if ImGui.CollapsingHeader("Bank - Gear") then
        if #state.bank_gear == 0 then
            ImGui.Text("None")
        else
            render_rows_numbered(state.bank_gear, 1)
        end
    end

    if ImGui.CollapsingHeader("Bank - Gems") then
        if #state.bank_gems == 0 then
            ImGui.Text("None")
        else
            render_gem_list(state.bank_gems)
        end
    end

    ImGui.Separator()
    -- Gear Stats Editor popup
    if state.editor_open == nil then state.editor_open = false end
    if state.editor_open then
        -- OpenPopup only needs to be called once, right when we switch to open
        if state._editor_popup_armed then
            ImGui.OpenPopup("Gear Stats Editor##Popup")
            state._editor_popup_armed = false
        end
    end

    -- BeginPopupModal keeps the editor open until closed explicitly
    if ImGui.BeginPopupModal("Gear Stats Editor##Popup") then
        render_gear_stats_editor(state)

        ImGui.Separator()
        if ImGui.Button("Close##PopupClose") then
            ImGui.CloseCurrentPopup()
            state.editor_open = false
        end
        ImGui.EndPopup()
    end

    if ImGui.CollapsingHeader("Bank - Other Items") then
        if #state.bank_other == 0 then
            ImGui.Text("None")
        else
            render_list_numbered(state.bank_other, 1)
        end
    end
end
ImGui.End()

-- Tracked Items window
render_tracked_window()
