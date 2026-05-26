#!/usr/bin/env python3
import os
import shutil

def organize():
    root_dir = os.path.dirname(os.path.abspath(__file__))
    
    diagnostics_dir = os.path.join(root_dir, 'diagnostics')
    legacy_dir = os.path.join(root_dir, 'legacy')
    
    # Create directories
    os.makedirs(diagnostics_dir, exist_ok=True)
    os.makedirs(legacy_dir, exist_ok=True)
    
    # Files to move to diagnostics
    diagnostics_files = [
        'check_4bd83120_in_vanilla.py',
        'check_all_payloads.py',
        'check_all_texture_mappings.py',
        'check_baseline.py',
        'check_container_types.py',
        'check_esf_headers.py',
        'check_frontiers_reference_strips.py',
        'check_iso_layout.py',
        'check_native_62700_strips.py',
        'check_native_vif.py',
        'check_payload_types.py',
        'check_textures.py',
        'check_vanilla_target_strips.py',
        'compare_22000_hashes.py',
        'compare_4bd83120.py',
        'compare_62700_headers.py',
        'compare_pointer_entries.py',
        'compare_pointer_tables.py',
        'compare_raw_geom.py',
        'compare_textures.py',
        'count_vanilla_models.py',
        'deep_diagnostic.py',
        'deep_diff_models.py',
        'deep_scan_udf_fe.py',
        'definitive_comparison.py',
        'diagnose_udf_iso.py',
        'dump_trees.py',
        'inspect_05000.py',
        'inspect_42710.py',
        'inspect_all_charsel_files.py',
        'inspect_both_occurrences.py',
        'inspect_char_esf.py',
        'inspect_charcust.py',
        'inspect_charsel_csf.py',
        'inspect_geometry.py',
        'inspect_joints.py',
        'inspect_joints_detailed.py',
        'inspect_material_texture_links.py',
        'inspect_materials.py',
        'inspect_native_62700.py',
        'inspect_original_charsel.py',
        'inspect_skeleton_records.py',
        'inspect_strip_vif.py',
        'inspect_strips.py',
        'inspect_vif_end.py',
        'inspect_vif_packets.py',
        'list_all_files_sorted_lba.py',
        'list_iso_files.py',
        'search_all_char_esf_directory_entries.py',
        'search_all_files_for_lba.py',
        'search_classic_in_frontiers.py',
        'search_elf_lba.py',
        'search_elf_offsets.py',
        'scan_charcust_flat.py',
        'scan_dependencies.py',
        'scan_vif_offsets.py',
        'offset_calculator.py',
        'hex_analyzer.py',
        'test_resize.py',
        'test_texture_swap.py',
        'test_vanilla_frontiers.py'
    ]
    
    # Files to move to legacy
    legacy_files = [
        'clean_surgery_pipeline.py',
        'deep_surgery_fix.py',
        'frankenstein_grafter.py',
        'frankenstein_texture_swapper.py',
        'null_hypothesis.py',
        'precision_texture_graft_pipeline.py',
        'pristine_transplant_pipeline.py',
        'apply_udf_patch_direct.py',
        'patch_udf_char_esf.py',
        'solve_udf_partition_map.py'
    ]
    
    moved_count = 0
    for filename in diagnostics_files:
        src = os.path.join(root_dir, filename)
        if os.path.exists(src):
            dst = os.path.join(diagnostics_dir, filename)
            shutil.move(src, dst)
            print(f"[+] Moved {filename} -> diagnostics/")
            moved_count += 1
            
    for filename in legacy_files:
        src = os.path.join(root_dir, filename)
        if os.path.exists(src):
            dst = os.path.join(legacy_dir, filename)
            shutil.move(src, dst)
            print(f"[+] Moved {filename} -> legacy/")
            moved_count += 1
            
    print(f"\n[SUCCESS] Successfully organized {moved_count} files into diagnostics/ and legacy/ directories.")

if __name__ == '__main__':
    organize()
