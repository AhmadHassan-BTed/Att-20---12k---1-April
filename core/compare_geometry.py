import os
import sys
import struct
import hashlib
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from core.esf_parser import ESFParser

def find_model_node(parser, target_hash):
    for child in parser.root['children']:
        if child['type_id'] == 0x0A010:
            for model_node in child['children']:
                h = parser._find_hash_in_subtree(model_node)
                if h == target_hash:
                    return model_node
    return None

def find_node(node, type_id):
    if node['type_id'] == type_id:
        return node
    for child in node['children']:
        res = find_node(child, type_id)
        if res: return res
    return None

def main():
    target_hash = 0x05AEBA67
    original_esf = "workspace/original/CHAR.ESF" # Vanilla
    expansion_esf = "workspace/expansion/CHAR.ESF" # Frontiers
    
    print("[*] Loading Vanilla ESF...")
    with open(original_esf, 'rb') as f:
        van_parser = ESFParser(f.read()).parse()
        
    print("[*] Loading Frontiers ESF...")
    with open(expansion_esf, 'rb') as f:
        fro_parser = ESFParser(f.read()).parse()
        
    van_model = find_model_node(van_parser, target_hash)
    fro_model = find_model_node(fro_parser, target_hash)
    
    van_geom = find_node(van_model, 0x02610)
    fro_geom = find_node(fro_model, 0x02610)
    
    if not van_geom or not fro_geom:
        print("[-] Could not find 0x02610 nodes.")
        return
        
    # Serialize the nodes to compare their raw binary
    def serialize_node(node):
        buf = bytearray()
        buf += struct.pack('<III', node['type_id'], node['data_size'], node['child_count'])
        if node['child_count'] == 0:
            if node['inline_data']:
                buf += node['inline_data']
        else:
            for child in node['children']:
                buf += serialize_node(child)
        return bytes(buf)
        
    van_bin = serialize_node(van_geom)
    fro_bin = serialize_node(fro_geom)
    
    print(f"\nVanilla 0x02610 Size: {len(van_bin)}")
    print(f"Frontiers 0x02610 Size: {len(fro_bin)}")
    
    van_hash = hashlib.sha256(van_bin).hexdigest()
    fro_hash = hashlib.sha256(fro_bin).hexdigest()
    
    print(f"Vanilla Hash:   {van_hash}")
    print(f"Frontiers Hash: {fro_hash}")
    
    if van_hash == fro_hash:
        print("\n[+] The Geometry Nodes are EXACTLY IDENTICAL! No injection is needed!")
    else:
        print("\n[-] The Geometry Nodes differ.")

if __name__ == "__main__":
    main()
