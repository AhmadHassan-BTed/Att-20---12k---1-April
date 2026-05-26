# #!/usr/bin/env python3
# """
# EQOA Surgical Texture/Palette Swapper
# =====================================
# A production-grade, layout-aware surgical binary texture patcher.
# Transplants Vanilla character textures and palettes into native Frontiers
# skeletons to preserve original visual aesthetics while ensuring 100% native
# compatibility with the Frontiers 43-bone skinning pipeline and Vector Unit engine.
# """

# import os
# import sys
# import struct
# import glob
# from esf_parser import ESFParser

# def parse_node(data, pos):
#     """Recursively parse a binary node tree."""
#     if pos + 12 > len(data):
#         return None, pos
        
#     type_id = struct.unpack_from('<I', data, pos)[0]
#     data_size = struct.unpack_from('<I', data, pos + 4)[0]
#     child_count = struct.unpack_from('<I', data, pos + 8)[0]
    
#     node = {
#         'type_id': type_id,
#         'data_size': data_size,
#         'child_count': child_count,
#         'children': [],
#         'inline_data': None
#     }
    
#     next_pos = pos + 12
#     if child_count == 0:
#         if next_pos + data_size > len(data):
#             raise EOFError(f"Unexpected EOF reading leaf node at 0x{next_pos:X} (expected {data_size} bytes)")
#         node['inline_data'] = data[next_pos : next_pos + data_size]
#         next_pos += data_size
#     else:
#         for _ in range(child_count):
#             child, next_pos = parse_node(data, next_pos)
#             if child is not None:
#                 node['children'].append(child)
                
#     return node, next_pos

# def update_node_sizes(node):
#     """Recursively calculate and update correct data_size for every node in the tree."""
#     if node['child_count'] == 0:
#         node['data_size'] = len(node['inline_data'])
#     else:
#         size = 0
#         for child in node['children']:
#             update_node_sizes(child)
#             size += 12 + child['data_size']
#         node['data_size'] = size

# def serialize_node(node):
#     """Recursively serialize a node tree to binary bytes."""
#     data = bytearray()
#     header = struct.pack('<III', node['type_id'], node['data_size'], node['child_count'])
#     data.extend(header)
    
#     if node['child_count'] == 0:
#         if node['inline_data'] is not None:
#             data.extend(node['inline_data'])
#     else:
#         for child in node['children']:
#             data.extend(serialize_node(child))
            
#     return bytes(data)

# def perform_texture_swaps():
#     payload_dir = './workspace/payloads'
#     frontiers_esf_path = './workspace/expansion/CHAR.ESF'
    
#     print("[*] Commencing Surgical Texture/Palette Splicing...")
    
#     if not os.path.exists(frontiers_esf_path):
#         print(f"[-] Error: Frontiers ESF not found at '{frontiers_esf_path}'")
#         sys.exit(1)
        
#     print("[*] Parsing Frontiers CHAR.ESF to locate native templates...")
#     with open(frontiers_esf_path, 'rb') as f:
#         frontiers_data = f.read()
#     frontiers_parser = ESFParser(frontiers_data).parse()
    
#     # Create index mapping of asset_hash -> PointerTableEntry in Frontiers ESF
#     frontiers_map = { entry.asset_id: entry for entry in frontiers_parser.pointer_table if entry.asset_id is not None }
    
#     bin_files = sorted(glob.glob(os.path.join(payload_dir, '*.bin')))
#     if not bin_files:
#         print("[-] Error: No payloads found in workspace/payloads directory.")
#         sys.exit(1)
        
#     swapped_count = 0
#     skipped_count = 0
    
#     for filepath in bin_files:
#         filename = os.path.basename(filepath)
#         try:
#             hash_str = filename.split('_')[1].split('.')[0]
#             asset_hash = int(hash_str, 16)
#         except Exception:
#             continue
            
#         with open(filepath, 'rb') as f:
#             vanilla_bytes = f.read()
            
#         if len(vanilla_bytes) < 12:
#             continue
            
#         # Parse Vanilla Node
#         try:
#             vanilla_node, _ = parse_node(vanilla_bytes, 0)
#         except Exception as e:
#             print(f"  [-] Failed to parse Vanilla payload {filename}: {e}")
#             continue
            
#         # We transplant textures for any branch model node
#         if vanilla_node['child_count'] == 0:
#             continue
            
#         # Check if texture container (type 0x11110) exists in Vanilla
#         vanilla_tex_containers = [c for c in vanilla_node['children'] if c['type_id'] == 0x11110]
#         if not vanilla_tex_containers:
#             continue
#         vanilla_tex = vanilla_tex_containers[0]
        
#         # Check if asset hash exists in Frontiers
#         if asset_hash not in frontiers_map:
#             print(f"  [-] Hash 0x{asset_hash:08X} does not exist in Frontiers. Retaining pristine Vanilla format.")
#             skipped_count += 1
#             continue
            
#         # Extract native Frontiers model
#         entry = frontiers_map[asset_hash]
#         frontiers_bytes = frontiers_data[entry.offset : entry.offset + entry.length]
        
#         try:
#             frontiers_node, _ = parse_node(frontiers_bytes, 0)
#         except Exception as e:
#             print(f"  [-] Failed to parse native Frontiers model for 0x{asset_hash:08X}: {e}")
#             continue
            
#         # Locate texture container node index in Frontiers
#         fro_tex_indices = [i for i, c in enumerate(frontiers_node['children']) if c['type_id'] == 0x11110]
#         if not fro_tex_indices:
#             print(f"  [-] Warning: Native Frontiers model 0x{asset_hash:08X} lacks texture container node!")
#             continue
#         idx = fro_tex_indices[0]
        
#         # Perform Surgical Transplant
#         frontiers_node['children'][idx] = vanilla_tex
        
#         # Recalculate sizes recursively across Frontiers tree
#         update_node_sizes(frontiers_node)
        
#         # Serialize the resulting hybrid node tree
#         swapped_bytes = serialize_node(frontiers_node)
        
#         # Overwrite the payload file with the hybrid model
#         with open(filepath, 'wb') as f:
#             f.write(swapped_bytes)
            
