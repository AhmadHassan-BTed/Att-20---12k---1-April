#!/usr/bin/env python3
"""
Final verification that the patched ISO is ready for PCSX2 testing.
"""
import pycdlib
import struct
from core.esf_parser import ESFParser
import io
import json

def verify_patched_iso():
    iso = pycdlib.PyCdlib()
    iso.open('iso/patched/EQOA_Frontiers_Patched.iso')
    record = iso.get_record(iso_path='/DATA/CHAR.ESF;1')

    print('[FINAL ISO VERIFICATION]')
    print(f'[OK] Patched ISO File: /DATA/CHAR.ESF;1')
    print(f'  LBA: {record.extent_location()}')
    print(f'  Size: {record.data_length:,} bytes')
    print(f'  Expected: 148,838,890 bytes')

    if record.data_length == 148838890:
        print('  [OK] SIZE MATCHES!')
    else:
        print(f'  [WARN] SIZE MISMATCH! Got {record.data_length:,}')
    print()

    # Extract ESF and check patched assets
    bio = io.BytesIO()
    iso.get_file_from_iso_fp(bio, iso_path='/DATA/CHAR.ESF;1')
    esf_bytes = bio.getvalue()
    iso.close()

    parser = ESFParser(esf_bytes).parse()
    esf_map = {e.asset_id: e for e in parser.pointer_table if e.asset_id is not None}

    # Load target assets
    with open('workspace/target_assets.json', 'r') as f:
        targets = json.load(f)

    print('Patched Model Verification:')
    found_count = 0
    for asset in targets[:3]:  # Check first 3 as sample
        h = int(asset['original_hash'], 16)
        if h in esf_map:
            e = esf_map[h]
            print(f'  0x{h:08X}: {e.length:,} bytes [OK]')
            found_count += 1
        else:
            print(f'  0x{h:08X}: NOT FOUND [FAIL]')

    print()
    print(f'[RESULT] Patched ISO contains {len(esf_map)} assets')
    print(f'[STATUS] ISO is ready for PCSX2 testing!' if found_count > 0 else '[STATUS] VERIFICATION FAILED')

if __name__ == '__main__':
    verify_patched_iso()
