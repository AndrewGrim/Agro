import subprocess
import os

ESCAPE_RED = "\033[38;2;255;0;0m"
ESCAPE_GREEN = "\033[38;2;0;255;0m"
ESCAPE_BLUE = "\033[38;2;0;0;255m"
ESCAPE_YELLOW = "\033[38;2;255;255;0m"
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
BIN_PATH = "Agro/bin"

executables = os.listdir(BIN_PATH)
i = 0
for exe in executables:
    command = [f"./{BIN_PATH}/{exe}", "quit"]

    p = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    p.wait()
    if p.returncode != 0 and p.returncode != None:
        print(f"{{{error(' FAIL ')}}} '{exe}'")
        while True:
            line = p.stderr.readline()
            if not line:
                break
            print(warn(line.rstrip().decode("UTF-8")))
        failed += 1
    else:
        print(f"{{{success(' PASS ')}}} '{exe}'")
        passed += 1
    i += 1

print(f"Passed: {success(passed)}/{passed + failed}")
if failed > 0:
    print(f"Failed: {error(failed)}/{passed + failed}")
else:
    print(f"Failed: {failed}/{passed + failed}")