#         print(f"  [+] Surgically Grafted textures onto native Frontiers skeleton for 0x{asset_hash:08X}:")
#         print(f"      - Original Vanilla size:   {len(vanilla_bytes):,} bytes")
#         print(f"      - Frontiers template size: {len(frontiers_bytes):,} bytes")
#         print(f"      - Hybrid swapped size:     {len(swapped_bytes):,} bytes")
#         swapped_count += 1
        
#     print(f"\n[+] TEXTURE SPLICING COMPLETE: {swapped_count} models grafted with original Vanilla aesthetics!")
#     print(f"    (Skipped {skipped_count} Vanilla-only sub-assets/dependencies to preserve backward compatibility)")

# if __name__ == '__main__':
#     perform_texture_swaps()

#!/usr/bin/env python3
"""
frankenstein_texture_swapper.py  —  v2: Deep Structural Translation
====================================================================
Transplants Vanilla character textures and palettes into native Frontiers
skeleton nodes with full internal-pointer recalculation and GS material
flag diffing.

WHY v1 CRASHED (TLB miss, addr=0x4, EE thread soft-lock)
---------------------------------------------------------
v1 performed a blind block-swap: it copied the entire 0x11110 Vanilla node
as raw bytes into the Frontiers tree.

  The 0x01000 leaf nodes store a pixel_data_offset and clut_data_offset as
  absolute-within-blob DWORDs.  When the Vanilla blob was a different size
  than the Frontiers blob it replaced, those offsets no longer pointed to
  valid data.  The engine read pixel_data_offset, got a stale value that
  resolved to 0x00000000, then tried to read the texture header at
  NULL + 0x04 = 0x4 → EE TLB miss, game freeze.

  Additionally, the 0x31100 GS material state node stores a GIF A+D packet
  whose TEX0 register encodes PSM (pixel format), TW/TH (log2 dimensions),
  CPSM (CLUT format), and CBP (CLUT base pointer in GS VRAM).  Blindly
  keeping the Vanilla CBP in a Frontiers GS memory layout pointed to an
  invalid GS VRAM region, triggering a second class of null-pointer error
  the first time the engine touched the CLUT during draw-call setup.

v2 FIX STRATEGY
---------------
  For each texture slot (paired 0x01000 + 0x31100 nodes, matched by index):

  1.  Parse the Vanilla 0x01000 inline_data to extract:
        psm, cpsm, width, height, pixel_data (raw pixels), clut_data (palette).

  2.  Rebuild the 0x01000 blob from scratch using the Frontiers header bytes
      as the base (preserving unknown engine flags), then appending Vanilla
      pixel_data + clut_data and recalculating pixel_data_offset,
      clut_data_offset, and total_size so they precisely describe the new blob.

  3.  Parse the Frontiers 0x31100 inline_data as a GIF A+D packet.
      Locate the TEX0 register entry and rewrite ONLY the format fields:
        PSM  ← vanilla_td.psm
        TW   ← floor(log2(vanilla_td.width))
        TH   ← floor(log2(vanilla_td.height))
        TCC  ← 1 if RGBA (PSMCT32) else preserve Frontiers value
        CPSM ← vanilla_td.cpsm  (if paletted)
        CBP  ← 0   (zero: engine re-allocates CLUT in GS VRAM at load time)
        CSA  ← 0   (zero: offset from start of CLUT)
        CLD  ← 1   (1 = LOAD: instruct GS to upload CLUT on next draw call)
      All other GS registers (TEX1, CLAMP, ALPHA, blending, filtering) are
      left EXACTLY as Frontiers shipped them — only TEX0 format bytes change.

  4.  Replace the 0x11110 subtree in the Frontiers asset, call
      update_node_sizes() to cascade correct data_size values up the tree,
      then serialize and write back to the payload .bin.

Node type reference (EQOA ESF)
  0x11110  texture container  — branch; N direct children (one per texture slot)
  0x01000  texture leaf       — leaf; inline_data = header + pixel blob + CLUT blob
  0x31100  GS material state  — leaf; inline_data = GIF A+D register packet

Dependencies: Python 3.8+ stdlib only + esf_parser.py (same directory).
"""

import os
import sys
import math
import struct
import glob
import copy
import traceback
from esf_parser import ESFParser


# ─────────────────────────────────────────────────────────────────────────────
# PS2 GS Constants
# ─────────────────────────────────────────────────────────────────────────────

# GS register addresses used in GIF A+D data packets.
# Ref: PS2 GS User's Manual rev 6.0, Chapter 7.
GS_REG_TEX0_1  = 0x06   # texture buffer params, CLUT params, pixel format (context 1)
GS_REG_TEX0_2  = 0x16   # same, context 2
GS_REG_TEX1_1  = 0x07   # texture filter / LOD settings (context 1)
GS_REG_CLAMP_1 = 0x08   # UV wrap mode (context 1)
GS_REG_ALPHA_1 = 0x42   # alpha-blend equation (context 1)
GS_REG_MIPTBP1 = 0x34   # mipmap base pointer level 1–3

# GS Pixel Storage Mode constants (6-bit PSM field in TEX0).
PSM_PSMCT32  = 0x00   # 32-bit RGBA8888 — direct colour, no CLUT
PSM_PSMCT24  = 0x01   # 24-bit RGB888   — direct colour
PSM_PSMCT16  = 0x02   # 16-bit RGBA1555 — direct colour
PSM_PSMT8    = 0x13   # 8-bit indexed   — 256-colour paletted
PSM_PSMT4    = 0x14   # 4-bit indexed   — 16-colour paletted
PSM_PSMT8H   = 0x1B   # 8-bit stored in alpha of 32-bit block
PSM_PSMT4HL  = 0x24   # 4-bit low-nibble
PSM_PSMT4HH  = 0x2C   # 4-bit high-nibble

PSM_NAMES = {
    PSM_PSMCT32: 'PSMCT32', PSM_PSMCT24: 'PSMCT24', PSM_PSMCT16: 'PSMCT16',
    PSM_PSMT8:   'PSMT8',   PSM_PSMT4:   'PSMT4',   PSM_PSMT8H:  'PSMT8H',
    PSM_PSMT4HL: 'PSMT4HL', PSM_PSMT4HH: 'PSMT4HH',
}
PSM_PALETTED = frozenset({PSM_PSMT8, PSM_PSMT4, PSM_PSMT8H, PSM_PSMT4HL, PSM_PSMT4HH})


