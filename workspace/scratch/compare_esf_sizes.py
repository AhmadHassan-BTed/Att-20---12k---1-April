#!/usr/bin/env python3
"""
Compare asset sizes between Vanilla, Frontiers, and Patched ESF files.
Shows the transformation and validates the Pristine Structural Upgrade.
"""
import os
from core.esf_parser import ESFParser
import json

def compare_esf_sizes():
    print("\n[COMPARISON] ESF Asset Size Analysis")
    print("=" * 70)

    try:
        # Load target assets
        with open('workspace/target_assets.json', 'r') as f:
            targets = json.load(f)

        # Load all three ESF databases
        databases = {}

        for db_name, path in [
            ('Vanilla', 'workspace/original/CHAR.ESF'),
            ('Frontiers', 'workspace/expansion/CHAR.ESF'),
            ('Patched', 'workspace/FINAL_CHAR_MERGED.ESF'),
        ]:
            if not os.path.exists(path):
                print(f"\n[SKIP] {db_name} database not found: {path}")
                databases[db_name] = {}
                continue

            with open(path, 'rb') as f:
                data = f.read()

            parser = ESFParser(data).parse()
            esf_map = {e.asset_id: e for e in parser.pointer_table if e.asset_id is not None}
            databases[db_name] = esf_map

            print(f"\n{db_name:12} database: {len(esf_map):3} assets, {len(data):,} bytes")

        # Compare target models
        print(f"\n{'-'*70}")
        print(f"{'Hash':<12} {'Vanilla':>15} {'Frontiers':>15} {'Patched':>15} {'Status':>10}")
        print(f"{'-'*70}")

        for i, asset in enumerate(targets[:11]):
            h = int(asset['original_hash'], 16)

            v_size = databases['Vanilla'].get(h).length if h in databases['Vanilla'] else 0
            f_size = databases['Frontiers'].get(h).length if h in databases['Frontiers'] else 0
            p_size = databases['Patched'].get(h).length if h in databases['Patched'] else 0

            # Determine status
            if p_size > 0:
                if p_size == v_size:
                    status = "VANILLA"
                elif p_size == f_size:
                    status = "FRONTIERS"
                else:
                    status = "HYBRID"
            else:
                status = "MISSING"

            print(f"0x{h:08X}  {v_size:>15,} {f_size:>15,} {p_size:>15,} {status:>10}")

        print(f"{'-'*70}")
        print("\nInterpretation:")
        print("  VANILLA    = Asset size matches original Vanilla model")
        print("  FRONTIERS  = Asset size matches native Frontiers model")
        print("  HYBRID     = Asset is the upgraded Vanilla model in Frontiers format")
        print("  MISSING    = Asset not found in patched database")

        return True

    except Exception as e:
        print(f"[ERROR] {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == '__main__':
    compare_esf_sizes()
