import subprocess
import os

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

passed = 0
failed = 0

test_files = os.listdir("tests")

# Run `make local_build` which creates all required .o files and the build dir 
command = ["make", "local_build"]
p = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
p.wait()
if p.returncode != 0 and p.returncode != None:
    print(f"{error('Compilation Error')}: ")
    while True:
        line = p.stderr.readline()
        if not line:
            break
        print(warn(line.rstrip().decode("UTF-8")))
    print(f"Occured when running command: '{command}'")
else:
    object_files = os.listdir("build")
    for obj in object_files:
        if not obj.endswith(".o") or obj == "main.o":
            object_files.remove(obj)

    for test in filter(lambda f: f.endswith(".cpp"), test_files):
        filename = test.replace(".cpp", "")

        index = 2
        command = [
            "g++",
            f"tests/{test}",
            "-DTEST",
            "-Iinclude", "-I/usr/include", "-I/usr/include/freetype2",
            "-Llib", "-L/usr/lib/i386-linux-gnu", "-L/usr/lib",
            "-lGL", "-lSDL2", "-lfreetype", "-lX11", "-lpthread", "-lXrandr", "-lXi", "-ldl",
            "-o", f"{filename}.out",
        ]
        for obj in object_files:
            command.insert(index, f"build/{obj}")
            index += 1

        run = [
            f"./{filename}.out",
        ]

        p = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        p.wait()
        if p.returncode != 0 and p.returncode != None:
            print(f"{error('Compilation Error')}: '{test}'")
            while True:
                line = p.stderr.readline()
                if not line:
                    break
                print(warn(line.rstrip().decode("UTF-8")))
            print(f"Occured when running command: '{command}'")
            failed += 1
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