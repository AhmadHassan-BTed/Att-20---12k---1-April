import os
import sys
import struct
sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
from core.esf_parser import ESFParser

def count_types(node, tallies):
    tallies[node['type_id']] = tallies.get(node['type_id'], 0) + 1
    for c in node.get('children', []):
        count_types(c, tallies)

def debug():
    with open("workspace/original/CHAR.ESF", 'rb') as f:
        data = f.read()
    esf = ESFParser(data).parse()
    
    tallies = {}
    count_types(esf.root, tallies)
    
    for k, v in sorted(tallies.items()):
        print(f"Node Type 0x{k:05X}: {v} occurrences")

if __name__ == "__main__":
    debug()
