import os
import sys
import math
import struct
import json
import copy
from esf_parser import ESFParser

# PS2 GS Constants for TEX0 Patching
GS_REG_TEX0_1  = 0x06
GS_REG_TEX0_2  = 0x16
PSM_PALETTED = {0x13, 0x14, 0x1B, 0x24, 0x2C}

_TEX0_FIELDS = [
    ('TBP0', 0,  14), ('TBW',  14,  6), ('PSM',  20,  6), ('TW',   26,  4),
    ('TH',   30,  4), ('TCC',  34,  1), ('TFX',  35,  2), ('CBP',  37, 14),
    ('CPSM', 51,  4), ('CSM',  55,  1), ('CSA',  56,  5), ('CLD',  61,  3),
]

def _extract_bits(val: int, offset: int, count: int) -> int:
    return (val >> offset) & ((1 << count) - 1)

def _insert_bits(target: int, val: int, offset: int, count: int) -> int:
    mask = ((1 << count) - 1) << offset
    return (target & ~mask) | ((val & ((1 << count) - 1)) << offset)

def parse_tex0(reg64: int) -> dict:
    return {name: _extract_bits(reg64, off, cnt) for name, off, cnt in _TEX0_FIELDS}

def encode_tex0(fields: dict) -> int:
    val = 0
    for name, off, cnt in _TEX0_FIELDS:
        val = _insert_bits(val, fields.get(name, 0), off, cnt)
    return val

# Node Tree Utilities
def parse_node(data: bytes, pos: int) -> tuple:
    if pos + 12 > len(data):
        return None, pos
    type_id     = struct.unpack_from('<I', data, pos    )[0]
    data_size   = struct.unpack_from('<I', data, pos + 4)[0]
    child_count = struct.unpack_from('<I', data, pos + 8)[0]
    node = {
        'type_id': type_id, 'data_size': data_size,
        'child_count': child_count, 'children': [], 'inline_data': None,
    }
    pos += 12
    if child_count == 0:
        if pos + data_size > len(data):
            raise EOFError(f"EOF reading leaf at 0x{pos:X} (need {data_size} B)")
        node['inline_data'] = data[pos : pos + data_size]
        pos += data_size
    else:
        for _ in range(child_count):
            child, pos = parse_node(data, pos)
            if child is not None:
                node['children'].append(child)
    return node, pos

def update_node_sizes(node: dict):
    if node['child_count'] == 0:
        node['data_size'] = len(node['inline_data']) if node['inline_data'] else 0
    else:
        node['child_count'] = len(node['children'])
        total = 0
        for child in node['children']:
            update_node_sizes(child)
            total += 12 + child['data_size']
        node['data_size'] = total

def serialize_node(node: dict) -> bytes:
    buf = bytearray()
    buf += struct.pack('<III', node['type_id'], node['data_size'], node['child_count'])
    if node['child_count'] == 0:
        if node['inline_data']:
            buf += node['inline_data']
    else:
        for child in node['children']:
            buf += serialize_node(child)
    return bytes(buf)

# Dimension Parsing & TEX0 Patcher
def get_vanilla_texture_dimensions(inline_data: bytes) -> tuple:
    if len(inline_data) < 12:
        return 64, 64
    width = struct.unpack_from('<I', inline_data, 4)[0]
    height = struct.unpack_from('<I', inline_data, 8)[0]
    valid_dims = {16, 32, 64, 128, 256, 512}
    if width not in valid_dims or height not in valid_dims:
        w_16 = struct.unpack_from('<H', inline_data, 14)[0]
        h_16 = struct.unpack_from('<H', inline_data, 16)[0]
        if w_16 in valid_dims and h_16 in valid_dims:
            return w_16, h_16
        return 64, 64
    return width, height

