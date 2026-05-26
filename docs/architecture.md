# EQOA Character Model Pipeline — Architecture Reference

This document provides a low-level architectural specification of the PlayStation 2 *EverQuest Online Adventures (EQOA)* game assets, filesystem, and model splicing pipeline.

---

## 1. Proprietary ESF File Format (`FJBO`)

The `.esf` file is a recursive, typed, byte-aligned tree container.

### 1.1 Master File Header (32 bytes)
| Offset | Size (Bytes) | Field Name | Constant Value / Description |
|--------|--------------|------------|-----------------------------|
| `0x00` | 4 | `magic` | `b"FJBO"` (File Magic) |
| `0x04` | 4 | `version` | `1` (Model / Database version) |
| `0x08` | 4 | `constant` | `0xAB4F` (Platform marker) |
| `0x0C` | 4 | `reserved1` | `0x00000000` |
| `0x10` | 4 | `hdr_size` | `0x00000020` (32 bytes) |
| `0x14` | 4 | `reserved2` | `0x00000000` |
| `0x18` | 8 | `padding` | `0xFFFFFFFFFFFFFFFF` |

### 1.2 Recursive Node Header (12 bytes)
Every node in the FJBO tree starts with a 12-byte header:
- **`type_id` (4 bytes, uint32-LE)**: Defines the node data type.
- **`data_size` (4 bytes, uint32-LE)**: Byte length of content.
  - For leaf nodes (`child_count = 0`): Size of inline data.
  - For branch nodes (`child_count > 0`): Sum of all child nodes (each including its 12-byte header).
- **`child_count` (4 bytes, uint32-LE)**: Number of recursive child nodes.

---

## 2. Character Model Node Structural Layouts

### 2.1 Vanilla Model Layout (`0x62700` — 15 Children)
Natively used in original EQOA for characters. Contains 15 children:
- **Child 0 (`0x42710`)**: Metadata / Bounding box limits.
- **Child 1 (`0x11110`)**: Texture Container (housing `0x01001` textures and `0x01101` material render states).
- **Child 2 (`0x05000`)**: Joint indices / rigging offsets.
- **Child 3 (`0x0B070`)**: Joint coordinate scale values.
- **Child 4 (`0x02800`)**: Bounding boxes / Skeleton joints rig.
- **Child 5 (`0x02610`)**: Mesh Container (housing `0x02600` DMA vertex strips, vertices, UVs, weights).
- **Child 6 (`0x12400`)**: Skeleton bone definitions.
- **Child 7-14**: Supporting animation and scale controllers (`0x05000`, `0x02450`, `0x02900`, `0x32910`, `0x12915`, `0x02920`, `0x02930`, `0x02940`).

### 2.2 Frontiers Model Layout (`0x72700` — 17 Children)
Natively required by the *Frontiers* expansion engine for characters. Includes all 15 components of the Vanilla model, with three specific changes:
- **`0x62700` root upgraded to `0x72700`**.
- **Child 6 upgraded from `0x12400` to `0x22400`** (expanded bone definition properties).
- **Two additional trailer children appended**:
  - **Child 15 (`0x02950`)**: Size 0 empty node.
  - **Child 16 (`0x02960`)**: Size 4 metadata leaf node (`0x00000000`).

---

## 3. Dual-Filesystem ISO Layout

PS2 games boot and load files using a dual-filesystem scheme consisting of ISO 9660 and UDF (Universal Disk Format).

```
  Patched ISO Structure
  ┌──────────────────────────────────────────────┐
  │  System Area & PVD (Sectors 0 - 15)           │
  ├──────────────────────────────────────────────┤
  │  ISO9660 Directory Records (Sector 16+)      │
  ├──────────────────────────────────────────────┤
  │  UDF File Entry Descriptors                  │
  │  - Sector 337: UDF File Entry for CHAR.ESF   │
  ├──────────────────────────────────────────────┤
  │  Original Files                              │
  ├──────────────────────────────────────────────┤
  │  Original CHAR.ESF LBA 3578 (Ignored)        │
  ├──────────────────────────────────────────────┤
  │  Appended custom CHAR.ESF (LBA 1492368)       │
  └──────────────────────────────────────────────┘
```

### 3.1 Logical vs Physical LBA Mapping
- **Partition Offset**: The UDF partition begins at sector `278`.
- **Logical Sector Address**: Used inside the UDF descriptors (Relative to sector 278).
- **Physical Sector Address**: Used in ISO 9660 Directory records and physical seeks (Absolute sector on disc).
- **Logical LBA = Physical LBA - 278**
  - Physical Target: `1,492,368`
  - Logical Target: `1,492,090`
