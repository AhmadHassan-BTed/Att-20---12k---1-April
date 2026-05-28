import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
from core.esf_parser import ESFParser

def test_csf():
    filepath = "workspace/CHARSEL1.CSF"
    if not os.path.exists(filepath):
        print(f"File not found: {filepath}")
        return
        
    with open(filepath, 'rb') as f:
        data = f.read()
        
    try:
        parser = ESFParser(data)
        result = parser.parse()
        print(f"Parsed {filepath} successfully!")
        print(f"Pointer table size: {len(result.pointer_table)}")
        print(f"Root node type: 0x{result.root['type_id']:05X}")
    except Exception as e:
        print(f"Error parsing CSF: {e}")

if __name__ == "__main__":
    test_csf()
