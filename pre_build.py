import subprocess
import datetime
import os
import configparser
Import("env")



# 1. Haal de Git branch op
try:
    branch = subprocess.check_output(['git', 'rev-parse', '--abbrev-ref', 'HEAD']).strip().decode('utf-8')
except:
    branch = "unknown"

# 2. Beheer het buildnummer (opgeslagen in build_no.txt)
try:
    with open("build_no.txt", "r") as f:
        build_no = int(f.read().strip()) + 1
except FileNotFoundError:
    build_no = 1

with open("build_no.txt", "w") as f:
    f.write(str(build_no))

version = env.GetProjectOption("custom_version", "1.0.0")
print(f"--- Bouwen van versie: {version} ---")

# Haalt de architectuur/MCU op (bijv. "esp32c3" of "esp32s3")
mcu = env.BoardConfig().get("build.mcu")

# 3. Schrijf version.h
with open("include/version.h", "w") as f:
    f.write("// DIT BESTAND IS AUTOMATISCH GEGENEREERD - NIET HANDMATIG AANPASSEN\n")
    f.write("#ifndef VERSION_H\n#define VERSION_H\n\n")
    f.write(f'#define VERSION "{version}"\n')
    f.write(f'const char* BUILD_NUMBER = "{build_no}";\n')
    f.write(f'const char* GIT_BRANCH = "{branch}";\n')
    f.write(f'const char* BOARD = "{mcu}";\n')
    f.write(f'const char* BUILD_DATE = "{datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")}";\n')
    f.write("\n#endif")

print(f"--- Version.h gegenereerd: Build {build_no} op branch {branch} ---")