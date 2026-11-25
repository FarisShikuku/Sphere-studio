#!/usr/bin/env python3
"""
KTX2 Texture Generator for High-Performance Vulkan Studio
Converts input images (JPEG/PNG) to Khronos Texture 2.0 (KTX2) format with:
- Standard 12-byte KTX2 identifier
- Vulkan VK_FORMAT_R8G8B8A8_SRGB (43) or VK_FORMAT_R8G8B8A8_UNORM (37)
- Multi-level mipmap pyramid down to 1x1
- Complete Khronos Data Format Descriptor (DFD) and Key/Value metadata
"""
import os
import sys
import struct
import subprocess

KTX2_IDENTIFIER = b'\xABKTX 20\xBB\r\n\x1A\n'

VK_FORMAT_R8G8B8A8_UNORM = 37
VK_FORMAT_R8G8B8A8_SRGB = 43

def build_dfd_block(is_srgb=True):
    # Total DFD block: uint32 totalSize + basic descriptor (24 bytes) + 4 samples (16 bytes each) = 4 + 24 + 64 = 92 bytes
    total_size = 4 + 24 + 64
    vendor_id = 0       # KHR_DF_VENDORID_KHRONOS
    desc_type = 0       # KHR_DF_KHR_DESCSPECIFIC
    version_num = 2     # KHR_DF_VERSIONNUMBER_1_3
    block_size = 24 + 64 # 88 bytes
    model = 1           # KHR_DF_MODEL_RGBSDA
    primaries = 1       # KHR_DF_PRIMARIES_BT709
    transfer = 2 if is_srgb else 1  # 2 = sRGB, 1 = Linear
    flags = 0           # KHR_DF_FLAG_ALPHA_STRAIGHT
    
    basic_header = struct.pack(
        "<IHHHHBBBBBBBBBBBBBBBB",
        total_size,
        vendor_id,
        desc_type,
        version_num,
        block_size,
        model,
        primaries,
        transfer,
        flags,
        0, 0, 0, 0,      # texelBlockDimensions 0..3 (1x1x1x1 encoded as 0)
        4, 0, 0, 0, 0, 0, 0, 0 # bytesPlanes 0..7
    )
    
    # 4 channels: R (0), G (1), B (2), A (15)
    channels = [(0, 0), (8, 1), (16, 2), (24, 15)]
    sample_data = bytearray()
    for bit_offset, channel_type in channels:
        # bitOffset: 16b, bitLength: 8b (7 = 8-1), channelType: 8b
        # samplePosition[4]: 4b, sampleLower: 32b, sampleUpper: 32b
        sample = struct.pack(
            "<HBBBBBBII",
            bit_offset,
            7, # bitLength - 1
            channel_type,
            0, 0, 0, 0, # samplePositions
            0x00000000, # sampleLower (0)
            0x000000FF  # sampleUpper (255)
        )
        sample_data.extend(sample)
        
    return basic_header + sample_data

def build_kvd_block(metadata=None):
    # Key-value data: KTXwriter + optional sorted custom entries
    entries = [("KTXwriter", "SphereStudioEngine 1.0 (SaschaWillems Vulkan Framework)")]
    if metadata:
        for key in sorted(metadata.keys()):
            val = metadata[key]
            if key and val is not None:
                entries.append((str(key), str(val)))

    kvd = bytearray()
    for key, val in entries:
        raw_entry = key.encode("utf-8") + b"\x00" + val.encode("utf-8") + b"\x00"
        length = len(raw_entry)
        pad = (4 - (length % 4)) % 4
        kvd.extend(struct.pack("<I", length) + raw_entry + (b"\x00" * pad))
    return bytes(kvd)

