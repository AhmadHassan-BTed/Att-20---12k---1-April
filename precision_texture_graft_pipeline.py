import os
import sys
import math
import struct
import json
import copy
import subprocess
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

def find_nodes(node, type_id, lst):
    if node['type_id'] == type_id:
        lst.append(node)
    for child in node['children']:
        find_nodes(child, type_id, lst)

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

def main():
    json_path = 'workspace/target_assets.json'
    original_esf = 'workspace/original/CHAR.ESF'
    expansion_esf = 'workspace/expansion/CHAR.ESF'
    payloads_dir = 'workspace/payloads'
    
    print("=" * 80)
    print("  EQOA AUTOMATED HIGH-FIDELITY PRECISION TEXTURE SWAP PIPELINE")
    print("=" * 80)
    
    if not os.path.exists(json_path):
        print(f"[-] Error: {json_path} not found!")
        sys.exit(1)
        
    if not os.path.exists(original_esf) or not os.path.exists(expansion_esf):
        print("[-] Error: Source ESF files not found!")
        sys.exit(1)
        
    with open(json_path, 'r') as f:
        targets = json.load(f)
        
    # Clear out and recreate payloads directory
    if os.path.exists(payloads_dir):
        import shutil
        shutil.rmtree(payloads_dir)
    os.makedirs(payloads_dir, exist_ok=True)
    
    # Load ESFs
    print(f"\n[*] Parsing Vanilla ESF: {original_esf}...")
    with open(original_esf, 'rb') as f:
        van_esf_bytes = f.read()
    van_parser = ESFParser(van_esf_bytes).parse()
    van_map = {e.asset_id: e for e in van_parser.pointer_table if e.asset_id is not None}
    
    print(f"\n[*] Parsing Frontiers ESF: {expansion_esf}...")
    with open(expansion_esf, 'rb') as f:
        fro_esf_bytes = f.read()
    fro_parser = ESFParser(fro_esf_bytes).parse()
    fro_map = {e.asset_id: e for e in fro_parser.pointer_table if e.asset_id is not None}
    
    print(f"\n[*] Commencing Dynamic Texture Linking & Patching on {len(targets)} target assets...")
    
    for idx, t in enumerate(targets):
        h = int(t['original_hash'], 16)
        print(f"\n[{idx+1}/11] Processing asset 0x{h:08X}...")
        
        # Load Vanilla and Frontiers models
        van_entry = van_map[h]
        van_bytes = van_esf_bytes[van_entry.offset : van_entry.offset + van_entry.length]
        van_root, _ = parse_node(van_bytes, 0)
        
        fro_entry = fro_map[h]
        fro_bytes = fro_esf_bytes[fro_entry.offset : fro_entry.offset + fro_entry.length]
        fro_root, _ = parse_node(fro_bytes, 0)
        
        # Locate texture containers: Child 1 (0x11110)
        van_11110 = van_root['children'][1]
        fro_11110 = fro_root['children'][1]
        
        # Locate textures: Child 1 (0x01001)
        van_textures = van_11110['children'][1]['children']
        fro_textures = fro_11110['children'][1]['children']
        
        # Map Vanilla and Frontiers texture hashes to their slot indices
        van_hash_to_idx = {struct.unpack('<I', t_node['inline_data'][:4])[0]: i for i, t_node in enumerate(van_textures)}
        fro_hash_to_idx = {struct.unpack('<I', t_node['inline_data'][:4])[0]: i for i, t_node in enumerate(fro_textures)}
        
        # Locate material nodes (0x01100) inside Child 2 (0x01101)
        van_mats = van_11110['children'][2]['children']
        fro_mats = fro_11110['children'][2]['children']
        
        # Locate 0x31100 rendering state nodes under the entire tree
        van_31100 = []
        fro_31100 = []
        find_nodes(van_root, 0x31100, van_31100)
        find_nodes(fro_root, 0x31100, fro_31100)
        
        # 1. Establish automated dynamic pairing using material-texture links
        paired_f_to_v = {}
        for m_idx in range(min(len(van_31100), len(fro_31100))):
            v_mat = van_31100[m_idx]
            f_mat = fro_31100[m_idx]
            
            v_hash = struct.unpack('<I', v_mat['inline_data'][16:20])[0]
            f_hash = struct.unpack('<I', f_mat['inline_data'][16:20])[0]
            
            v_idx = van_hash_to_idx.get(v_hash, -1)
            f_idx = fro_hash_to_idx.get(f_hash, -1)
            
            if v_idx != -1 and f_idx != -1:
                paired_f_to_v[f_idx] = v_idx
                
        # 2. Pair any unlinked textures (typically the 128x128 palettes) by size and order
        unlinked_fro = [i for i in range(len(fro_textures)) if i not in paired_f_to_v]
        unlinked_van = [i for i in range(len(van_textures)) if i not in paired_f_to_v.values()]
        
        for f_idx in unlinked_fro:
            f_t_node = fro_textures[f_idx]
            f_w = struct.unpack_from('<I', f_t_node['inline_data'], 4)[0]
            f_h = struct.unpack_from('<I', f_t_node['inline_data'], 8)[0]
            
            # Find matching unlinked Vanilla texture of same dimensions
            match_v_idx = None
            for v_idx in unlinked_van:
                v_t_node = van_textures[v_idx]
                v_w = struct.unpack_from('<I', v_t_node['inline_data'], 4)[0]
                v_h = struct.unpack_from('<I', v_t_node['inline_data'], 8)[0]
                if f_w == v_w and f_h == v_h:
                    match_v_idx = v_idx
                    break
                    
            if match_v_idx is not None:
                paired_f_to_v[f_idx] = match_v_idx
                unlinked_van.remove(match_v_idx)
            else:
                # If no dimension match, pair with first remaining unlinked Vanilla texture of same size if possible
                for v_idx in unlinked_van:
                    v_t_node = van_textures[v_idx]
                    if len(f_t_node['inline_data']) == len(v_t_node['inline_data']):
                        match_v_idx = v_idx
                        break
                if match_v_idx is not None:
                    paired_f_to_v[f_idx] = match_v_idx
                    unlinked_van.remove(match_v_idx)
                    
        print(f"    Dynamic Texture Mapping Pairs (Frontiers -> Vanilla):")
        print(f"      {sorted(paired_f_to_v.items())}")
        
        # 3. Perform Precision Texture Grafting on Frontiers model
        graft_root = copy.deepcopy(fro_root)
        g_11110 = graft_root['children'][1]
        g_textures = g_11110['children'][1]['children']
        g_mats = g_11110['children'][2]['children']
        
        for f_idx, v_idx in paired_f_to_v.items():
            f_t_node = fro_textures[f_idx]
            v_t_node = van_textures[v_idx]
            g_t_node = g_textures[f_idx]
            
            # Extract Frontiers original texture hash (first 4 bytes)
            fro_original_hash = f_t_node['inline_data'][:4]
            
            # Overwrite keeping original hash
            g_t_node['inline_data'] = fro_original_hash + v_t_node['inline_data'][4:]
            g_t_node['data_size'] = len(g_t_node['inline_data'])
            
            # Parse Vanilla texture dimensions
            v_w, v_h = get_vanilla_texture_dimensions(v_t_node['inline_data'])
            
            # Find and patch log2 TW/TH inside corresponding material nodes (0x01100)
            # A texture slot can be linked to multiple material nodes
            f_t_hash = struct.unpack('<I', fro_original_hash)[0]
            for m_idx, mat_node in enumerate(g_mats):
                # Search the material node children or inline data for references to this texture hash
                # If the material node inline data references this hash, patch its TEX0 TW/TH
                patched_mat = translate_material_node(mat_node, v_w, v_h)
                g_mats[m_idx] = patched_mat
                
        # 4. Update parent node sizes recursively
        update_node_sizes(graft_root)
        
        # 5. Serialize hybrid model
        final_payload = serialize_node(graft_root)
        
        bin_path = os.path.join(payloads_dir, f"asset_0x{h:08X}.bin")
        with open(bin_path, 'wb') as f:
            f.write(final_payload)
        print(f"    [+] Successfully grafted textures & saved payload -> {bin_path} ({len(final_payload):,} bytes)")
        
        # Verify
        _, end_pos = parse_node(final_payload, 0)
        if end_pos == len(final_payload):
            print("    [PASS] Hybrid model validated successfully!")
        else:
            print("    [FAIL] Hybrid model parsing mismatch!")
            sys.exit(1)
            
    # Trigger database rebuilder
    print("\n[*] Rebuilding the merged CHAR.ESF database...")
    subprocess.run([sys.executable, "esf_rebuilder.py"], check=True)
    
    # Trigger ISO repacker
    print("\n[*] Repacking and patching the playable game ISO...")
    subprocess.run([sys.executable, "repack_iso.py"], check=True)
    
    # Verify final ISO
    print("\n[*] Verifying repacked ISO integrity...")
    subprocess.run([sys.executable, "verify_final_iso.py"], check=True)
    
    print("\n" + "=" * 80)
    print("  DYNAMIC PRECISION TEXTURE SWAP PIPELINE COMPLETION SUCCESSFUL!")
    print("=" * 80)

if __name__ == '__main__':
    main()
