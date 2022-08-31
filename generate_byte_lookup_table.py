def main():
    luts = {
        "is_identifier": b"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789",
        "is_identifier_start": b"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_",
        "is_number": b"0123456789abcdefABCDEFuUlLxXpP.'",
        "is_number_start": b"0123456789",
    }
    for lut in luts.items():
        print(f"const u8 {lut[0]}[256] = {{")
        print("    ", end="")
        length = 0;
        for byte in range(0, 256):
            print(f"{int(byte in lut[1])}", end="")
            if byte != 255: print(", ", end="")
            length += 1
            if length % 10 == 0: print("\n    ", end="")
        print("\n};\n")


if __name__ == "__main__":
    main()
