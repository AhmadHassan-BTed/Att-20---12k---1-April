#!/usr/bin/env python3
import sys
import json
from esf_parser import ESFParser

def calculate_offsets():
    json_path = './workspace/target_assets.json'
    with open(json_path, 'r') as f:
        targets = json.load(f)
        
    target_map = { int(t['expansion_hash'], 16): t for t in targets }
    
    esf_path = './workspace/expansion/CHAR.ESF'
    with open(esf_path, 'rb') as f:
        esf_data = f.read()
        
    parser = ESFParser(esf_data)
    parser.parse()
    
    cumulative_shift = 0
    results = []
    
    print("[*] Calculating Offset Shifts for all nodes in Virtual Pointer Table...")
    
    for entry in parser.pointer_table:
        old_offset = entry.offset
        new_offset = old_offset + cumulative_shift
        
        if entry.asset_id in target_map:
            t = target_map[entry.asset_id]
            old_size = t['expansion_length']
            new_size = t['original_length']
            
            delta_s = new_size - old_size
            
            # DMA Quadword Alignment logic
            p = 0
            if new_size % 16 != 0:
                p = 16 - (new_size % 16)
                
            node_shift = delta_s + p
            cumulative_shift += node_shift
            
            results.append({
                'index': entry.index,
                'hash': f"0x{entry.asset_id:08X}",
                'old_offset': old_offset,
                'new_offset': new_offset,
                'is_target': True,
                'old_size': old_size,
                'new_size': new_size,
                'delta_s': delta_s,
                'padding': p,
                'node_shift': node_shift,
                'cumulative_shift': cumulative_shift
            })
        else:
            results.append({
                'index': entry.index,
                'hash': f"0x{entry.asset_id:08X}" if entry.asset_id is not None else "None",
                'old_offset': old_offset,
                'new_offset': new_offset,
                'is_target': False,
                'cumulative_shift': cumulative_shift
            })
            
    print("\n" + "="*80)
    print("PROMPT 9: OFFSET SHIFT MATHEMATICS - DRY RUN LOG (LAST 5 ASSETS)")
    print("="*80)
    
    for r in results[-5:]:
        if r['is_target']:
            print(f"Asset Index: {r['index']} (Hash: {r['hash']}) [REPLACED]")
            print(f"  Old Offset:      0x{r['old_offset']:08X} ({r['old_offset']})")
            print(f"  New Offset:      0x{r['new_offset']:08X} ({r['new_offset']})")
            print(f"  Old Size:        {r['old_size']}")
            print(f"  New Size:        {r['new_size']}")
            print(f"  Delta S:         {r['delta_s']}")
            print(f"  Padding (P):     {r['padding']}")
            print(f"  Node Shift:      {r['node_shift']}")
            print(f"  Cumul Shift:     {r['cumulative_shift']}")
        else:
            print(f"Asset Index: {r['index']} (Hash: {r['hash']})")
            print(f"  Old Offset:      0x{r['old_offset']:08X} ({r['old_offset']})")
            print(f"  New Offset:      0x{r['new_offset']:08X} ({r['new_offset']})")
            print(f"  Cumul Shift:     {r['cumulative_shift']}")
        print("-" * 80)

if __name__ == '__main__':
    calculate_offsets()