# ─────────────────────────────────────────────────────────────────────────────
# 0x01000 texture leaf — header field byte offsets
# ─────────────────────────────────────────────────────────────────────────────
#
# EQOA embeds a 32-byte custom header at the start of every 0x01000 node's
# inline_data.  Field positions derived from compare_textures.py DWORD dump.
#
# Offset  Type     Field
# 0x00    uint32   total_blob_size  (== len(inline_data); engine sanity check)
# 0x04    uint32   pixel_data_size  (byte count of raw pixel block)
# 0x08    uint32   clut_data_size   (byte count of palette block; 0 if direct)
# 0x0C    uint8    psm              (GS pixel storage mode, PSM_* constant)
# 0x0D    uint8    cpsm             (GS CLUT storage mode)
# 0x0E    uint16   width            (texture width in pixels)
# 0x10    uint16   height           (texture height in pixels)
# 0x12    uint16   engine_flags     (misc engine-internal flags; preserve verbatim)
# 0x14    uint32   pixel_data_offset  (byte offset FROM START OF blob to pixel data)
# 0x18    uint32   clut_data_offset   (byte offset FROM START OF blob to CLUT data)
# 0x1C    uint32   reserved         (preserve verbatim)
# ── total: 32 bytes ──────────────────────────────────────────────────────────
#
# Following the 32-byte header there may be zero or more bytes of additional
# sub-header data (mipmaps descriptors, LOD ranges, etc.) before the pixel
# block begins.  pixel_data_offset tells us where that gap ends.

TEX_HDR_TOTAL_SIZE    = 0x00
TEX_HDR_PIXEL_SIZE    = 0x04
TEX_HDR_CLUT_SIZE     = 0x08
TEX_HDR_PSM           = 0x0C
TEX_HDR_CPSM          = 0x0D
TEX_HDR_WIDTH         = 0x0E
TEX_HDR_HEIGHT        = 0x10
TEX_HDR_FLAGS         = 0x12
TEX_HDR_PIXEL_OFFSET  = 0x14
TEX_HDR_CLUT_OFFSET   = 0x18
TEX_HDR_RESERVED      = 0x1C
TEX_HDR_SIZE          = 0x20  # 32 bytes

# TIM2 file magic (alternative embedded format)
TIM2_MAGIC = b'TIM2'


# ─────────────────────────────────────────────────────────────────────────────
# TEX0 GS register bit-field layout
# ─────────────────────────────────────────────────────────────────────────────
#
# TEX0 is a 64-bit GS register.  Bits are packed little-endian.
# Only the fields we actively translate are listed; the rest are masked off.
#
# (field_name, first_bit, bit_count)
_TEX0_FIELDS = [
    ('TBP0', 0,  14),   # texture buffer base pointer (/64)  — keep Frontiers value
    ('TBW',  14,  6),   # texture buffer width (/64 pixels)  — keep Frontiers value
    ('PSM',  20,  6),   # pixel storage mode                 ← PATCH from Vanilla
    ('TW',   26,  4),   # log2(texture width)                ← PATCH from Vanilla
    ('TH',   30,  4),   # log2(texture height)               ← PATCH from Vanilla
    ('TCC',  34,  1),   # transparency: 0=RGB, 1=RGBA        ← PATCH from Vanilla
    ('TFX',  35,  2),   # texture function (modulate/decal)  — keep Frontiers value
    ('CBP',  37, 14),   # CLUT base pointer (/64)            ← ZERO (runtime alloc)
    ('CPSM', 51,  4),   # CLUT pixel format                  ← PATCH from Vanilla
    ('CSM',  55,  1),   # CLUT storage mode                  ← PATCH from Vanilla
    ('CSA',  56,  5),   # CLUT entry offset                  ← ZERO
    ('CLD',  61,  3),   # CLUT load control                  ← SET to 1 if paletted
]

def _extract_bits(val: int, offset: int, count: int) -> int:
    return (val >> offset) & ((1 << count) - 1)

def _insert_bits(target: int, val: int, offset: int, count: int) -> int:
    mask = ((1 << count) - 1) << offset
    return (target & ~mask) | ((val & ((1 << count) - 1)) << offset)

def parse_tex0(reg64: int) -> dict:
    """Unpack all named fields from a 64-bit TEX0 register value."""
    return {name: _extract_bits(reg64, off, cnt) for name, off, cnt in _TEX0_FIELDS}

def encode_tex0(fields: dict) -> int:
    """Pack a named-field dict back into a 64-bit TEX0 value."""
    val = 0
    for name, off, cnt in _TEX0_FIELDS:
        val = _insert_bits(val, fields.get(name, 0), off, cnt)
    return val


# ─────────────────────────────────────────────────────────────────────────────
# Node tree utilities
# ─────────────────────────────────────────────────────────────────────────────

def parse_node(data: bytes, pos: int) -> tuple:
    """Recursively deserialize a binary ESF node at byte offset *pos*."""
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


def find_nodes_by_type(node: dict, type_id: int, results: list):
    """Depth-first collect all nodes whose type_id matches *type_id*."""
    if node is None:
        return
    if node['type_id'] == type_id:
        results.append(node)
    for child in node.get('children', []):
        find_nodes_by_type(child, type_id, results)


def update_node_sizes(node: dict):
    """
    Bottom-up recomputation of data_size.
    Must be called after ANY structural change before serialize_node().
    Also syncs child_count to len(children) so the header stays honest.
    """
    if node['child_count'] == 0:
        node['data_size'] = len(node['inline_data']) if node['inline_data'] else 0
    else:
        node['child_count'] = len(node['children'])  # keep in sync
        total = 0
        for child in node['children']:
            update_node_sizes(child)
            total += 12 + child['data_size']
        node['data_size'] = total


