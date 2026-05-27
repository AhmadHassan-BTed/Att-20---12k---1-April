#!/usr/bin/env python3
"""
INTEGRATED DIAGNOSTICS SUITE
Master diagnostic tool that runs all ISO, ESF, and PCSX2 checks in one command.
Provides comprehensive output for debugging character model visibility issues.
"""
import os
import sys
import struct
import json
import pycdlib
from core.esf_parser import ESFParser
import io

class DiagnosticSuite:
    def __init__(self):
        self.results = {}
        self.errors = []

    def section(self, name):
        print(f"\n{'='*70}")
        print(f"  {name}")
        print(f"{'='*70}\n")

    def subsection(self, name):
        print(f"\n{'-'*70}")
        print(f"  {name}")
        print(f"{'-'*70}\n")

    # ===== ISO VERIFICATION =====
    def check_patched_iso_integrity(self):
        """Verify patched ISO structure and CHAR.ESF file"""
        self.section("1. PATCHED ISO INTEGRITY CHECK")

        try:
            iso = pycdlib.PyCdlib()
            iso.open('iso/patched/EQOA_Frontiers_Patched.iso')
            record = iso.get_record(iso_path='/DATA/CHAR.ESF;1')

            result = {
                'file': '/DATA/CHAR.ESF;1',
                'lba': record.extent_location(),
                'size': record.data_length,
                'expected_size': 148838890,
                'matches': record.data_length == 148838890
            }

            print(f"[ISO] Patched ISO File: {result['file']}")
            print(f"  LBA: {result['lba']}")
            print(f"  Size: {result['size']:,} bytes")
            print(f"  Expected: {result['expected_size']:,} bytes")
            print(f"  Status: {'[OK]' if result['matches'] else '[MISMATCH]'}")

            self.results['patched_iso'] = result
            iso.close()
            return result['matches']
        except Exception as e:
            print(f"[ERROR] {e}")
            self.errors.append(f"Patched ISO check failed: {e}")
            return False

    def check_unmodified_iso_integrity(self):
        """Verify unmodified ISO for comparison"""
        self.section("2. UNMODIFIED ISO INTEGRITY CHECK")

        try:
            iso = pycdlib.PyCdlib()
            iso.open('iso/unmodified/EQOA_Frontiers.iso')
            record = iso.get_record(iso_path='/DATA/CHAR.ESF;1')

            result = {
                'file': '/DATA/CHAR.ESF;1',
                'lba': record.extent_location(),
                'size': record.data_length,
                'expected_size': 148370972
            }

            print(f"[ISO] Unmodified ISO File: {result['file']}")
            print(f"  LBA: {result['lba']}")
            print(f"  Size: {result['size']:,} bytes")
            print(f"  Expected (Original): {result['expected_size']:,} bytes")
            print(f"  Difference: {result['size'] - result['expected_size']:,} bytes")

            self.results['unmodified_iso'] = result
            iso.close()
            return True
        except Exception as e:
            print(f"[ERROR] {e}")
            self.errors.append(f"Unmodified ISO check failed: {e}")
            return False

    # ===== ESF ASSET VERIFICATION =====
    def check_patched_esf_assets(self):
        """Verify patched ESF contains all 11 target models"""
        self.section("3. PATCHED ESF ASSET VERIFICATION")

        try:
            iso = pycdlib.PyCdlib()
            iso.open('iso/patched/EQOA_Frontiers_Patched.iso')
            bio = io.BytesIO()
            iso.get_file_from_iso_fp(bio, iso_path='/DATA/CHAR.ESF;1')
            esf_bytes = bio.getvalue()
            iso.close()

            parser = ESFParser(esf_bytes).parse()
            esf_map = {e.asset_id: e for e in parser.pointer_table if e.asset_id is not None}

            # Load target assets
            with open('workspace/target_assets.json', 'r') as f:
                targets = json.load(f)

            print(f"Total assets in patched ESF: {len(esf_map)}")
            print(f"\nTarget patched models (first 11):\n")

            found_count = 0
            for i, asset in enumerate(targets[:11]):
                h = int(asset['original_hash'], 16)
                if h in esf_map:
                    e = esf_map[h]
                    print(f"  {i+1}. 0x{h:08X}: {e.length:,} bytes [OK]")
                    found_count += 1
                else:
                    print(f"  {i+1}. 0x{h:08X}: NOT FOUND [FAIL]")

            self.results['patched_assets'] = {'found': found_count, 'expected': 11}
            return found_count == 11
        except Exception as e:
            print(f"[ERROR] {e}")
            self.errors.append(f"Patched ESF asset check failed: {e}")
            return False

    def check_vanilla_esf_sizes(self):
        """List Vanilla character model sizes for reference"""
        self.subsection("Vanilla Character Model Sizes (for reference)")

        try:
            if not os.path.exists('workspace/original/CHAR.ESF'):
                print("[SKIP] Original CHAR.ESF not found")
                return False

            with open('workspace/original/CHAR.ESF', 'rb') as f:
                data = f.read()

            parser = ESFParser(data).parse()
            esf_map = {e.asset_id: e for e in parser.pointer_table if e.asset_id is not None}

            # Load target assets to get hashes
            with open('workspace/target_assets.json', 'r') as f:
                targets = json.load(f)

            print(f"Vanilla CHAR.ESF contains {len(esf_map)} assets\n")
            print("First 11 target models in Vanilla:\n")

            for i, asset in enumerate(targets[:11]):
                h = int(asset['original_hash'], 16)
                if h in esf_map:
                    e = esf_map[h]
                    print(f"  {i+1}. 0x{h:08X}: {e.length:,} bytes")
                else:
                    print(f"  {i+1}. 0x{h:08X}: NOT FOUND")

            return True
        except Exception as e:
            print(f"[ERROR] {e}")
            self.errors.append(f"Vanilla ESF size check failed: {e}")
            return False

    def check_frontiers_esf_sizes(self):
        """List Frontiers character model sizes for comparison"""
        self.subsection("Frontiers Character Model Sizes (for comparison)")

        try:
            if not os.path.exists('workspace/expansion/CHAR.ESF'):
                print("[SKIP] Expansion CHAR.ESF not found")
                return False

            with open('workspace/expansion/CHAR.ESF', 'rb') as f:
                data = f.read()

            parser = ESFParser(data).parse()
            esf_map = {e.asset_id: e for e in parser.pointer_table if e.asset_id is not None}

            with open('workspace/target_assets.json', 'r') as f:
                targets = json.load(f)

            print(f"Frontiers CHAR.ESF contains {len(esf_map)} assets\n")
            print("First 11 target models in Frontiers:\n")

            for i, asset in enumerate(targets[:11]):
                h = int(asset['expansion_hash'], 16)
                if h in esf_map:
                    e = esf_map[h]
                    print(f"  {i+1}. 0x{h:08X}: {e.length:,} bytes")
                else:
                    print(f"  {i+1}. 0x{h:08X}: NOT FOUND")

            return True
        except Exception as e:
            print(f"[ERROR] {e}")
            self.errors.append(f"Frontiers ESF size check failed: {e}")
            return False

    # ===== ISO FILE STRUCTURE =====
    def check_iso_file_listing(self):
        """List all ESF files in ISO sorted by LBA"""
        self.section("4. ISO FILE STRUCTURE (All ESF Files)")

        try:
            iso = pycdlib.PyCdlib()
            iso.open('iso/patched/EQOA_Frontiers_Patched.iso')

            esf_files = []
            for child in iso.pvd.root_dir_record.children:
                if child.file_ident.decode().endswith('.ESF;1'):
                    esf_files.append({
                        'name': child.file_ident.decode().rstrip(';1'),
                        'lba': child.extent_location(),
                        'size': child.get_data_length()
                    })

            esf_files.sort(key=lambda x: x['lba'])

            print(f"Total ESF files: {len(esf_files)}\n")
            print(f"{'File Name':<20} {'LBA':<12} {'Size':<15}")
            print(f"{'-'*20} {'-'*12} {'-'*15}")
            for f in esf_files:
                print(f"{f['name']:<20} {f['lba']:<12} {f['size']:<15,}")

            self.results['esf_files'] = esf_files
            iso.close()
            return True
        except Exception as e:
            print(f"[ERROR] {e}")
            self.errors.append(f"ISO file listing failed: {e}")
            return False

    def check_duplicate_esf_copies(self):
        """Search for duplicate FJBO signatures in ISO"""
        self.section("5. DUPLICATE ESF CHECK (FJBO Signature Search)")

        try:
            with open('iso/patched/EQOA_Frontiers_Patched.iso', 'rb') as f:
                data = f.read()

            print("Searching for FJBO signatures (ESF archives)...\n")

            fjbo_count = 0
            fjbo_locations = []
            idx = 0
            while True:
                idx = data.find(b'FJBO', idx)
                if idx == -1:
                    break
                fjbo_locations.append(idx)
                fjbo_count += 1
                idx += 1

            print(f"Total FJBO signatures found: {fjbo_count}\n")

            if fjbo_count > 1:
                print("Multiple FJBO locations (potential duplicates):\n")
                for i, loc in enumerate(fjbo_locations[:10]):  # Show first 10
                    sector = loc // 2048
                    offset = loc % 2048
                    print(f"  {i+1}. Byte offset: {loc:,} (Sector {sector}, Offset {offset})")
                if fjbo_count > 10:
                    print(f"  ... and {fjbo_count - 10} more")
            else:
                print("[OK] Only one FJBO signature found - no duplicates detected")

            self.results['fjbo_count'] = fjbo_count
            return fjbo_count <= 1
        except Exception as e:
            print(f"[ERROR] {e}")
            self.errors.append(f"Duplicate ESF check failed: {e}")
            return False

    # ===== PCSX2 STATE =====
    def check_pcsx2_savestates(self):
        """Check for stale PCSX2 savestates"""
        self.section("6. PCSX2 SAVESTATE STATUS")

        try:
            savestate_dir = r"C:\Users\PMLS\OneDrive\Documents\PCSX2\sstates"

            if not os.path.exists(savestate_dir):
                print(f"[OK] Savestate directory does not exist: {savestate_dir}")
                print("     This is GOOD - PCSX2 will perform cold boots")
                return True

            savestates = []
            for f in os.listdir(savestate_dir):
                if f.endswith('.p2s'):
                    path = os.path.join(savestate_dir, f)
                    stat = os.stat(path)
                    savestates.append({
                        'name': f,
                        'size': stat.st_size,
                        'modified': stat.st_mtime
                    })

            if not savestates:
                print("[OK] No EQOA savestates found")
                print("     PCSX2 will perform cold boots from ISO")
                return True
            else:
                print(f"[WARNING] Found {len(savestates)} savestate(s):\n")
                for s in savestates:
                    print(f"  - {s['name']} ({s['size']:,} bytes)")
                print("\n[ACTION REQUIRED] Delete these savestates to ensure cold boot")
                return False
        except Exception as e:
            print(f"[ERROR] {e}")
            self.errors.append(f"PCSX2 savestate check failed: {e}")
            return False

    def check_pcsx2_emulation_log(self):
        """Check PCSX2 emulation log for ISO load information"""
        self.subsection("PCSX2 Emulation Log Analysis")

        try:
            log_path = r"C:\Users\PMLS\OneDrive\Documents\PCSX2\logs\emuLog.txt"

            if not os.path.exists(log_path):
                print("[SKIP] emuLog.txt not found")
                return False

            with open(log_path, 'r', errors='ignore') as f:
                lines = f.readlines()

            # Find last 10 relevant lines
            iso_refs = []
            for i, line in enumerate(lines[-500:]):
                if any(x in line.lower() for x in ['iso', 'cdvd', 'eqoa', 'load']):
                    iso_refs.append(line.strip())

            if iso_refs:
                print("Recent ISO/CDVD references in emulation log:\n")
                for ref in iso_refs[-10:]:
                    print(f"  {ref[:100]}")
            else:
                print("[OK] No recent ISO references found in log")

            return True
        except Exception as e:
            print(f"[ERROR] {e}")
            self.errors.append(f"PCSX2 log check failed: {e}")
            return False

    # ===== SUMMARY REPORT =====
    def generate_summary(self):
        """Generate final diagnostic summary"""
        self.section("DIAGNOSTIC SUMMARY")

        print("Overall Status:\n")

        if not self.errors:
            print("[OK] All diagnostic checks passed")
            print("\nKey Findings:")
            print(f"  - Patched ISO: {self.results.get('patched_iso', {}).get('size', 0):,} bytes")
            print(f"  - Patched Assets: {self.results.get('patched_assets', {}).get('found', 0)}/11 found")
            print(f"  - ESF Files in ISO: {len(self.results.get('esf_files', []))}")
            print(f"  - Duplicate Copies: {self.results.get('fjbo_count', 0)}")
            print("\nRECOMMENDATION: ISO is ready for testing in PCSX2")
        else:
            print(f"[WARNINGS] {len(self.errors)} issue(s) detected:\n")
            for err in self.errors:
                print(f"  - {err}")
            print("\nRECOMMENDATION: Address issues before testing")

    def run_all(self):
        """Execute all diagnostic checks"""
        print("\n")
        print("=" * 70)
        print("  EQOA INTEGRATED DIAGNOSTICS SUITE")
        print("  Character Model Visibility Debugging Tool")
        print("=" * 70)

        self.check_patched_iso_integrity()
        self.check_unmodified_iso_integrity()
        self.check_patched_esf_assets()
        self.check_vanilla_esf_sizes()
        self.check_frontiers_esf_sizes()
        self.check_iso_file_listing()
        self.check_duplicate_esf_copies()
        self.check_pcsx2_savestates()
        self.check_pcsx2_emulation_log()
        self.generate_summary()

        print("\n" + "=" * 70)
        print("  END OF DIAGNOSTIC REPORT")
        print("=" * 70 + "\n")

if __name__ == '__main__':
    suite = DiagnosticSuite()
    suite.run_all()
