def create_resource_bytes(filename):
    with open(filename, "rb") as f:
        tab = " " * 4
        normalized_filename = filename.replace(" ", "_").replace(".", "_")
        print(f"unsigned char {normalized_filename}[] = {{\n{tab}", end="")
        length = 0
        for line in f:
            for byte in line:
                print(f"0x{byte:02X}", end=", ")
                length += 1
                if length % 12 == 0:
                    print(f"\n{tab}", end="")
        print("\n};")
        print(f"unsigned int {normalized_filename}_length = {length};\n")

def main():
    images = ["close.png", "close_thin.png", "up_arrow.png"]
    for img in images:
        create_resource_bytes(img)

if __name__ == "__main__":
    main()