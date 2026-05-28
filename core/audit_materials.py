import os
import sys
import struct
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
    vanilla_esf = "workspace/original/CHAR.ESF"
    frontiers_esf = "workspace/expansion/CHAR.ESF"
    
    with open(vanilla_esf, 'rb') as f:
        van_data = f.read()
    van_parser = ESFParser(van_data).parse()
    
    with open(frontiers_esf, 'rb') as f:
        fro_data = f.read()
    fro_parser = ESFParser(fro_data).parse()
    
    van_model = find_model_node(van_parser, target_hash)
    fro_model = find_model_node(fro_parser, target_hash)
    
    van_mat = next((c for c in van_model['children'] if c['type_id'] in (0x11110, 0x11100)), None)
    fro_mat = next((c for c in fro_model['children'] if c['type_id'] == 0x11110), None)
    
    print("=" * 60)
    print("  MATERIAL / TEXTURE CONTAINER AUDIT")
    print("=" * 60)
    
    def analyze_container(mat_node):
        if not mat_node: return "Not Found", 0, 0
        node_01001 = find_node(mat_node, 0x01001)
        node_01101 = find_node(mat_node, 0x01101)
        num_tex = len(node_01001['children']) if node_01001 else 0
        num_mat = len(node_01101['children']) if node_01101 else 0
        return f"0x{mat_node['type_id']:05X}", num_tex, num_mat
        
    v_id, v_tex, v_mat = analyze_container(van_mat)
    f_id, f_tex, f_mat = analyze_container(fro_mat)
    
    print(f"Vanilla   ({v_id}): {v_tex} Textures, {v_mat} Materials")
    print(f"Frontiers ({f_id}): {f_tex} Textures, {f_mat} Materials")
    
if __name__ == "__main__":
    main()