def convert_image_to_ktx2(input_path, output_path, max_dim=1024, is_srgb=True, metadata=None):
    print(f"Converting {input_path} -> {output_path}")
    
    # Identify image dimensions using convert
    cmd_dim = ["convert", input_path, "-format", "%w %h", "info:"]
    res = subprocess.check_output(cmd_dim).decode("utf-8").strip().split()
    orig_w, orig_h = int(res[0]), int(res[1])
    
    # Scale maintaining aspect ratio, snap to multiple of 4 for GPU block alignment
    scale = min(1.0, float(max_dim) / max(orig_w, orig_h))
    target_w = max(4, int(round(orig_w * scale / 4.0)) * 4)
    target_h = max(4, int(round(orig_h * scale / 4.0)) * 4)
    
    # Calculate mip levels
    levels = []
    w, h = target_w, target_h
    while True:
        # Generate raw RGBA using convert
        cmd_raw = ["convert", input_path, "-resize", f"{w}x{h}!", "-depth", "8", f"rgba:-"]
        raw_rgba = subprocess.check_output(cmd_raw)
        levels.append((w, h, raw_rgba))
        if w == 1 and h == 1:
            break
        w = max(1, w // 2)
        h = max(1, h // 2)
        
    level_count = len(levels)
    dfd_data = build_dfd_block(is_srgb=is_srgb)
    kvd_data = build_kvd_block(metadata=metadata)
    
    header_size = 80
    level_index_size = level_count * 24
    
    dfd_offset = header_size + level_index_size
    dfd_length = len(dfd_data)
    
    kvd_offset = dfd_offset + dfd_length
    kvd_length = len(kvd_data)
    
    # Data begins aligned to 16 bytes
    data_start = kvd_offset + kvd_length
    data_start = (data_start + 15) & ~15
    kvd_padding = data_start - (kvd_offset + kvd_length)
    
    # Compute level offsets and sizes (levels stored from mip 0 to mip N-1 or reverse)
    # KTX2 spec: Level index contains entries for mip 0 to levelCount-1.
    level_indices = []
    curr_offset = data_start
    level_blobs = []
    
    for w, h, raw_data in levels:
        # Align level offset to 16 bytes
        curr_offset = (curr_offset + 15) & ~15
        byte_length = len(raw_data)
        level_indices.append((curr_offset, byte_length, byte_length))
        level_blobs.append(raw_data)
        curr_offset += byte_length
        
    vk_format = VK_FORMAT_R8G8B8A8_SRGB if is_srgb else VK_FORMAT_R8G8B8A8_UNORM
    
    # Pack header (80 bytes)
    header = struct.pack(
        "<12sIIIIIIIIIIIIIQQ",
        KTX2_IDENTIFIER,
        vk_format,
        1,              # typeSize = 1
        target_w,       # pixelWidth
        target_h,       # pixelHeight
        0,              # pixelDepth = 0 (2D)
        0,              # layerCount = 0 (non-array)
        1,              # faceCount = 1
        level_count,    # levelCount
        0,              # supercompressionScheme = 0 (None)
        dfd_offset,     # dfdByteOffset
        dfd_length,     # dfdByteLength
        kvd_offset,     # kvdByteOffset
        kvd_length,     # kvdByteLength
        0,              # sgdByteOffset
        0               # sgdByteLength
    )
    
    level_index_bytes = bytearray()
    for byte_offset, byte_length, uncompressed_length in level_indices:
        level_index_bytes.extend(struct.pack("<QQQ", byte_offset, byte_length, uncompressed_length))
        
    with open(output_path, "wb") as f:
        f.write(header)
        f.write(level_index_bytes)
        f.write(dfd_data)
        f.write(kvd_data)
        f.write(b"\x00" * kvd_padding)
        for i, (byte_offset, byte_length, _) in enumerate(level_indices):
            # Pad to match byte_offset
            curr_pos = f.tell()
            if byte_offset > curr_pos:
                f.write(b"\x00" * (byte_offset - curr_pos))
            f.write(level_blobs[i])
            
    print(f"Successfully generated {output_path} ({os.path.getsize(output_path)} bytes, {target_w}x{target_h}, {level_count} mips)")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: convert_to_ktx2.py <input_img> <output_ktx2> [--unorm] [max_dim]")
        sys.exit(1)
    inp = sys.argv[1]
    out = sys.argv[2]
    is_srgb = "--unorm" not in sys.argv
    max_d = 1024
    for arg in sys.argv[3:]:
        if arg.isdigit():
            max_d = int(arg)
    convert_image_to_ktx2(inp, out, max_dim=max_d, is_srgb=is_srgb)
