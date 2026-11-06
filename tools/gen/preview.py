#!/usr/bin/env python3
"""Writes a PNG contact sheet of the generated animals, for checking the art by eye.

    python tools/gen/preview.py out.png [first_id] [count] [pose]
"""
import json
import os
import struct
import sys
import zlib

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import spritelib as sl


def png(path, w, h, rgb):
    raw = b''.join(b'\0' + bytes(rgb[y * w * 3:(y + 1) * w * 3]) for y in range(h))

    def ch(t, d):
        return struct.pack('>I', len(d)) + t + d + struct.pack('>I', zlib.crc32(t + d))
    with open(path, 'wb') as f:
        f.write(b'\x89PNG\r\n\x1a\n' + ch(b'IHDR', struct.pack('>IIBBBBB', w, h, 8, 2, 0, 0, 0)) + ch(b'IDAT', zlib.compress(raw)) + ch(b'IEND', b''))


def sheet(cells, path, cols=12, scale=2, size=40, bg=(0xf2, 0xef, 0xe1)):
    rows = (len(cells) + cols - 1) // cols
    cw = size * scale + 4
    w, h = cols * cw, rows * cw
    img = bytearray(bytes(bg) * (w * h))
    for i, g in enumerate(cells):
        ox, oy = (i % cols) * cw + 2, (i // cols) * cw + 2
        for y, row in enumerate(g):
            for x, c in enumerate(row):
                if c is None:
                    continue
                v = sl.PAL[c]
                px = bytes(((v >> 16) & 255, (v >> 8) & 255, v & 255))
                for dy in range(scale):
                    for dx in range(scale):
                        o = ((oy + y * scale + dy) * w + ox + x * scale + dx) * 3
                        img[o:o + 3] = px
    png(path, w, h, img)


if __name__ == '__main__':
    animals = json.load(open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'animals.json')))
    first = int(sys.argv[2]) if len(sys.argv) > 2 else 1
    count = int(sys.argv[3]) if len(sys.argv) > 3 else 60
    chosen = [a for a in animals if a['id'] >= first][:count]
    sheet([sl.draw(a['name'], a['arch'], {}) for a in chosen], sys.argv[1])
    print(len(chosen), 'cells')