def translate_material_node(frontiers_mat_node: dict, vanilla_width: int, vanilla_height: int) -> dict:
    result = copy.deepcopy(frontiers_mat_node)
    fro_raw = frontiers_mat_node.get('inline_data', b'')
    if not fro_raw:
        return result

    try:
        lo = struct.unpack_from('<Q', fro_raw, 0)[0]
        nloop = lo & 0x7FFF
        flg   = (lo >> 58) & 0x3
        nreg  = (lo >> 52) & 0xF
        if nreg == 0: nreg = 16
    except Exception:
        return result

    if flg != 0:
        return result

    fro_tex0_val = None
    pos = 16
    tex0_offset = None
    for _ in range(nloop * nreg):
        if pos + 16 > len(fro_raw):
            break
        addr = struct.unpack_from('<Q', fro_raw, pos + 8)[0]
        if (addr & 0xFF) in (GS_REG_TEX0_1, GS_REG_TEX0_2):
            fro_tex0_val = struct.unpack_from('<Q', fro_raw, pos)[0]
            tex0_offset = pos
            break
        pos += 16

    if fro_tex0_val is None or tex0_offset is None:
        return result

    base = parse_tex0(fro_tex0_val)
    merged = dict(base)
    tw = max(0, int(math.log2(vanilla_width)))
    th = max(0, int(math.log2(vanilla_height)))
    merged['TW'] = tw
    merged['TH'] = th

    if base['PSM'] in PSM_PALETTED:
        merged['CBP'] = 0
        merged['CSA'] = 0
        merged['CLD'] = 1

    new_tex0_val = encode_tex0(merged)
    buf = bytearray(fro_raw)
    struct.pack_into('<Q', buf, tex0_offset, new_tex0_val)

    result['inline_data'] = bytes(buf)
    result['data_size']   = len(buf)
    return result

def translate_texture_container(vanilla_container: dict, frontiers_container: dict, asset_label: str) -> dict:
    result = copy.deepcopy(frontiers_container)
    van_01001 = next((c for c in vanilla_container['children'] if c['type_id'] == 0x01001), None)
    fro_01001 = next((c for c in result['children'] if c['type_id'] == 0x01001), None)
    van_01101 = next((c for c in vanilla_container['children'] if c['type_id'] == 0x01101), None)
    fro_01101 = next((c for c in result['children'] if c['type_id'] == 0x01101), None)

    if not van_01001 or not fro_01001:
        return result

    van_textures = van_01001['children']
    fro_textures = fro_01001['children']
    van_materials = van_01101['children'] if van_01101 else []
    fro_materials = fro_01101['children'] if fro_01101 else []

    num_textures = min(len(van_textures), len(fro_textures))
    for i in range(num_textures):
        van_tex_node = van_textures[i]
        live_tex     = fro_textures[i]
        v_width, v_height = get_vanilla_texture_dimensions(van_tex_node['inline_data'])
        live_tex['inline_data'] = van_tex_node['inline_data']
        live_tex['data_size']   = len(van_tex_node['inline_data'])

        if i < len(van_materials) and i < len(fro_materials):
            fro_materials[i] = translate_material_node(fro_materials[i], v_width, v_height)

    update_node_sizes(result)
    return result

