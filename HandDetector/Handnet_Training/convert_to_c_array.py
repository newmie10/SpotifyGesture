#!/usr/bin/env python3
"""
Convert TFLite model to C array for ESP32 deployment.

Usage:
    python convert_to_c_array.py <input_tflite_file> [output_cc_file]

Example:
    python convert_to_c_array.py handnet_int8.tflite handnet_model_data.cc
"""

import sys
import os

def convert(file_path, output_path=None):
    """Convert TFLite model to C array format."""
    
    if not os.path.exists(file_path):
        print(f"Error: File '{file_path}' not found!")
        return False
    
    # Read model data
    with open(file_path, "rb") as f:
        data = f.read()
    
    # Generate output filename if not provided
    if output_path is None:
        base_name = os.path.splitext(os.path.basename(file_path))[0]
        output_path = f"{base_name}_model_data.cc"
    
    # Generate C array code
    output_lines = []
    output_lines.append("// Auto-generated model data from TFLite")
    output_lines.append(f"// Source: {file_path}")
    output_lines.append(f"// Size: {len(data)} bytes\n")
    output_lines.append("#include \"handnet_model_data.h\"\n")
    output_lines.append("// Model data array")
    output_lines.append("alignas(8) const unsigned char g_model[] = {")
    
    # Write bytes in rows of 12
    for i, b in enumerate(data):
        if i % 12 == 0:
            output_lines.append("  ")
        output_lines[-1] += f"0x{b:02x},"
        if i % 12 == 11 or i == len(data) - 1:
            output_lines[-1] = output_lines[-1].rstrip(",") if i == len(data) - 1 else output_lines[-1]
            if i != len(data) - 1:
                output_lines.append("")
        else:
            output_lines[-1] += " "
    
    output_lines.append("\n};")
    output_lines.append(f"\nconst int g_model_len = {len(data)};")
    
    # Write to file
    with open(output_path, "w", encoding="utf-8") as f:
        f.write("\n".join(output_lines))
    
    print(f"✓ Converted {file_path}")
    print(f"✓ Model size: {len(data):,} bytes")
    print(f"✓ Output written to: {output_path}")
    
    # Also create/update header file
    header_path = output_path.replace(".cc", ".h")
    with open(header_path, "w", encoding="utf-8") as f:
        f.write("// Auto-generated model header\n")
        f.write(f"// Source: {file_path}\n\n")
        f.write("#ifndef HANDNET_MODEL_DATA_H_\n")
        f.write("#define HANDNET_MODEL_DATA_H_\n\n")
        f.write("extern const unsigned char g_model[];\n")
        f.write("extern const int g_model_len;\n\n")
        f.write("#endif  // HANDNET_MODEL_DATA_H_\n")
    
    print(f"✓ Header written to: {header_path}")
    return True

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(__doc__)
        print("\nError: Please provide input TFLite file")
        print("\nAvailable TFLite files in current directory:")
        for f in os.listdir("."):
            if f.endswith(".tflite"):
                size = os.path.getsize(f)
                print(f"  - {f} ({size:,} bytes)")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) > 2 else None
    
    success = convert(input_file, output_file)
    sys.exit(0 if success else 1)
