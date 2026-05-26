#!/usr/bin/env python3
import struct, json, os

def find_first_vif(data, label):
    for i in range(0, len(data) - 4, 4):
        val = struct.unpack('<I', data[i:i+4])[0]
        msb = (val >> 24) & 0xFF
        if msb == 0x01 or (0x60 <= msb <= 0x7F):
            print(f'  [{label}] First VIF tag at offset 0x{i:06X}: 0x{val:08X} (MSB=0x{msb:02X})')
            return i
    print(f'  [{label}] No VIF tags found!')
    return -1

# Vanilla payload
vanilla_path = 'workspace/payloads/asset_0x05AEBA67.bin'
with open(vanilla_path, 'rb') as f:
    vdata = f.read()
print(f'Vanilla file size: {len(vdata):,}')
v_start = find_first_vif(vdata, 'VANILLA')

# Frontiers
with open('workspace/target_assets.json', 'r') as f:
    targets = json.load(f)
t = targets[1]
with open('workspace/expansion/CHAR.ESF', 'rb') as f:
    f.seek(t['expansion_offset'])
    fdata = f.read(t['expansion_length'])
print(f'Frontiers file size: {len(fdata):,}')
f_start = find_first_vif(fdata, 'FRONTIERS')

# Node tree structure
print()
print('=== NODE TREE STRUCTURE (first levels) ===')
print()
print('VANILLA:')
pos = 0
type_id, data_size, child_count = struct.unpack('<III', vdata[0:12])
print(f'  Root: type=0x{type_id:05X} data_size={data_size:,} children={child_count}')
pos = 12
for c in range(min(child_count, 5)):
    ct, cs, cc = struct.unpack('<III', vdata[pos:pos+12])
    print(f'    Child[{c}]: type=0x{ct:05X} data_size={cs:,} children={cc} @ offset 0x{pos:X}')
    pos += 12
    if cc == 0:
        pos += cs
    else:
        for gc in range(min(cc, 3)):
            if pos + 12 > len(vdata):
                break
            gt, gs, gcc = struct.unpack('<III', vdata[pos:pos+12])
            print(f'      GrandChild[{gc}]: type=0x{gt:05X} data_size={gs:,} children={gcc} @ offset 0x{pos:X}')
            pos += 12
            if gcc == 0:
                pos += gs

print()
print('FRONTIERS:')
pos = 0
type_id, data_size, child_count = struct.unpack('<III', fdata[0:12])
print(f'  Root: type=0x{type_id:05X} data_size={data_size:,} children={child_count}')
pos = 12
for c in range(min(child_count, 5)):
    ct, cs, cc = struct.unpack('<III', fdata[pos:pos+12])
    print(f'    Child[{c}]: type=0x{ct:05X} data_size={cs:,} children={cc} @ offset 0x{pos:X}')
    pos += 12
    if cc == 0:
        pos += cs
    else:
        for gc in range(min(cc, 3)):
            if pos + 12 > len(fdata):
                break
            gt, gs, gcc = struct.unpack('<III', fdata[pos:pos+12])
            print(f'      GrandChild[{gc}]: type=0x{gt:05X} data_size={gs:,} children={gcc} @ offset 0x{pos:X}')
            pos += 12
            if gcc == 0:
                pos += gs

# Also check: does the Frontiers ESF use type 0x72700 for the same hash?
print()
print('=== TYPE MISMATCH PROOF ===')
print(f'  Vanilla  root type_id: 0x{struct.unpack("<I", vdata[0:4])[0]:05X}')
print(f'  Frontiers root type_id: 0x{struct.unpack("<I", fdata[0:4])[0]:05X}')

# Also scan the Frontiers node: does it have MORE children?
v_cc = struct.unpack('<I', vdata[8:12])[0]
f_cc = struct.unpack('<I', fdata[8:12])[0]
print(f'  Vanilla  child_count: {v_cc}')
print(f'  Frontiers child_count: {f_cc}')
