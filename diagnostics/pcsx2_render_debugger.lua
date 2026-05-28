-- diagnostics/pcsx2_render_debugger.lua
-- PCSX2-rr Render Debugger (Lua)
-- Live-prints active bounding box coordinates and flashes red on corruption.

local CSF_BASE_POINTER = 0x00A3F120 -- Mock pointer to the active character model in EE memory

function read_float(addr)
    local hex = memory.readdword(addr)
    -- standard IEEE 754 float conversion in Lua (basic approximation or use emu built-in)
    -- PCSX2-rr might support memory.readfloat(addr), let's assume it does.
    if memory.readfloat then return memory.readfloat(addr) end
    return 0.0
end

while true do
    local char_base = memory.readdword(CSF_BASE_POINTER)
    
    if char_base ~= 0 and char_base ~= nil then
        -- In our ESF node tree, bounding box floats are typically at +0x20 of the first child inline data
        -- For a direct .CSF, let's assume the bounding box floats are at an offset like char_base + 0x40
        -- (This is just a diagnostic hook, exact offset depends on game structure, typically 0x40 or so)
        local bounds_addr = char_base + 0x40
        
        local min_x = memory.readfloat(bounds_addr)
        local min_y = memory.readfloat(bounds_addr + 4)
        local min_z = memory.readfloat(bounds_addr + 8)
        local max_x = memory.readfloat(bounds_addr + 12)
        local max_y = memory.readfloat(bounds_addr + 16)
        local max_z = memory.readfloat(bounds_addr + 20)
        
        gui.text(10, 10, string.format("Active Mesh Bounding Box: Min(%.2f, %.2f, %.2f) Max(%.2f, %.2f, %.2f)", min_x, min_y, min_z, max_x, max_y, max_z))
        
        local is_corrupted = false
        if min_x == 0 and min_y == 0 and min_z == 0 and max_x == 0 and max_y == 0 and max_z == 0 then
            is_corrupted = true
        end
        -- check for NaN in Lua (x ~= x)
        if min_x ~= min_x or min_y ~= min_y or min_z ~= min_z then
            is_corrupted = true
        end
        
        if is_corrupted then
            -- Flash "CULLING ALERT: BAD BOUNDS" in red
            local flash = (emu.framecount() % 30) < 15
            if flash then
                gui.text(10, 30, "CULLING ALERT: BAD BOUNDS", "red")
            end
        else
            gui.text(10, 30, "BOUNDS OK", "green")
        end
    else
        gui.text(10, 10, "Waiting for character mesh to load...")
    end
    
    emu.frameadvance()
end
