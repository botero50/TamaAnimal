#!/usr/bin/env python3
"""Draws every animal, writes the microSD files and packs them for the web installer.

Output: build/sdcard/mons/{p,j,c}NNN.bin (adult, juvenile, cub) + thumbs.bin (scratch, gitignored) and
web/sprites-<region>.pak (published, one per region so a player can install a part).

TPAK format (little-endian): "TPAK", u16 count, count x { u8 nameLen, name, u32 size }, then the
data of each file in the same order. The installer splits it and sends each file to the board.

The card folder is /animals, on purpose: a reused card can hold other games' /mons files with the
same names, and those must never be drawn as animals.

    python tools/gen/gen_sprites.py
"""
import json
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.join(HERE, '..', '..')
sys.path.insert(0, HERE)
import spritelib as sl

ANIMALS = json.load(open(os.path.join(HERE, 'animals.json')))
REGIONS = [('africa', 1, 145), ('asia', 146, 200), ('americas', 201, 325)]
MONS = os.path.join(ROOT, 'build', 'sdcard', 'mons')
WEB = os.path.join(ROOT, 'web')


def thumb(a):
    g = sl.draw(a['name'], a['arch'], {})
    used = sorted({c for row in g for c in row if c})
    idx = {c: i for i, c in enumerate(used)}
    blob = struct.pack('<3B', 40, 40, len(used))
    blob += struct.pack(f'<{len(used)}H', *[sl.rgb565(sl.PAL[c]) for c in used])
    blob += bytes(0xFF if c is None else idx[c] for row in g for c in row)
    return blob


def main():
    os.makedirs(MONS, exist_ok=True)
    os.makedirs(WEB, exist_ok=True)
    files = {}
    # three files per animal: c = cub, j = juvenile, p = adult (the firmware falls back to p)
    for a in ANIMALS:
        for stage, prefix in sl.STAGE_PREFIX.items():
            data = sl.tpk2(a['name'], a['arch'], stage)
            name = f'{prefix}{a["id"]:03d}.bin'
            with open(os.path.join(MONS, name), 'wb') as f:
                f.write(data)
            files[(a['id'], prefix)] = (name, data)
    blobs = [thumb(a) for a in ANIMALS]
    pos = 6 + 4 * len(blobs)
    offs = []
    for b in blobs:
        offs.append(pos)
        pos += len(b)
    th = b'TPTH' + struct.pack('<H', len(blobs)) + struct.pack(f'<{len(offs)}I', *offs) + b''.join(blobs)
    with open(os.path.join(MONS, 'thumbs.bin'), 'wb') as f:
        f.write(th)
    for region, lo, hi in REGIONS:
        entries = [('animals/thumbs.bin', th)] + [('animals/' + files[(i, pre)][0], files[(i, pre)][1]) for i in range(lo, hi + 1) for pre in 'pjc']
        out = os.path.join(WEB, f'sprites-{region}.pak')
        with open(out, 'wb') as o:
            o.write(b'TPAK' + struct.pack('<H', len(entries)))
            for n, blob in entries:
                nb = n.encode()
                o.write(struct.pack('<B', len(nb)) + nb + struct.pack('<I', len(blob)))
            for _, blob in entries:
                o.write(blob)
        print(f'{os.path.normpath(out)}: {len(entries)} files, {os.path.getsize(out) / 1048576:.1f} MB')


if __name__ == '__main__':
    main()
