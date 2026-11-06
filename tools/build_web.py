#!/usr/bin/env python3
"""Rebuilds everything the web installer serves: firmware parts, manifest and sprite packs.

Works on Windows, macOS and Linux. Needs arduino-cli with the esp32 core and these libraries:
GFX Library for Arduino, SensorLib, XPowersLib.

    python tools/build_web.py                 # firmware + manifest + checks
    python tools/build_web.py --sprites       # ...and redraw the sprite packs too
"""
import glob
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
FQBN = 'esp32:esp32:esp32s3:CDCOnBoot=cdc,FlashSize=16M,PSRAM=opi,PartitionScheme=app3M_fat9M_16MB'
SKETCH = 'Tamanimals'


def boot_app0():
    roots = [os.path.expanduser('~/Library/Arduino15'), os.path.expanduser('~/.arduino15'), os.path.join(os.environ.get('LOCALAPPDATA', ''), 'Arduino15')]
    for r in roots:
        hits = glob.glob(os.path.join(r, 'packages', 'esp32', 'hardware', 'esp32', '*', 'tools', 'partitions', 'boot_app0.bin'))
        if hits:
            return sorted(hits)[-1]
    raise SystemExit('boot_app0.bin not found: is the esp32 core installed?')


def main():
    os.chdir(ROOT)
    if '--sprites' in sys.argv:
        subprocess.check_call([sys.executable, 'tools/gen/gen_sprites.py'])
    # arduino-cli wants the folder named after the .ino, whatever the clone is called
    tmp = tempfile.mkdtemp()
    sk = os.path.join(tmp, SKETCH)
    os.makedirs(sk)
    for pat in ('*.ino', '*.cpp', '*.h'):
        for f in glob.glob(pat):
            shutil.copy(f, sk)
    out = os.path.join(ROOT, 'build', 'firmware')
    os.makedirs(out, exist_ok=True)
    print('Compiling...')
    subprocess.check_call(['arduino-cli', 'compile', '--fqbn', FQBN, '--output-dir', out, sk])
    shutil.rmtree(tmp, ignore_errors=True)

    # FOUR PARTS, each at its own offset, never one image from 0. A merged image pads the
    # gaps with 0xFF and would write straight over the NVS partition at 0x9000: the save.
    fw = os.path.join('web', 'firmware')
    os.makedirs(fw, exist_ok=True)
    shutil.copy(os.path.join(out, SKETCH + '.ino.bootloader.bin'), os.path.join(fw, 'bootloader.bin'))
    shutil.copy(os.path.join(out, SKETCH + '.ino.partitions.bin'), os.path.join(fw, 'partitions.bin'))
    shutil.copy(os.path.join(out, SKETCH + '.ino.bin'), os.path.join(fw, 'app.bin'))
    ba = os.path.join(out, 'boot_app0.bin')
    shutil.copy(ba if os.path.exists(ba) else boot_app0(), os.path.join(fw, 'boot_app0.bin'))

    version = re.search(r'#define FW_VERSION "([^"]+)"', open(SKETCH + '.ino', encoding='utf-8').read()).group(1)
    dex = int(re.search(r'#define DEX_COUNT (\d+)', open('dex.h', encoding='utf-8').read()).group(1))
    # new_install_prompt_erase must stay TRUE, whatever the name suggests. In esp-web-tools'
    # no-Improv path FALSE means "do not ask, just erase the whole chip", save included. TRUE
    # shows an "Erase device" box that starts unchecked, and leaving it unchecked keeps the save.
    manifest = {
        'name': 'Tamanimals', 'version': version, 'dex_count': dex, 'new_install_prompt_erase': True,
        'builds': [{'chipFamily': 'ESP32-S3', 'parts': [
            {'path': 'firmware/bootloader.bin', 'offset': 0},
            {'path': 'firmware/partitions.bin', 'offset': 0x8000},
            {'path': 'firmware/boot_app0.bin', 'offset': 0xE000},
            {'path': 'firmware/app.bin', 'offset': 0x10000},
        ]}],
    }
    with open(os.path.join('web', 'manifest.json'), 'w', newline='\n') as f:
        json.dump(manifest, f, indent=2)
        f.write('\n')
    print('Checking the installer cannot erase a save...')
    subprocess.check_call([sys.executable, 'tools/check_installer.py'])
    print('OK: web/ is ready, firmware', version, 'with', dex, 'animals')


if __name__ == '__main__':
    main()