def clean_surgery(vanilla_bytes: bytes, frontiers_bytes: bytes, asset_label: str) -> bytes:
    """Surgically graft Vanilla texture container and geometry container into Frontiers template."""
    van_node, _ = parse_node(vanilla_bytes, 0)
    fro_node, _ = parse_node(frontiers_bytes, 0)
    
    # Verify both are character models
    if van_node['type_id'] not in (0x62700, 0x72700) or fro_node['type_id'] not in (0x62700, 0x72700):
        raise ValueError(f"Asset {asset_label} is not a valid character model!")
        
    graft_root = copy.deepcopy(fro_node)
    
    # 1. Graft texture container (0x11110)
    van_tex = next((c for c in van_node['children'] if c['type_id'] in (0x11100, 0x11110)), None)
    fro_tex_idx = next((i for i, c in enumerate(graft_root['children']) if c['type_id'] == 0x11110), None)
    
    if van_tex and fro_tex_idx is not None:
        graft_root['children'][fro_tex_idx] = translate_texture_container(
            vanilla_container   = van_tex,
            frontiers_container = graft_root['children'][fro_tex_idx],
            asset_label         = asset_label
        )
        print(f"    [+] Grafted Vanilla texture container into slot {fro_tex_idx}")
    else:
        print(f"    [warn] Texture container missing for {asset_label}")
        
    # 2. Replace geometry container (0x02610)
    van_geom = next((c for c in van_node['children'] if c['type_id'] == 0x02610), None)
    fro_geom_idx = next((i for i, c in enumerate(graft_root['children']) if c['type_id'] == 0x02610), None)
    
    if van_geom and fro_geom_idx is not None:
        graft_root['children'][fro_geom_idx] = copy.deepcopy(van_geom)
        print(f"    [+] Replaced geometry container at slot {fro_geom_idx} with Vanilla's container ({van_geom['child_count']} strips)")
    else:
        raise ValueError(f"Geometry container 0x02610 not found in Vanilla or Frontiers for {asset_label}!")
        
    # 3. Update all node sizes recursively
    update_node_sizes(graft_root)
    
    # 4. Serialize hybrid tree
    final_payload = serialize_node(graft_root)
    return final_payload

def main():
    json_path = 'workspace/target_assets.json'
    original_esf = 'workspace/original/CHAR.ESF'
    expansion_esf = 'workspace/expansion/CHAR.ESF'
    
    with open(json_path, 'r') as f:
        targets = json.load(f)
        
    print(f"[*] Parsing Frontiers template ESF: {expansion_esf}")
    with open(expansion_esf, 'rb') as f:
        fro_esf_bytes = f.read()
    fro_parser = ESFParser(fro_esf_bytes).parse()
    fro_map = {e.asset_id: e for e in fro_parser.pointer_table if e.asset_id is not None}
    
    print(f"[*] Parsing Vanilla ESF: {original_esf}")
    with open(original_esf, 'rb') as f:
        van_esf_bytes = f.read()
    van_parser = ESFParser(van_esf_bytes).parse()
    van_map = {e.asset_id: e for e in van_parser.pointer_table if e.asset_id is not None}
    
    print(f"\n[*] Commencing Clean Node-Level Surgery on {len(targets)} target assets...")
    
    for idx, t in enumerate(targets):
        h = int(t['expansion_hash'], 16)
        print(f"\n[{idx+1}/11] Processing target model 0x{h:08X}...")
        
        # Load vanilla payload from vanilla ESF bytes directly to ensure pristine bytes
        van_entry = van_map[h]
        vanilla_bytes = van_esf_bytes[van_entry.offset : van_entry.offset + van_entry.length]
        
        # Load frontiers template
        fro_entry = fro_map[h]
        frontiers_bytes = fro_esf_bytes[fro_entry.offset : fro_entry.offset + fro_entry.length]
        
        try:
            # Perform surgery
            final_payload = clean_surgery(vanilla_bytes, frontiers_bytes, f"0x{h:08X}")
            
            # Save hybrid payload as .bin file in workspace/payloads
            bin_path = f"workspace/payloads/asset_0x{h:08X}.bin"
            with open(bin_path, 'wb') as f:
                f.write(final_payload)
            print(f"    [+] Saved clean hybrid payload -> {bin_path} ({len(final_payload):,} bytes)")
            
            # Sanity verification: parse the generated payload to ensure it is 100% valid
            parsed_node, end_pos = parse_node(final_payload, 0)
            if end_pos == len(final_payload):
                print(f"    [PASS] Clean payload verified successfully! Sizes match, children count = {parsed_node['child_count']}")
            else:
                print(f"    [FAIL] Payload size mismatch after parse: end_pos={end_pos}, file_size={len(final_payload)}")
                
        except Exception as e:
            print(f"    [-] Surgery failed: {e}")
            import traceback
            traceback.print_exc()
            sys.exit(1)

if __name__ == '__main__':
    main()
