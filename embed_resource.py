def normalize_filename(filename):
    return filename.replace(" ", "_").replace(".", "_")


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
    images = ["close.png", "close_thin.png", "up_arrow.png"]
    cpp = open("src/resources.cpp", "w")
    hpp = open("src/resources.hpp", "w")
    hpp.write("#pragma once\n\n")

    for img in images:
        cpp.write(create_resource_bytes(img))
        hpp.write(f"extern const unsigned char {normalize_filename(img)}[];\n")
        hpp.write(f"extern const unsigned char {normalize_filename(img)}_length;\n\n")

    cpp.close()
    hpp.close()


if __name__ == "__main__":
    main()