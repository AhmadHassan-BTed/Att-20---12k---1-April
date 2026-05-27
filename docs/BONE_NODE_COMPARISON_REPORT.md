# Bone Node Comparison Report (Frontiers vs Vanilla)

## Summary
- Frontiers extracted type_id: `0x22400`
- Vanilla extracted type_id:   `0x12400`
- Frontiers data_size: 2,100 bytes
- Vanilla data_size:   1,500 bytes
- Frontiers child_count: 0
- Vanilla child_count:   0
- Frontiers serialized total_size: 2,112 bytes
- Vanilla serialized total_size:   1,512 bytes

## Binary Comparison (serialized bone-node subgraph)
- Frontiers serialized length: 2,112 bytes
- Vanilla serialized length:   1,512 bytes
- Bytes compared: 1,512
- Differing bytes: 880
- Percent difference: 58.20%

### First byte difference
- Byte offset (within compared serialized span): `0x2`
- Frontiers byte: `0x02`
- Vanilla byte:   `0x01`

## Leaf Payload Comparison (DFS order, best-effort alignment)
- Frontiers leaf count: 1
- Vanilla   leaf count: 1
- Compared leaf payloads: 1
- Differing leaf payloads (best-effort): 1

|#|Front Type|Van Type|Front Size|Van Size|Diffs|Diff%|FirstDiff@|Front Path|Van Path|
|-:|---:|---:|---:|---:|---:|---:|---:|---|---|
|0|0x22400|0x12400|2,100|1,500|877|58.5%|0x10|root|root|
