import os

def normalize_filename(filename):
    return os.path.basename(filename).replace(" ", "_").replace(".", "_")


def create_resource_bytes(filename):
    string = ""
    with open(filename, "rb") as f:
        tab = " " * 4
        string += f"const unsigned char {normalize_filename(filename)}[] = {{\n{tab}"
        length = 0
        for line in f:
            for byte in line:
                string += f"0x{byte:02X}, "
                length += 1
                if length % 12 == 0:
                    string += f"\n{tab}"
        string += "\n};\n"
        string += f"const unsigned int {normalize_filename(filename)}_length = {length};\n\n"
    return string


def main():
    BASE_DIR = "images"
    images = os.listdir(BASE_DIR)
    # We do not want to embed this file since its just for the README
    images.remove("screenshot_mhwi_db.png")
    cpp = open("src/resources.cpp", "w")
    cpp.write('#include "resources.hpp"\n\n')
    hpp = open("src/resources.hpp", "w")
    hpp.write("#pragma once\n\n")

    for img in images:
        cpp.write(create_resource_bytes(f"{BASE_DIR}/{img}"))
        hpp.write(f"extern const unsigned char {normalize_filename(img)}[];\n")
        hpp.write(f"extern const unsigned int {normalize_filename(img)}_length;\n\n")

    cpp.close()
    hpp.close()


if __name__ == "__main__":
    main()