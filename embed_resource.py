def create_resource_bytes(filename):
    with open(filename, "rb") as f:
        normalized_filename = filename.replace(" ", "_").replace(".", "_")
        print(f"unsigned char {normalized_filename}[] = {{\n\t", end="")
        length = 0
        for line in f:
            for byte in line:
                print(f"0x{byte:02X}", end=", ")
                length += 1
                if length % 12 == 0:
                    print("\n\t", end="")
        print("\n};")
        print(f"unsigned int {normalized_filename}_length = {length};")

def main():
    create_resource_bytes("close.png")

if __name__ == "__main__":
    main()