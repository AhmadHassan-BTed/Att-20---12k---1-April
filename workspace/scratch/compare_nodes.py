import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
from core.esf_parser import ESFParser

def get_nodes(node, type_id, out_list):
    if node['type_id'] == type_id:
        out_list.append(node)
    for c in node.get('children', []):
        get_nodes(c, type_id, out_list)

def debug():
    with open("workspace/original/CHAR.ESF", 'rb') as f:
        data = f.read()
    esf = ESFParser(data).parse()
    
    for entry in esf.pointer_table:
        if entry.asset_id == 0x05AEBA67:
            def search_tree(node, target_offset):
                if node['offset'] == target_offset: return node
                for c in node.get('children', []):
                    res = search_tree(c, target_offset)
                    if res: return res
                return None
            
            model = search_tree(esf.root, entry.offset)
            
            n_2610 = []
            get_nodes(model, 0x02610, n_2610)
            
            n_1210 = []
            get_nodes(model, 0x21210, n_1210)
            get_nodes(model, 0x1210, n_1210)
            get_nodes(model, 0x11210, n_1210)
            
            n_2320 = []
            get_nodes(model, 0x02320, n_2320)
            
            print(f"Ogre Model:")
            for n in n_2610:
                print(f"  0x02610 size: {n['data_size']}")
            for n in n_1210:
                print(f"  SkinPrimBuffer size: {n['data_size']}")
            for n in n_2320:
                print(f"  SkinSubSprite size: {n['data_size']}")
            break

if __name__ == "__main__":
    debug()