def serialize_node(node: dict) -> bytes:
    """Recursively emit a node tree to binary bytes (12-byte header + payload)."""
    buf = bytearray()
    buf += struct.pack('<III', node['type_id'], node['data_size'], node['child_count'])
    if node['child_count'] == 0:
        if node['inline_data']:
            buf += node['inline_data']
    else:
        for child in node['children']:
            buf += serialize_node(child)
    return bytes(buf)


# ─────────────────────────────────────────────────────────────────────────────
# SECTION A  —  0x01000 texture leaf deep parser
# ─────────────────────────────────────────────────────────────────────────────

class TextureDescriptor:
    """
    Parsed content of a single 0x01000 leaf node.
    Holds the format metadata and the raw pixel / CLUT byte blobs.
    """
    __slots__ = ('psm', 'cpsm', 'width', 'height', 'engine_flags', 'reserved',
                 'pixel_data', 'clut_data', 'inter_header')

    def __init__(self, psm, cpsm, width, height, engine_flags, reserved,
                 pixel_data, clut_data, inter_header):
        self.psm          = psm           # GS PSM value
        self.cpsm         = cpsm          # GS CPSM value
        self.width        = width
        self.height       = height
        self.engine_flags = engine_flags  # opaque; preserve from Frontiers side
        self.reserved     = reserved      # opaque; preserve from Frontiers side
        self.pixel_data   = pixel_data    # raw pixels (Vanilla content)
        self.clut_data    = clut_data     # raw palette (Vanilla content; may be b'')
        # inter_header: bytes between fixed 32-byte header and pixel_data_offset.
        # May contain mipmap descriptors, extra LOD data, etc.  Preserved verbatim.
        self.inter_header = inter_header

    @property
    def is_paletted(self):
        return self.psm in PSM_PALETTED

    @property
    def tw(self):
        """log2(width) — safe: clamps non-power-of-2 to nearest valid value."""
        return max(0, int(math.log2(self.width))) if self.width >= 1 else 0

    @property
    def th(self):
        return max(0, int(math.log2(self.height))) if self.height >= 1 else 0

    def __repr__(self):
        psm_str = PSM_NAMES.get(self.psm, hex(self.psm))
        return (f"TexDesc(psm={psm_str}, {self.width}x{self.height}, "
                f"pixels={len(self.pixel_data)}, clut={len(self.clut_data)})")


def parse_texture_descriptor(inline_data: bytes) -> 'TextureDescriptor':
    """
    Parse the inline_data of a 0x01000 node into a TextureDescriptor.
    Supports:
      • EQOA native format (32-byte header with offset fields)
      • Standard TIM2 (detected by 'TIM2' magic at byte 0)
    """
    if len(inline_data) < TEX_HDR_SIZE:
        raise ValueError(f"0x01000 inline_data only {len(inline_data)} B "
                         f"(minimum {TEX_HDR_SIZE})")

    # ── TIM2 fast-path ───────────────────────────────────────────────────────
    if inline_data[:4] == TIM2_MAGIC:
        return _parse_tim2_descriptor(inline_data)

    # ── EQOA native header ───────────────────────────────────────────────────
    total_blob_size  = struct.unpack_from('<I', inline_data, TEX_HDR_TOTAL_SIZE )[0]
    pixel_data_size  = struct.unpack_from('<I', inline_data, TEX_HDR_PIXEL_SIZE )[0]
    clut_data_size   = struct.unpack_from('<I', inline_data, TEX_HDR_CLUT_SIZE  )[0]
    psm              = struct.unpack_from('<B', inline_data, TEX_HDR_PSM        )[0]
    cpsm             = struct.unpack_from('<B', inline_data, TEX_HDR_CPSM       )[0]
    width            = struct.unpack_from('<H', inline_data, TEX_HDR_WIDTH      )[0]
    height           = struct.unpack_from('<H', inline_data, TEX_HDR_HEIGHT     )[0]
    engine_flags     = struct.unpack_from('<H', inline_data, TEX_HDR_FLAGS      )[0]
    pixel_data_offset= struct.unpack_from('<I', inline_data, TEX_HDR_PIXEL_OFFSET)[0]
    clut_data_offset = struct.unpack_from('<I', inline_data, TEX_HDR_CLUT_OFFSET)[0]
    reserved         = struct.unpack_from('<I', inline_data, TEX_HDR_RESERVED   )[0]

    # Validate pixel offset
    if pixel_data_offset < TEX_HDR_SIZE:
        raise ValueError(f"pixel_data_offset 0x{pixel_data_offset:X} < header size; "
                         f"header layout assumption is wrong — run compare_textures.py "
                         f"and inspect the DWORD dump to find the correct field positions.")
    if pixel_data_offset + pixel_data_size > len(inline_data):
        raise ValueError(f"pixel slice [0x{pixel_data_offset:X} : "
                         f"0x{pixel_data_offset+pixel_data_size:X}] exceeds blob "
                         f"({len(inline_data)} B)")

    pixel_data  = inline_data[pixel_data_offset : pixel_data_offset + pixel_data_size]
    inter_header = inline_data[TEX_HDR_SIZE : pixel_data_offset]  # may be b''

    clut_data = b''
    if clut_data_size > 0:
        if clut_data_offset < TEX_HDR_SIZE:
            raise ValueError(f"clut_data_offset 0x{clut_data_offset:X} < header size")
        if clut_data_offset + clut_data_size > len(inline_data):
            raise ValueError(f"CLUT slice out of bounds in blob ({len(inline_data)} B)")
        clut_data = inline_data[clut_data_offset : clut_data_offset + clut_data_size]

    return TextureDescriptor(
        psm=psm, cpsm=cpsm, width=width, height=height,
        engine_flags=engine_flags, reserved=reserved,
        pixel_data=pixel_data, clut_data=clut_data,
        inter_header=inter_header,
    )


