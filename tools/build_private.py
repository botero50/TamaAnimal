#!/usr/bin/env python3
"""Builds YOUR OWN copy of the game with extra regions of your own, kept off the public repo.

Put a region in private/<anything>.json (the folder is gitignored):

    {"region": "NAME", "starters": [id, ...], "rangers": [[name, place, TYPE, team size, lead level], ...13],
     "ring": [[colour, colour] x8],   (optional: the stamp colours, as spritelib letters)
     "animals": [{"id", "name", "arch", "rarity", "biome", "type1", "type2", "look"}, ...]}

Ids carry on after the last public animal. This regenerates the headers, the calls, the stamps and
every sprite pack into build/private/, compiles the firmware there, and makes a local installer:

    python tools/build_private.py
    python -m http.server 8000 -d build/private/web      then open http://localhost:8000

Nothing here writes into the repo's own files, and build/ and private/ are both gitignored.
"""
import glob
import json
import os
import re
import shutil
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GEN = os.path.join(ROOT, 'tools', 'gen')
OUT = os.path.join(ROOT, 'build', 'private')
SK = os.path.join(OUT, 'Tamanimals')
sys.path.insert(0, GEN)
sys.path.insert(0, os.path.join(ROOT, 'tools'))


def main():
    regions = [json.load(open(p, encoding='utf-8')) for p in sorted(glob.glob(os.path.join(ROOT, 'private', '*.json')))]
    if not regions:
        raise SystemExit('No private/*.json: nothing private to build.')
    shutil.rmtree(SK, ignore_errors=True)
    os.makedirs(SK)
    for pat in ('*.ino', '*.cpp', '*.h'):
        for f in glob.glob(os.path.join(ROOT, pat)):
            shutil.copy(f, SK)

    import gen_dex, gen_audio, gen_art, gen_sprites
    for g in (gen_dex, gen_audio, gen_art):
        g.ROOT = SK                       # every header lands in the private sketch copy
    for r in regions:
        a = r['animals']
        lo, hi = a[0]['id'], a[-1]['id']
        gen_dex.ANIMALS += a
        gen_audio.ANIMALS += a
        gen_sprites.ANIMALS += a
        gen_dex.REGIONS.append((r['region'], lo, hi, r['starters']))
        gen_dex.ROSTER[r['region']] = [tuple(x) for x in r['rangers']]
        gen_art.RING[r['region']] = [tuple(x) for x in r.get('ring', [('f', 'Y'), ('B', 'N'), ('r', 'R'), ('f', 'R'), ('B', 'Y'), ('r', 'Y'), ('f', 'N'), ('r', 'N')])]
        gen_sprites.REGIONS.append((r['region'].lower(), lo, hi))
        print(f"{r['region']}: {len(a)} animals, {lo}-{hi}")
    assert [x['id'] for x in gen_dex.ANIMALS] == list(range(1, len(gen_dex.ANIMALS) + 1)), 'ids must carry on without gaps'
    gen_dex.gen_dex(); gen_dex.gen_moves(); gen_dex.gen_trainers()
    gen_audio.gen_cries()
    gen_art.gen_badges()

    web = os.path.join(OUT, 'web')
    shutil.rmtree(web, ignore_errors=True)
    shutil.copytree(os.path.join(ROOT, 'web'), web, ignore=shutil.ignore_patterns('*.pak'))
    gen_sprites.MONS = os.path.join(OUT, 'sdcard', 'mons')
    gen_sprites.WEB = web
    gen_sprites.main()

    fw_out = os.path.join(OUT, 'firmware')
    os.makedirs(fw_out, exist_ok=True)
    import build_web
    print('Compiling your private build...')
    subprocess.check_call(['arduino-cli', 'compile', '--fqbn', build_web.FQBN, '--output-dir', fw_out, SK])
    fw = os.path.join(web, 'firmware')
    for src, dst in (('Tamanimals.ino.bootloader.bin', 'bootloader.bin'), ('Tamanimals.ino.partitions.bin', 'partitions.bin'), ('Tamanimals.ino.bin', 'app.bin')):
        shutil.copy(os.path.join(fw_out, src), os.path.join(fw, dst))
    ba = os.path.join(fw_out, 'boot_app0.bin')
    shutil.copy(ba if os.path.exists(ba) else build_web.boot_app0(), os.path.join(fw, 'boot_app0.bin'))
    version = re.search(r'#define FW_VERSION "([^"]+)"', open(os.path.join(SK, 'Tamanimals.ino'), encoding='utf-8').read()).group(1)
    dex = len(gen_dex.ANIMALS)
    man = json.load(open(os.path.join(ROOT, 'web', 'manifest.json')))
    man.update(version=version + '-private', dex_count=dex)
    with open(os.path.join(web, 'manifest.json'), 'w', newline='\n') as f:
        json.dump(man, f, indent=2)
        f.write('\n')
    # a button for each private region, next to the public ones
    page = os.path.join(web, 'index.html')
    s = open(page, encoding='utf-8').read()
    anchor = s.index('<button id="autol"')
    end = s.index('</button>', anchor) + len('</button>')
    extra = ''.join(f'\n      <button id="autop{i}" data-region="{r["region"].lower()}" disabled>⬇️ {r["region"].title()} ({len(r["animals"])}, private)</button>'
                    for i, r in enumerate(regions))
    s = s[:end] + extra + s[end:]
    s = s.replace('<title>', '<title>PRIVATE ')
    open(page, 'w', encoding='utf-8', newline='\n').write(s)
    print(f'OK: your private build {version}-private with {dex} animals is in build/private/web')
    print('Serve it with:  python -m http.server 8000 -d build/private/web   then open http://localhost:8000')


if __name__ == '__main__':
    main()
