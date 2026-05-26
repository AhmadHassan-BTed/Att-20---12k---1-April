#!/usr/bin/env python3
"""
null_hypothesis.py
==================
NULL HYPOTHESIS: The characters were NEVER visible in either ISO.
The "invisible character" is the default behavior for an MMO that can't
connect to a game server and has no local character to display.

This tests:
1. Create a CONTROL ISO: copy of unmodified Frontiers, no changes at all
2. Compare our patched ISO side-by-side
3. If the control also shows no characters, then our patches are irrelevant

ALSO: Check what the CHARSEL files do — these control the character 
CREATION screen, which is the first place you'd see a character model.
"""
import struct, os, json

print("=" * 70)
print("  NULL HYPOTHESIS: Are characters visible in unmodified Frontiers?")
print("=" * 70)
print()

# Check CHARSEL structure — this is the CHARACTER SELECTION SCREEN
# CHARSEL*.CSF files in /DATA2/ control what models appear on the
# character creation/selection screen
print("[1] Examining CHARSEL files...")

charsel_files = [
    ('Frontiers CHARSEL1', 'workspace/CHARSEL1.CSF'),
    ('Original CHARSEL1', 'workspace/CHARSEL1_orig.CSF'),
]

for label, path in charsel_files:
    if os.path.exists(path):
        sz = os.path.getsize(path)
        with open(path, 'rb') as f:
            data = f.read(256)
        print(f"  {label}: {sz:,} bytes, header: {data[:16].hex()}")
    else:
        print(f"  {label}: NOT FOUND")

print()

# The key realization: CHARSEL files contain the models shown on the
# character creation screen. CHAR.ESF contains the models used in-game.
# If the user is on the CHARACTER CREATION screen, they see CHARSEL models.
# If they're in-game, they see CHAR.ESF models.

# Let's check if our target hashes appear in CHARSEL files
print("[2] Do our 11 target hashes appear in CHARSEL files?")
with open('workspace/target_assets.json') as f:
    targets = json.load(f)

target_hashes = [int(t['expansion_hash'], 16) for t in targets]
target_bytes = [struct.pack('<I', h) for h in target_hashes]

for label, path in [
    ('CHARSEL1.CSF', 'workspace/CHARSEL1.CSF'),
    ('CHARSEL2.CSF', 'workspace/CHARSEL2.CSF'),
    ('CHARSEL3.CSF', 'workspace/CHARSEL3.CSF'),
    ('CHARSEL4.CSF', 'workspace/CHARSEL4.CSF'),
    ('CHARCUST_Frontiers.CSF', 'workspace/CHARCUST_Frontiers.CSF'),
    ('CHARCUST_Frontiers.ESF', 'workspace/CHARCUST_Frontiers.ESF'),
    ('CHARFACE_Frontiers.CSF', 'workspace/CHARFACE_Frontiers.CSF'),
    ('CHARFACE_Frontiers.ESF', 'workspace/CHARFACE_Frontiers.ESF'),
]:
    if not os.path.exists(path):
        continue
    with open(path, 'rb') as f:
        data = f.read()
    found = []
    for i, (h, hb) in enumerate(zip(target_hashes, target_bytes)):
        count = data.count(hb)
        if count > 0:
            found.append((targets[i]['expansion_hash'], count))
    if found:
        print(f"  {label}: {found}")
    else:
        print(f"  {label}: No target hashes found")

print()

# Also search for ORIGINAL hashes (0x62700 models from vanilla)
print("[3] Do ORIGINAL model hashes (from vanilla) appear in CHARCUST?")
for label, path in [
    ('CHARCUST_Frontiers.ESF', 'workspace/CHARCUST_Frontiers.ESF'),
    ('CHARCUST_Original.ESF', 'workspace/CHARCUST_Original.ESF'),
]:
    if not os.path.exists(path):
        continue
    with open(path, 'rb') as f:
        data = f.read()
    
    # The CHARCUST might reference models by index number, not by hash
    # Let's check what kinds of values it contains
    
    # Dump all unique 4-byte values that look like model hashes
    # (values > 0x00010000 that aren't standard type IDs)
    hash_like = set()
    for i in range(0, len(data)-4, 4):
        v = struct.unpack_from('<I', data, i)[0]
        if 0x00010000 < v < 0xFFFF0000 and v not in (0x72700, 0x62700, 0x22000):
            hash_like.add(v)
    
    # See if any of our 11 target hashes are in there
    our_hashes = set(target_hashes)
    overlap = our_hashes & hash_like
    print(f"  {label}: {len(hash_like)} unique hash-like values, {len(overlap)} match our targets")
    if overlap:
        for h in sorted(overlap):
            print(f"    0x{h:08X}")

print()

# Critical check: compare CHARCUST between Original and Frontiers
print("[4] Are the CHARCUST files different between versions?")
for suffix in ['CSF', 'ESF']:
    fp = f'workspace/CHARCUST_Frontiers.{suffix}'
    op = f'workspace/CHARCUST_Original.{suffix}'
    if os.path.exists(fp) and os.path.exists(op):
        fsz = os.path.getsize(fp)
        osz = os.path.getsize(op)
        with open(fp, 'rb') as f:
            fd = f.read()
        with open(op, 'rb') as f:
            od = f.read()
        if fd == od:
            print(f"  CHARCUST.{suffix}: IDENTICAL between versions")
        else:
            # Count differences
            min_len = min(len(fd), len(od))
            diff_count = sum(1 for i in range(min_len) if fd[i] != od[i])
            print(f"  CHARCUST.{suffix}: DIFFERENT! {diff_count:,} byte diffs (Frontiers={fsz:,}, Original={osz:,})")
    else:
        print(f"  CHARCUST.{suffix}: one or both missing")

print()
print("[5] WHERE in the game flow is the user?")
print("  The logs show DEV9/SMAP network activity + VU1 programs loading.")
print("  This suggests the game has booted and is either:")
print("    A) At the title/login screen (no characters visible by default)")
print("    B) At character selection (characters from CHARSEL*.CSF, NOT CHAR.ESF)")
print("    C) In-game (characters from CHAR.ESF loaded by server)")
print()
print("  If (A): Characters are not expected to be visible!")
print("  If (B): We need to patch CHARSEL, not CHAR.ESF!")
print("  If (C): This requires a server to send character spawn data!")
print()
print("  >>> QUESTION FOR USER: What screen are you on when characters are invisible? <<<")
print("  >>> A screenshot would help identify whether this is login, char select, or in-game. <<<")