def _parse_tim2_descriptor(inline_data: bytes) -> 'TextureDescriptor':
    """
    Parse a TIM2-format blob embedded inside a 0x01000 node.

    TIM2 file header  (16 bytes):   magic[4], version[1], fmt[1], num_images[2], reserved[8]
    TIM2 surface hdr  (48 bytes):   total[4], clut_sz[4], img_sz[4], hdr_sz[2],
                                    clut_clr_count[2], img_fmt[1], mipmap_cnt[1],
                                    clut_type[1], bpp[1], width[2], height[2],
                                    gs_tex0[8], gs_tex1[8], gs_flags[4], gs_texclut[4]
    """
    TIM2_FILE_HDR_SZ = 16
    TIM2_SURF_HDR_SZ = 48
    MIN_LEN = TIM2_FILE_HDR_SZ + TIM2_SURF_HDR_SZ

    if len(inline_data) < MIN_LEN:
        raise ValueError(f"TIM2 blob too short: {len(inline_data)} < {MIN_LEN}")

    # Surface header starts at offset 16
    S = TIM2_FILE_HDR_SZ
    clut_sz   = struct.unpack_from('<I', inline_data, S +  4)[0]
    img_sz    = struct.unpack_from('<I', inline_data, S +  8)[0]
    hdr_sz    = struct.unpack_from('<H', inline_data, S + 12)[0]
    width     = struct.unpack_from('<H', inline_data, S + 24)[0]
    height    = struct.unpack_from('<H', inline_data, S + 26)[0]
    gs_tex0   = struct.unpack_from('<Q', inline_data, S + 28)[0]

    tex0_fields = parse_tex0(gs_tex0)
    psm  = tex0_fields['PSM']
    cpsm = tex0_fields['CPSM']

    pixel_start = TIM2_FILE_HDR_SZ + hdr_sz
    pixel_data  = inline_data[pixel_start : pixel_start + img_sz]
    clut_data   = inline_data[pixel_start + img_sz : pixel_start + img_sz + clut_sz]

    return TextureDescriptor(
        psm=psm, cpsm=cpsm, width=width, height=height,
        engine_flags=0, reserved=0,
        pixel_data=pixel_data, clut_data=clut_data,
        inter_header=b'',
    )


def rebuild_tex_inline_data(vanilla_td: 'TextureDescriptor',
                             frontiers_inline: bytes) -> bytes:
    """
    Construct a new 0x01000 inline_data blob that contains:
      • The Frontiers node's 32-byte header as a base (preserves engine_flags,
        reserved, and any other opaque fields the Frontiers engine relies on).
      • The Frontiers node's inter_header bytes (mipmap descriptors etc.).
      • Vanilla's raw pixel_data.
      • Vanilla's raw clut_data.

    The fields pixel_data_size, clut_data_size, pixel_data_offset,
    clut_data_offset, total_blob_size, psm, cpsm, width, and height are
    recomputed from scratch so every offset in the blob is geometrically
    correct.  This is what prevents the EE from dereferencing a stale
    pointer to address 0x0 and soft-locking.
    """
    if len(frontiers_inline) < TEX_HDR_SIZE:
        # Cannot read Frontiers header; emit pure Vanilla blob as fallback.
        return frontiers_inline

    # Grab the inter_header (sub-header gap) from the Frontiers blob.
    fro_pixel_offset = struct.unpack_from('<I', frontiers_inline, TEX_HDR_PIXEL_OFFSET)[0]
    fro_engine_flags = struct.unpack_from('<H', frontiers_inline, TEX_HDR_FLAGS      )[0]
    fro_reserved     = struct.unpack_from('<I', frontiers_inline, TEX_HDR_RESERVED   )[0]

    if fro_pixel_offset >= TEX_HDR_SIZE:
        inter_header = frontiers_inline[TEX_HDR_SIZE : fro_pixel_offset]
    else:
        inter_header = b''

    # Compute new layout offsets.
    new_pixel_offset = TEX_HDR_SIZE + len(inter_header)
    new_clut_offset  = new_pixel_offset + len(vanilla_td.pixel_data)
    new_total        = new_clut_offset  + len(vanilla_td.clut_data)

    # Build the 32-byte header with all corrected fields.
    header = bytearray(TEX_HDR_SIZE)
    struct.pack_into('<I', header, TEX_HDR_TOTAL_SIZE,   new_total)
    struct.pack_into('<I', header, TEX_HDR_PIXEL_SIZE,   len(vanilla_td.pixel_data))
    struct.pack_into('<I', header, TEX_HDR_CLUT_SIZE,    len(vanilla_td.clut_data))
    struct.pack_into('<B', header, TEX_HDR_PSM,          vanilla_td.psm)
    struct.pack_into('<B', header, TEX_HDR_CPSM,         vanilla_td.cpsm)
    struct.pack_into('<H', header, TEX_HDR_WIDTH,        vanilla_td.width)
    struct.pack_into('<H', header, TEX_HDR_HEIGHT,       vanilla_td.height)
    struct.pack_into('<H', header, TEX_HDR_FLAGS,        fro_engine_flags)   # Frontiers flags
    struct.pack_into('<I', header, TEX_HDR_PIXEL_OFFSET, new_pixel_offset)
    struct.pack_into('<I', header, TEX_HDR_CLUT_OFFSET,  new_clut_offset)
    struct.pack_into('<I', header, TEX_HDR_RESERVED,     fro_reserved)       # Frontiers reserved

    blob = bytearray()
    blob += header
    blob += inter_header
    blob += vanilla_td.pixel_data
    blob += vanilla_td.clut_data
    return bytes(blob)


# ─────────────────────────────────────────────────────────────────────────────
# SECTION B  —  0x31100 GS material state translator
# ─────────────────────────────────────────────────────────────────────────────

def _parse_gif_tag(data: bytes) -> tuple:
    """
    Read the 16-byte GIF tag at the start of *data*.
    Returns (nloop, nreg, flg) where:
      nloop = packet-repeat count
      nreg  = register descriptors per packet (0 in the tag means 16)
      flg   = data format (0=PACKED, 1=REGLIST, 2/3=IMAGE)
    """
    if len(data) < 16:
        raise ValueError("GIF tag requires at least 16 bytes")
    lo = struct.unpack_from('<Q', data, 0)[0]
    nloop = lo & 0x7FFF
    flg   = (lo >> 58) & 0x3
    nreg  = (lo >> 52) & 0xF
    if nreg == 0:
        nreg = 16
    return nloop, nreg, flg


