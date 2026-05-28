#!/usr/bin/env python3
"""Compare bone/skeleton structures between Vanilla and Frontiers models."""
import struct, json, sys, os
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from core.esf_parser import ESFParser

def parse_node(data, pos):
    if pos + 12 > len(data): return None, pos
    tid = struct.unpack_from('<I', data, pos)[0]
    dsz = struct.unpack_from('<I', data, pos+4)[0]
    cc  = struct.unpack_from('<I', data, pos+8)[0]
    node = {'type_id': tid, 'data_size': dsz, 'child_count': cc, 'children': [], 'inline_data': None}
    pos += 12
    if cc == 0:
        node['inline_data'] = data[pos:pos+dsz]
        pos += dsz
    else:
        for _ in range(cc):
            child, pos = parse_node(data, pos)
            if child: node['children'].append(child)
    return node, pos

with open('workspace/original/CHAR.ESF', 'rb') as f:
    van_bytes = f.read()
with open('workspace/expansion/CHAR.ESF', 'rb') as f:
    fro_bytes = f.read()

van_map = {e.asset_id: e for e in ESFParser(van_bytes).parse().pointer_table if e.asset_id}
fro_map = {e.asset_id: e for e in ESFParser(fro_bytes).parse().pointer_table if e.asset_id}

with open('workspace/target_assets.json') as f:
    targets = json.load(f)

for t in targets[:3]:  # Check first 3 targets
    h = int(t['expansion_hash'], 16)
    print(f"\n{'='*60}")
    print(f"  Target: 0x{h:08X}")
    print(f"{'='*60}")
    
    van_e = van_map[h]
    fro_e = fro_map[h]
    van_root, _ = parse_node(van_bytes[van_e.offset:van_e.offset+van_e.length], 0)
    fro_root, _ = parse_node(fro_bytes[fro_e.offset:fro_e.offset+fro_e.length], 0)
    
    print(f"  Van root: 0x{van_root['type_id']:05X} size={van_root['data_size']}")
    print(f"  Fro root: 0x{fro_root['type_id']:05X} size={fro_root['data_size']}")
    
    # 0x22400 hierarchy
    van_hier = next((c for c in van_root['children'] if c['type_id'] == 0x22400), None)
    fro_hier = next((c for c in fro_root['children'] if c['type_id'] == 0x22400), None)
    
    if van_hier and van_hier['inline_data']:
        van_bones = struct.unpack_from('<i', van_hier['inline_data'], 0)[0]
        print(f"  Van 0x22400 bones: {van_bones}")
    else:
        print(f"  Van 0x22400: NOT FOUND")
    
    if fro_hier and fro_hier['inline_data']:
        fro_bones = struct.unpack_from('<i', fro_hier['inline_data'], 0)[0]
        print(f"  Fro 0x22400 bones: {fro_bones}")
    else:
        print(f"  Fro 0x22400: NOT FOUND")
    
    # 0x0B070 skeleton
    van_skel = next((c for c in van_root['children'] if c['type_id'] == 0x0B070), None)
    fro_skel = next((c for c in fro_root['children'] if c['type_id'] == 0x0B070), None)
    van_skel_cc = van_skel['child_count'] if van_skel else 'N/A'
    fro_skel_cc = fro_skel['child_count'] if fro_skel else 'N/A'
    van_skel_sz = van_skel['data_size'] if van_skel else 'N/A'
    fro_skel_sz = fro_skel['data_size'] if fro_skel else 'N/A'
    print(f"  Van 0x0B070: children={van_skel_cc}, size={van_skel_sz}")
    print(f"  Fro 0x0B070: children={fro_skel_cc}, size={fro_skel_sz}")
    
    # 0x02800 bone matrices
    van_mat = next((c for c in van_root['children'] if c['type_id'] == 0x02800), None)
    fro_mat = next((c for c in fro_root['children'] if c['type_id'] == 0x02800), None)
    van_mat_sz = van_mat['data_size'] if van_mat else 'N/A'
    fro_mat_sz = fro_mat['data_size'] if fro_mat else 'N/A'
    print(f"  Van 0x02800 size: {van_mat_sz}")
    print(f"  Fro 0x02800 size: {fro_mat_sz}")
    
    # 0x42710 header compare
    van_hdr = next((c for c in van_root['children'] if c['type_id'] == 0x42710), None)
    fro_hdr = next((c for c in fro_root['children'] if c['type_id'] == 0x42710), None)
    if van_hdr and van_hdr['inline_data']:
        print(f"  Van 0x42710: {van_hdr['inline_data'][:48].hex()}")
    if fro_hdr and fro_hdr['inline_data']:
        print(f"  Fro 0x42710: {fro_hdr['inline_data'][:48].hex()}")
    
    # Compare 0x02610 mesh part sizes
    van_anim = next((c for c in van_root['children'] if c['type_id'] == 0x2610), None)
    fro_anim = next((c for c in fro_root['children'] if c['type_id'] == 0x2610), None)
    if van_anim and fro_anim:
        van_sizes = [c['data_size'] for c in van_anim['children']]
        fro_sizes = [c['data_size'] for c in fro_anim['children']]
        print(f"  Van 0x2610: {len(van_sizes)} parts, total={sum(van_sizes)}")
        print(f"  Fro 0x2610: {len(fro_sizes)} parts, total={sum(fro_sizes)}")
        print(f"  Size lists match: {van_sizes == fro_sizes}")
        mismatches = sum(1 for a, b in zip(van_sizes, fro_sizes) if a != b)
        print(f"  Mismatched parts: {mismatches}/{len(van_sizes)}")
        
        # Show first mismatched pair
        for i, (vs, fs) in enumerate(zip(van_sizes, fro_sizes)):
            if vs != fs:
                print(f"    Part[{i}]: Van={vs}, Fro={fs}")
                break
    
    # Check if BOTH models share exact same 0x42710 header bytes
    if van_hdr and fro_hdr and van_hdr['inline_data'] and fro_hdr['inline_data']:
        print(f"  Headers identical: {van_hdr['inline_data'] == fro_hdr['inline_data']}")
    
    # Check 0x32910 (CSpritePlayList variant?)
    van_play = next((c for c in van_root['children'] if c['type_id'] == 0x32910), None)
    fro_play = next((c for c in fro_root['children'] if c['type_id'] == 0x32910), None)
    if van_play and van_play['inline_data'] and fro_play and fro_play['inline_data']:
        print(f"  Van 0x32910 size: {len(van_play['inline_data'])}")
        print(f"  Fro 0x32910 size: {len(fro_play['inline_data'])}")
        print(f"  0x32910 identical: {van_play['inline_data'] == fro_play['inline_data']}")

