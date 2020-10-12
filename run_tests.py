import subprocess

ESCAPE_RED = "\033[91m"
ESCAPE_GREEN = "\033[96m"
ESCAPE_YELLOW = "\033[93m"
ESCAPE_BOLD = "\033[1m"
ESCAPE_END = "\033[0m"

def error(message):
    return f"{ESCAPE_RED}{ESCAPE_BOLD}{message}{ESCAPE_END}"

def success(message):
    return f"{ESCAPE_GREEN}{ESCAPE_BOLD}{message}{ESCAPE_END}"

def warn(message):
    return f"{ESCAPE_YELLOW}{ESCAPE_BOLD}{message}{ESCAPE_END}"

test_files = [
    "tests/box_vertical.cpp",
    "tests/box_horizontal.cpp",
]

passed = 0
failed = 0

for test in test_files:
    filename = test.replace("tests/", "").replace(".cpp", "")

    command = [
        "g++",
        f"{test}",
        "src/app.cpp",
        "src/renderer/glad.c",
        "src/renderer/shader.cpp", "src/renderer/texture.cpp", "src/renderer/resource_manager.cpp",
        "src/renderer/text_renderer.cpp", "src/renderer/stb_image.cpp",
        "src/controls/widget.cpp", "src/controls/scrollbar.cpp", "src/controls/slider.cpp", "src/controls/box.cpp", "src/controls/scrolledbox.hpp",
        "-DTEST",
        "-Iinclude", "-I/usr/include", "-I/usr/include/freetype2",
        "-Llib", "-L/usr/lib/i386-linux-gnu", "-L/usr/lib",
        "-lGL", "-lSDL2", "-lfreetype", "-lX11", "-lpthread", "-lXrandr", "-lXi", "-ldl",
        "-o", f"{filename}.out",
    ]

    run = [
        f"./{filename}.out",
    ]

    p = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    p.wait()
    if p.returncode != 0 and p.returncode != None:
        print(f"{error('Compilation Error')}: '{test}")
        while True:
            line = p.stderr.readline()
            if not line:
                break
            print(warn(line.rstrip().decode("UTF-8")))
        print(f"Occured when running command: '{command}'")
    else:
        p = subprocess.Popen(run, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        p.wait()
        if p.returncode != 0 and p.returncode != None:
            print(f"{{{error(' FAIL ')}}} '{test}'")
            while True:
                line = p.stderr.readline()
                if not line:
                    break
                print(warn(line.rstrip().decode("UTF-8")))
            failed += 1
        else:
            print(f"{{{success(' PASS ')}}} '{test}'")
            passed += 1

print(f"Passed: {success(passed)}/{passed + failed}")
if failed > 0:
    print(f"Failed: {error(failed)}/{passed + failed}")
else:
    print(f"Failed: {failed}/{passed + failed}")