def patch_tex0_in_gif_packet(raw: bytes, new_tex0_val: int) -> bytes | None:
    """
    Scan a GIF A+D packet (inline_data of a 0x31100 node) for the TEX0
    register entry and overwrite its 64-bit value with *new_tex0_val*.

    Handles both TEX0_1 (0x06) and TEX0_2 (0x16) register addresses.
    Returns the patched bytes on success, or None if TEX0 was not found
    (so the caller can fall back gracefully).

    We do NOT re-encode the entire GIF packet — we patch only the 8-byte
    value field in-place.  This preserves every other register's data
    (TEX1, CLAMP, ALPHA, MIPTBP, etc.) exactly as Frontiers ships it.
    """
    try:
        nloop, nreg, flg = _parse_gif_tag(raw)
    except ValueError:
        return None

    if flg != 0:
        # Not PACKED format — we cannot navigate it safely.
        return None

    buf = bytearray(raw)
    pos = 16  # skip GIF tag

    for _ in range(nloop * nreg):
        if pos + 16 > len(raw):
            break
        # In A+D format each 16-byte entry is:  value[8]  addr[8]
        addr = struct.unpack_from('<Q', raw, pos + 8)[0]
        reg  = addr & 0xFF

        if reg in (GS_REG_TEX0_1, GS_REG_TEX0_2):
            struct.pack_into('<Q', buf, pos, new_tex0_val)
            return bytes(buf)

        pos += 16

    return None  # TEX0 not present in this packet


def translate_material_node(vanilla_mat_node: dict,
                             frontiers_mat_node: dict,
                             vanilla_td: 'TextureDescriptor',
                             label: str) -> dict:
    """
    Produce a new 0x31100 node that combines:
      • Frontiers' GIF packet as the base (all pipeline state preserved).
      • A re-encoded TEX0 register where format fields are overwritten to
        describe the Vanilla pixel data accurately.

    If the GIF packet cannot be parsed or TEX0 is absent, the function
    returns the Frontiers node verbatim and logs a diagnostic message.
    The game will not crash — it will just render with the wrong texture
    format (likely a garbled palette), which is far better than a TLB miss.
    """
    result = copy.deepcopy(frontiers_mat_node)

    fro_raw = frontiers_mat_node.get('inline_data', b'')
    van_raw = vanilla_mat_node.get('inline_data', b'')

    if not fro_raw:
        print(f"      [warn] {label}: empty Frontiers 0x31100; returning verbatim.")
        return result

    # ── Parse the Frontiers TEX0 to use as base ──────────────────────────────
    try:
        nloop, nreg, flg = _parse_gif_tag(fro_raw)
    except ValueError as e:
        print(f"      [warn] {label}: cannot parse Frontiers GIF tag: {e}. "
              f"Frontiers material retained.")
        return result

    if flg != 0:
        print(f"      [warn] {label}: Frontiers 0x31100 is GIF FLG={flg} "
              f"(not PACKED A+D). Cannot patch TEX0; Frontiers material retained.")
        return result

    # Find the existing TEX0 value in the Frontiers packet
    fro_tex0_val = None
    pos = 16
    for _ in range(nloop * nreg):
        if pos + 16 > len(fro_raw):
            break
        val  = struct.unpack_from('<Q', fro_raw, pos    )[0]
        addr = struct.unpack_from('<Q', fro_raw, pos + 8)[0]
        if (addr & 0xFF) in (GS_REG_TEX0_1, GS_REG_TEX0_2):
            fro_tex0_val = val
            break
        pos += 16

    if fro_tex0_val is None:
        print(f"      [warn] {label}: TEX0 register absent from Frontiers GIF packet. "
              f"Cannot patch; Frontiers material retained.")
        return result

    # ── Build merged TEX0 ────────────────────────────────────────────────────
    base   = parse_tex0(fro_tex0_val)   # Frontiers TEX0 — our base
    merged = dict(base)                 # copy; we'll overwrite specific fields

    # Fields we REPLACE with Vanilla-derived values:
    merged['PSM']  = vanilla_td.psm
    merged['TW']   = vanilla_td.tw
    merged['TH']   = vanilla_td.th

    # TCC: set RGBA flag if the source is 32-bit RGBA, otherwise follow Frontiers
    if vanilla_td.psm == PSM_PSMCT32:
        merged['TCC'] = 1   # RGBA — transparency enabled
    elif vanilla_td.psm in (PSM_PSMCT24, PSM_PSMCT16):
        merged['TCC'] = 0   # no transparency in packed direct-colour formats
    # else: paletted — ALPHA presence is defined by the palette entries; keep Frontiers TCC

    if vanilla_td.is_paletted:
        # Paletted texture: describe the CLUT format from the Vanilla header.
        # Zero CBP/CSA and set CLD=1 so the Frontiers engine re-allocates the
        # CLUT in GS VRAM at load time rather than using a stale Vanilla address.
        # This is the direct cure for addr=0x4 (NULL+4) TLB misses.
        merged['CPSM'] = vanilla_td.cpsm
        merged['CSM']  = 0   # CSM1 (swizzled CLUT storage; standard for EQOA)
        merged['CBP']  = 0   # ← ZERO: engine must re-allocate at runtime
        merged['CSA']  = 0   # ← ZERO: start from first CLUT entry
        merged['CLD']  = 1   # 1 = LOAD: upload new CLUT on next draw call
    else:
        # Direct-colour texture: no CLUT needed — clear all CLUT fields.
        merged['CPSM'] = 0
        merged['CSM']  = 0
        merged['CBP']  = 0
        merged['CSA']  = 0
        merged['CLD']  = 0

    # Fields we KEEP from Frontiers (they govern the engine pipeline, not format):
    #   TBP0 (texture buffer base pointer — engine manages GS VRAM layout)
    #   TBW  (texture buffer width        — follows TBP0 allocation)
    #   TFX  (texture function: modulate/decal/highlight — visual mode)

    new_tex0_val = encode_tex0(merged)

    # ── Log diffs for operator visibility ────────────────────────────────────
    diffs = []
    for field in ('PSM', 'TW', 'TH', 'TCC', 'CPSM', 'CBP', 'CSA', 'CLD'):
        if base.get(field) != merged.get(field):
            old_v = base.get(field, 0)
            new_v = merged.get(field, 0)
            if field == 'PSM':
                old_s = PSM_NAMES.get(old_v, hex(old_v))
                new_s = PSM_NAMES.get(new_v, hex(new_v))
                diffs.append(f"PSM {old_s}->{new_s}")
            else:
                diffs.append(f"{field} {old_v}->{new_v}")
    if diffs:
        print(f"      [TEX0 diff] {label}: {', '.join(diffs)}")
    else:
        print(f"      [TEX0 diff] {label}: format already compatible, CBP zeroed.")

    # ── Patch TEX0 in GIF packet in-place ────────────────────────────────────
    patched_raw = patch_tex0_in_gif_packet(fro_raw, new_tex0_val)
    if patched_raw is None:
        print(f"      [warn] {label}: in-place TEX0 patch failed; "
              f"Frontiers material retained.")
        return result

    result['inline_data'] = patched_raw
    result['data_size']   = len(patched_raw)
    return result


