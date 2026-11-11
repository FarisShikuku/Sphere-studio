#!/usr/bin/env python3
import os
import sys
import glob
import shutil
import subprocess
from concurrent.futures import ProcessPoolExecutor

sys.path.insert(0, "native_assets/scripts")
import convert_to_ktx2
from batch_process_textures import TEXTURE_SPECS

def verify_ktx2(filepath):
    import struct
    if not os.path.exists(filepath):
        return False, "File does not exist"
    size = os.path.getsize(filepath)
    if size < 104:
        return False, f"File too small ({size} bytes)"
    with open(filepath, "rb") as f:
        data = f.read(128)
    magic = data[:12]
    if magic != b'\xABKTX 20\xBB\r\n\x1A\n':
        return False, "Invalid KTX2 magic header"
    vk_format, type_size, w, h = struct.unpack("<4I", data[12:28])
    level_count = struct.unpack("<I", data[40:44])[0]
    if w <= 0 or h <= 0 or level_count <= 0:
        return False, f"Invalid dimensions or levels: {w}x{h}, {level_count} levels"
    level0_off, level0_len, _ = struct.unpack("<3Q", data[80:104])
    if level0_off <= 0 or level0_len != (w * h * 4):
        return False, f"Invalid level 0: offset={level0_off}, len={level0_len}, expected={w * h * 4}"
    if size < level0_off + level0_len:
        return False, f"Truncated file: size {size} < offset {level0_off + level0_len}"
    return True, f"Valid: {w}x{h}, {level_count} mips, {size} bytes"

def process_spec(spec):
    tex_id, pattern = spec[0], spec[1]
    # Find matching image in textures/
    image_files = [f for f in os.listdir("textures") if f.lower().endswith(('.jpg', '.jpeg', '.png'))]
    matches = [f for f in image_files if pattern in f.replace('_', '-').lower() or tex_id in f]
    if not matches:
        return (tex_id, False, f"No matching image found for pattern '{pattern}'")
    
    src_file = matches[0]
    src_path = os.path.join("textures", src_file)
    src_base = os.path.splitext(src_file)[0]
    
    out_ktx_id = os.path.join("textures", f"{tex_id}.ktx2")
    out_ktx_named = os.path.join("textures", f"{src_base}.ktx2")
    out_native = os.path.join("native_assets/textures", f"{tex_id}.ktx2")
    
    try:
        # Convert source image to out_ktx_id
        convert_to_ktx2.convert_image_to_ktx2(src_path, out_ktx_id, max_dim=512, is_srgb=True)
        # Duplicate to named ktx2 if different
        if out_ktx_id != out_ktx_named:
            shutil.copy2(out_ktx_id, out_ktx_named)
        # Copy to native_assets
        os.makedirs("native_assets/textures", exist_ok=True)
        shutil.copy2(out_ktx_id, out_native)
        
        ok, msg = verify_ktx2(out_ktx_id)
        if not ok:
            return (tex_id, False, f"Verification failed: {msg}")
        return (tex_id, True, f"{src_file} -> {tex_id}.ktx2 ({msg})")
    except Exception as e:
        return (tex_id, False, str(e))

def process_extra_image(img_file):
    src_path = os.path.join("textures", img_file)
    src_base = os.path.splitext(img_file)[0]
    out_ktx = os.path.join("textures", f"{src_base}.ktx2")
    try:
        convert_to_ktx2.convert_image_to_ktx2(src_path, out_ktx, max_dim=512, is_srgb=True)
        ok, msg = verify_ktx2(out_ktx)
        if not ok:
            return (img_file, False, f"Verification failed: {msg}")
        return (img_file, True, f"{img_file} -> {src_base}.ktx2 ({msg})")
    except Exception as e:
        return (img_file, False, str(e))

def main():
    print(f"=== Starting KTX2 High-Performance Texture Conversion ===")
    os.makedirs("textures", exist_ok=True)
    os.makedirs("native_assets/textures", exist_ok=True)
    
    # 1. Process all 65 specs
    with ProcessPoolExecutor(max_workers=4) as executor:
        spec_results = list(executor.map(process_spec, TEXTURE_SPECS))
    
    failed_specs = [r for r in spec_results if not r[1]]
    if failed_specs:
        print(f"ERROR: {len(failed_specs)} specs failed conversion!")
        for f in failed_specs:
            print(" ", f)
        sys.exit(1)
    print(f"Successfully converted and verified all {len(spec_results)} PBR texture specs!")
    
    # 2. Process any remaining images not covered by specs
    image_files = [f for f in os.listdir("textures") if f.lower().endswith(('.jpg', '.jpeg', '.png'))]
    already_covered = set()
    for spec in TEXTURE_SPECS:
        tex_id, pattern = spec[0], spec[1]
        matches = [f for f in image_files if pattern in f.replace('_', '-').lower() or tex_id in f]
        already_covered.update(matches)
    
    extra_images = [f for f in image_files if f not in already_covered]
    print(f"Processing {len(extra_images)} additional textures: {extra_images}")
    for ext_img in extra_images:
        res = process_extra_image(ext_img)
        print("  ", res)
        if not res[1]:
            print(f"ERROR on extra image {ext_img}: {res[2]}")
            sys.exit(1)
            
    # 3. Final verification of all KTX2 files in textures/
    all_ktx2 = [f for f in os.listdir("textures") if f.endswith(".ktx2")]
    print(f"\nVerifying all {len(all_ktx2)} KTX2 files in textures/...")
    for kf in all_ktx2:
        kp = os.path.join("textures", kf)
        ok, msg = verify_ktx2(kp)
        if not ok:
            print(f"CRITICAL: {kf} failed validation: {msg}")
            sys.exit(1)
    print(f"All {len(all_ktx2)} KTX2 files validated successfully!")
    
    # 4. Permanently delete all original JPEG/PNG files in textures/
    print("\nPermanently deleting all JPEG/PNG files from textures/ and native_assets/textures/...")
    deleted_count = 0
    for root in ["textures", "native_assets/textures", "references/textures"]:
        if not os.path.exists(root):
            continue
        for f in os.listdir(root):
            if f.lower().endswith(('.jpg', '.jpeg', '.png', '.webp')):
                full_path = os.path.join(root, f)
                os.remove(full_path)
                deleted_count += 1
                print(f"  Deleted: {full_path}")
                
    print(f"\nPermanently deleted {deleted_count} JPEG/PNG files.")
    print("KTX2 direct GPU assets are fully established!")

if __name__ == "__main__":
    main()
