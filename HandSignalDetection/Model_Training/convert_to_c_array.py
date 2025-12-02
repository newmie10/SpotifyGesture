import sys

def convert(file_path):
    data = open(file_path, "rb").read()
    name = file_path.replace(".", "_")
    print(f"const unsigned char g_model[] = {{")
    for i, b in enumerate(data):
        if i % 12 == 0:
            print(" ", end="")
        print(f"0x{b:02x}, ", end="")
        if i % 12 == 11:
            print()
    print("};")
    print(f"const int g_model_len = {len(data)};")

if __name__ == "__main__":
    convert(sys.argv[1])