# ─────────────────────────────────────────────────────────────────────────────
# SECTION C  —  0x11110 texture container rebuilder
# ─────────────────────────────────────────────────────────────────────────────

def _collect_tex_mat_pairs(container: dict) -> list:
    """
    Return a list of dicts, one per direct child of *container*.
    Each dict: {'child_idx': int, 'tex': node|None, 'mat': node|None}

    In EQOA the 0x11110 node has N direct children, one per texture slot.
    Each child sub-tree contains exactly one 0x01000 leaf and one 0x31100 leaf.
    We search depth-first within each child sub-tree to find them.
    """
    slots = []
    if container['child_count'] == 0:
        return slots   # leaf container — caller handles separately
    for idx, child in enumerate(container['children']):
        tex_list, mat_list = [], []
        find_nodes_by_type(child, 0x01000, tex_list)
        find_nodes_by_type(child, 0x31100, mat_list)
        slots.append({
            'child_idx': idx,
            'tex':  tex_list[0] if tex_list else None,
            'mat':  mat_list[0] if mat_list else None,
        })
    return slots


def translate_texture_container(vanilla_container: dict,
                                 frontiers_container: dict,
                                 asset_label: str) -> dict:
    """
    Build a new 0x11110 subtree:
      • Structural skeleton = Frontiers (ensures 43-bone pipeline compatibility).
      • Visual content      = Vanilla   (textures / palettes transplanted).
      • GS register TEX0   = Frontiers base with Vanilla format fields patched.

    Returns a deep copy of the Frontiers container with the graft applied.
    Raises no exceptions; non-fatal errors are logged and the Frontiers slot
    is silently retained so the rest of the asset stays renderable.
    """
    result = copy.deepcopy(frontiers_container)

    # Handle leaf-mode containers (monolithic blob).
    if vanilla_container['child_count'] == 0 or frontiers_container['child_count'] == 0:
        print(f"    [{asset_label}] Leaf-mode container detected; "
              f"deep parse not applicable. Returning Frontiers container verbatim.")
        print(f"    Run compare_textures.py on this asset to determine the correct")
        print(f"    blob layout before adding a custom leaf-mode translation path.")
        return result

    van_slots = _collect_tex_mat_pairs(vanilla_container)
    fro_slots = _collect_tex_mat_pairs(result)

    n = min(len(van_slots), len(fro_slots))
    if len(van_slots) != len(fro_slots):
        print(f"    [{asset_label}] Slot count: Vanilla={len(van_slots)}, "
              f"Frontiers={len(fro_slots)}. Translating first {n} slot(s).")

    for slot_idx in range(n):
        vslot = van_slots[slot_idx]
        fslot = fro_slots[slot_idx]
        label = f"{asset_label}/slot{slot_idx}"

        # ── Locate live nodes inside the result deep copy ───────────────────
        live_tex_list, live_mat_list = [], []
        find_nodes_by_type(result['children'][fslot['child_idx']], 0x01000, live_tex_list)
        find_nodes_by_type(result['children'][fslot['child_idx']], 0x31100, live_mat_list)

        live_tex = live_tex_list[0] if live_tex_list else None
        live_mat = live_mat_list[0] if live_mat_list else None

        van_tex_node = vslot['tex']
        van_mat_node = vslot['mat']

        # ── Parse Vanilla texture ────────────────────────────────────────────
        if van_tex_node is None:
            print(f"      [{label}] No Vanilla 0x01000 node; slot skipped.")
            continue

        try:
            vanilla_td = parse_texture_descriptor(van_tex_node['inline_data'])
            print(f"      [{label}] Vanilla: {vanilla_td}")
        except Exception as e:
            print(f"      [{label}] Failed to parse Vanilla 0x01000: {e}")
            print(f"      This slot retained from Frontiers to prevent corruption.")
            continue

        # ── Rebuild 0x01000 with recalculated offsets ────────────────────────
        if live_tex is not None:
            new_inline = rebuild_tex_inline_data(vanilla_td, live_tex['inline_data'])
            live_tex['inline_data'] = new_inline
            live_tex['data_size']   = len(new_inline)
        else:
            print(f"      [{label}] No Frontiers 0x01000 live node found; "
                  f"texture slot not patched.")

        # ── Patch TEX0 in 0x31100 material node ─────────────────────────────
        if live_mat is not None and van_mat_node is not None:
            new_mat = translate_material_node(
                van_mat_node, live_mat, vanilla_td, label
            )
            live_mat['inline_data'] = new_mat['inline_data']
            live_mat['data_size']   = new_mat['data_size']
        elif live_mat is None:
            print(f"      [{label}] No Frontiers 0x31100 live node; "
                  f"material not patched.")
        else:
            print(f"      [{label}] No Vanilla 0x31100 node; "
                  f"Frontiers material retained verbatim.")

    # Cascade corrected data_size values up through the rebuilt tree.
    update_node_sizes(result)
    return result


# ─────────────────────────────────────────────────────────────────────────────
# SECTION D  —  Main pipeline
# ─────────────────────────────────────────────────────────────────────────────