# Now check the injected payload
print(f"\n{'='*60}")
print(f"  INJECTED PAYLOAD ANALYSIS")
print(f"{'='*60}")

h = int(targets[0]['expansion_hash'], 16)
payload_path = f"workspace/payloads/asset_0x{h:08X}.bin"
if os.path.exists(payload_path):
    with open(payload_path, 'rb') as f:
        inj_data = f.read()
    inj_root, _ = parse_node(inj_data, 0)
    
    print(f"  Root: 0x{inj_root['type_id']:05X} size={inj_root['data_size']}")
    print(f"  Children: {inj_root['child_count']}")
    
    for i, c in enumerate(inj_root['children']):
        print(f"    [{i:2d}] 0x{c['type_id']:05X} size={c['data_size']:,} cc={c['child_count']}")
    
    # The KEY check: which 0x0B070/0x02800 does the injected model use?
    inj_skel = next((c for c in inj_root['children'] if c['type_id'] == 0x0B070), None)
    inj_mat = next((c for c in inj_root['children'] if c['type_id'] == 0x02800), None)
    inj_anim = next((c for c in inj_root['children'] if c['type_id'] == 0x2610), None)
    
    fro_root2, _ = parse_node(fro_bytes[fro_map[h].offset:fro_map[h].offset+fro_map[h].length], 0)
    fro_skel2 = next((c for c in fro_root2['children'] if c['type_id'] == 0x0B070), None)
    fro_mat2 = next((c for c in fro_root2['children'] if c['type_id'] == 0x02800), None)
    fro_anim2 = next((c for c in fro_root2['children'] if c['type_id'] == 0x2610), None)
    
    van_root2, _ = parse_node(van_bytes[van_map[h].offset:van_map[h].offset+van_map[h].length], 0)
    van_skel2 = next((c for c in van_root2['children'] if c['type_id'] == 0x0B070), None)
    van_mat2 = next((c for c in van_root2['children'] if c['type_id'] == 0x02800), None)
    van_anim2 = next((c for c in van_root2['children'] if c['type_id'] == 0x2610), None)
    
    print(f"\n  MISMATCH ANALYSIS:")
    if inj_skel and fro_skel2:
        skel_match_fro = (inj_skel['data_size'] == fro_skel2['data_size'])
        print(f"  Injected skeleton (0x0B070) matches Frontiers: {skel_match_fro}")
    if inj_skel and van_skel2:
        skel_match_van = (inj_skel['data_size'] == van_skel2['data_size'])
        print(f"  Injected skeleton (0x0B070) matches Vanilla: {skel_match_van}")
    
    if inj_mat and fro_mat2:
        mat_match_fro = (inj_mat['data_size'] == fro_mat2['data_size'])
        print(f"  Injected bone matrices (0x02800) matches Frontiers: {mat_match_fro}")
    if inj_mat and van_mat2:
        mat_match_van = (inj_mat['data_size'] == van_mat2['data_size'])
        print(f"  Injected bone matrices (0x02800) matches Vanilla: {mat_match_van}")
    
    if inj_anim and fro_anim2:
        geom_match_fro = (inj_anim['data_size'] == fro_anim2['data_size'])
        print(f"  Injected geometry (0x2610) matches Frontiers: {geom_match_fro}")
    if inj_anim and van_anim2:
        geom_match_van = (inj_anim['data_size'] == van_anim2['data_size'])
        print(f"  Injected geometry (0x2610) matches Vanilla: {geom_match_van}")
    
    print(f"\n  *** DIAGNOSIS ***")
    if inj_anim and van_anim2 and inj_skel and fro_skel2:
        geom_is_van = (inj_anim['data_size'] == van_anim2['data_size'])
        skel_is_fro = (inj_skel['data_size'] == fro_skel2['data_size'])
        if geom_is_van and skel_is_fro:
            print(f"  CONFIRMED: Geometry is Vanilla but Skeleton is Frontiers!")
            print(f"  This WILL cause invisible rendering or crash if the mesh references")
            print(f"  bone indices that don't exist in the Frontiers skeleton, or if the")
            print(f"  bone matrix stride differs.")