def perform_texture_swaps():
    payload_dir        = './workspace/payloads'
    frontiers_esf_path = './workspace/expansion/CHAR.ESF'

    print("=" * 65)
    print("  frankenstein_texture_swapper.py  —  v2: Deep Structural")
    print("  Translation (pointer recalculation + TEX0 material diffing)")
    print("=" * 65)

    # ── Load and parse Frontiers ESF ──────────────────────────────────────────
    if not os.path.exists(frontiers_esf_path):
        print(f"[-] Frontiers ESF not found: {frontiers_esf_path}")
        sys.exit(1)

    print(f"\n[*] Parsing Frontiers CHAR.ESF ...")
    with open(frontiers_esf_path, 'rb') as fh:
        frontiers_esf_bytes = fh.read()

    frontiers_parser = ESFParser(frontiers_esf_bytes).parse()
    frontiers_map    = {
        e.asset_id: e
        for e in frontiers_parser.pointer_table
        if e.asset_id is not None
    }
    print(f"    Frontiers asset map: {len(frontiers_map)} entries.")

    # ── Enumerate Vanilla payload .bin files ──────────────────────────────────
    bin_files = sorted(glob.glob(os.path.join(payload_dir, '*.bin')))
    if not bin_files:
        print(f"[-] No .bin payloads in {payload_dir}")
        sys.exit(1)

    swapped  = 0
    skipped  = 0
    failed   = 0

    for filepath in bin_files:
        filename = os.path.basename(filepath)

        # Decode asset hash from filename (asset_0x<HASH>.bin)
        try:
            hash_str   = filename.split('_')[1].split('.')[0]
            asset_hash = int(hash_str, 16)
        except Exception:
            continue
        asset_label = f"0x{asset_hash:08X}"

        # ── Load Vanilla payload ──────────────────────────────────────────────
        with open(filepath, 'rb') as fh:
            vanilla_bytes = fh.read()

        if len(vanilla_bytes) < 12:
            continue

        # ── Parse Vanilla node tree ───────────────────────────────────────────
        try:
            vanilla_node, _ = parse_node(vanilla_bytes, 0)
        except Exception as e:
            print(f"\n  [-] Parse error on Vanilla {filename}: {e}")
            failed += 1
            continue

        if vanilla_node is None or vanilla_node['child_count'] == 0:
            continue   # pure leaf — nothing to graft

        # ── Find Vanilla 0x11110 texture container ────────────────────────────
        van_tex_containers = [c for c in vanilla_node['children']
                               if c['type_id'] == 0x11110]
        if not van_tex_containers:
            continue   # no texture sub-tree in this payload

        vanilla_tex_container = van_tex_containers[0]

        # ── Check Frontiers has this asset ────────────────────────────────────
        if asset_hash not in frontiers_map:
            print(f"\n  [skip] {asset_label}: not in Frontiers ESF — retaining Vanilla.")
            skipped += 1
            continue

        # ── Extract and parse the Frontiers native node ───────────────────────
        entry     = frontiers_map[asset_hash]
        fro_bytes = frontiers_esf_bytes[entry.offset : entry.offset + entry.length]

        try:
            frontiers_node, _ = parse_node(fro_bytes, 0)
        except Exception as e:
            print(f"\n  [-] Parse error on Frontiers native {asset_label}: {e}")
            failed += 1
            continue

        fro_tex_containers = [c for c in frontiers_node.get('children', [])
                               if c['type_id'] == 0x11110]
        if not fro_tex_containers:
            print(f"\n  [warn] {asset_label}: Frontiers asset has no 0x11110 node. "
                  f"Skipping to avoid structural mismatch.")
            skipped += 1
            continue

        frontiers_tex_container = fro_tex_containers[0]

        # ── Deep structural translation ───────────────────────────────────────
        print(f"\n[*] {asset_label}: translating texture container ...")
        try:
            translated_container = translate_texture_container(
                vanilla_container   = vanilla_tex_container,
                frontiers_container = frontiers_tex_container,
                asset_label         = asset_label,
            )
        except Exception as e:
            print(f"  [-] Translation error for {asset_label}: {e}")
            traceback.print_exc()
            failed += 1
            continue

        # ── Graft translated container into a Frontiers node tree deep-copy ───
        # We always build the final asset on a Frontiers skeleton — never Vanilla.
        graft_root = copy.deepcopy(frontiers_node)
        fro_tex_idx = next(
            (i for i, c in enumerate(graft_root['children'])
             if c['type_id'] == 0x11110),
            None
        )
        if fro_tex_idx is None:
            print(f"  [-] {asset_label}: 0x11110 slot vanished from deep copy (logic error).")
            failed += 1
            continue

        graft_root['children'][fro_tex_idx] = translated_container

        # ── Recompute sizes and serialize ─────────────────────────────────────
        update_node_sizes(graft_root)
        final_bytes = serialize_node(graft_root)

        # ── Write back to payload slot ────────────────────────────────────────
        with open(filepath, 'wb') as fh:
            fh.write(final_bytes)

        print(f"  [+] {asset_label}: written "
              f"({len(vanilla_bytes):,} B vanilla -> "
              f"{len(fro_bytes):,} B fro template -> "
              f"{len(final_bytes):,} B grafted)")
        swapped += 1

    # ── Summary ───────────────────────────────────────────────────────────────
    print(f"\n{'='*65}")
    print(f"  DEEP TRANSLATION COMPLETE")
    print(f"  Translated : {swapped}")
    print(f"  Skipped    : {skipped}  (Vanilla-only / no Frontiers counterpart)")
    print(f"  Failed     : {failed}")
    print(f"{'='*65}")

    if failed > 0:
        print("\n[-] One or more assets failed. DO NOT run repack_iso.py.")
        print("    For each failed hash, run:")
        print("      python compare_textures.py   -> verify 0x01000 DWORD layout")
        print("      python inspect_materials.py  -> verify 0x31100 GIF packet format")
        print("    Then adjust TEX_HDR_* offset constants at the top of this file")
        print("    to match the actual field positions in your binary.")
        sys.exit(1)

    print("\n[+] All payloads ready. Proceed with ESF merge -> repack_iso.py.")


if __name__ == '__main__':
    perform_texture_swaps()