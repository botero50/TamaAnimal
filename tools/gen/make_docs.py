#!/usr/bin/env python3
"""Makes the README's pictures: screen captures from the real firmware, and animated GIFs.

    python tools/gen/make_docs.py

Needs g++ (the same as the tests) and the sprites from gen_sprites.py. Writes docs/screens/*.png
and docs/*.gif. Standard library only: the PNG and GIF encoders are below.
"""
import concurrent.futures as cf
import glob
import json
import os
import struct
import subprocess
import sys
import zlib

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
import spritelib as sl

EMU = os.path.join(ROOT, 'tools', 'emu')
BUILD = os.path.join(ROOT, 'build', 'docs')
DOCS = os.path.join(ROOT, 'docs')
N = 466
BG = (0xf2, 0xef, 0xe1)
CORE = ['gbsynth', 'pet', 'i18n', 'party', 'battle', 'link', 'save', 'steps', 'cry', 'talk', 'album', 'weather']


# ---------------------------------------------------------------- image writers
def png(path, w, h, rgba):
    raw = b''.join(b'\0' + bytes(rgba[y * w * 4:(y + 1) * w * 4]) for y in range(h))

    def ch(t, d):
        return struct.pack('>I', len(d)) + t + d + struct.pack('>I', zlib.crc32(t + d))
    with open(path, 'wb') as f:
        f.write(b'\x89PNG\r\n\x1a\n' + ch(b'IHDR', struct.pack('>IIBBBBB', w, h, 8, 6, 0, 0, 0)) + ch(b'IDAT', zlib.compress(raw, 9)) + ch(b'IEND', b''))


def lzw(indices, min_size):
    """GIF's variable-width LZW."""
    clear, eoi = 1 << min_size, (1 << min_size) + 1
    out, bits, nbits = bytearray(), 0, 0
    size, table, nxt = min_size + 1, {}, eoi + 1

    def emit(code):
        nonlocal bits, nbits
        bits |= code << nbits
        nbits += size
        while nbits >= 8:
            out.append(bits & 255)
            bits >>= 8
            nbits -= 8
    emit(clear)
    cur = None
    for px in indices:
        key = (cur, px) if cur is not None else None
        if cur is None:
            cur = px
        elif key in table:
            cur = table[key]
        else:
            emit(cur)
            if nxt < 4096:
                table[key] = nxt
                nxt += 1
                if nxt > (1 << size) and size < 12:
                    size += 1
            else:
                emit(clear)
                table, nxt, size = {}, eoi + 1, min_size + 1
            cur = px
    if cur is not None:
        emit(cur)
    emit(eoi)
    if nbits:
        out.append(bits & 255)
    return bytes(out)


def gif(path, frames, w, h, delays, transparent=None):
    """frames: lists of (r,g,b) tuples or None (transparent). One shared palette, quantised by frequency."""
    from collections import Counter
    count = Counter(px for f in frames for px in f if px is not None)
    pal = [c for c, _ in count.most_common(255 if transparent is not None else 256)]
    index = {c: i for i, c in enumerate(pal)}
    cache = {}

    def near(c):
        if c in index:
            return index[c]
        if c not in cache:
            cache[c] = min(range(len(pal)), key=lambda i: sum((a - b) ** 2 for a, b in zip(pal[i], c)))
        return cache[c]
    tidx = len(pal) if transparent is not None else None
    full = pal + [(0, 0, 0)] * (256 - len(pal))
    out = bytearray(b'GIF89a' + struct.pack('<HHBBB', w, h, 0xF7, 0, 0))
    out += bytes(v for c in full for v in c)
    out += b'\x21\xFF\x0BNETSCAPE2.0\x03\x01\x00\x00\x00'          # loop forever
    for f, d in zip(frames, delays):
        flags = 0x09 if tidx is not None else 0x08                    # restore to background
        out += b'\x21\xF9\x04' + bytes([flags]) + struct.pack('<H', d) + bytes([tidx or 0]) + b'\x00'
        out += b'\x2C' + struct.pack('<HHHHB', 0, 0, w, h, 0)
        data = lzw([tidx if px is None else near(px) for px in f], 8)
        out += b'\x08'
        for i in range(0, len(data), 255):
            out += bytes([len(data[i:i + 255])]) + data[i:i + 255]
        out += b'\x00'
    out += b'\x3B'
    with open(path, 'wb') as fh:
        fh.write(out)


# ---------------------------------------------------------------- screens
def raw_to_rgb(path):
    b = open(path, 'rb').read()
    px = []
    for i in range(N * N):
        c = b[2 * i] | (b[2 * i + 1] << 8)
        px.append((((c >> 11) & 31) << 3, ((c >> 5) & 63) << 2, (c & 31) << 3))
    return px


def round_rgba(px, size):
    """Shrinks a capture and makes the corners outside the round panel transparent."""
    out = bytearray(size * size * 4)
    r = size / 2
    for y in range(size):
        for x in range(size):
            if (x + .5 - r) ** 2 + (y + .5 - r) ** 2 > r * r:
                continue
            c = px[int(y * N / size) * N + int(x * N / size)]
            o = (y * size + x) * 4
            out[o:o + 4] = bytes((*c, 255))
    return out


def capture():
    os.makedirs(BUILD, exist_ok=True)
    flags = ['-std=c++17', '-O1', '-w', '-I' + EMU, '-I' + ROOT, '-DSPRITE_DIR="' + os.path.join(ROOT, 'build', 'sdcard', 'mons').replace('\\', '/') + '"']
    with open(os.path.join(EMU, 'sketch.cpp'), 'w', encoding='utf-8', newline='\n') as f:
        subprocess.check_call([sys.executable, 'genproto.py', os.path.join(ROOT, 'Tamanimals.ino')], cwd=EMU, stdout=subprocess.DEVNULL)
        f.write('#include "proto.h"\n' + open(os.path.join(ROOT, 'Tamanimals.ino'), encoding='utf-8').read())
    srcs = [os.path.join(ROOT, c + '.cpp') for c in CORE] + [os.path.join(EMU, x) for x in ('sketch.cpp', 'host_impl.cpp', 'font.cpp', 'clock.cpp', 'screens.cpp')]

    def obj(s):
        o = os.path.join(BUILD, os.path.basename(s) + '.o')
        subprocess.check_call(['g++', *flags, '-c', s, '-o', o])
        return o
    with cf.ThreadPoolExecutor() as ex:
        objs = list(ex.map(obj, srcs))
    exe = os.path.join(BUILD, 'screens.exe')
    subprocess.check_call(['g++', *flags, '-o', exe, *objs])
    raws = os.path.join(BUILD, 'raw')
    os.makedirs(raws, exist_ok=True)
    subprocess.check_call([exe, raws], cwd=BUILD, stdout=subprocess.DEVNULL)
    return raws


def screens(raws):
    out = os.path.join(DOCS, 'screens')
    os.makedirs(out, exist_ok=True)
    shots = {}
    for p in sorted(glob.glob(os.path.join(raws, '*.raw'))):
        name = os.path.splitext(os.path.basename(p))[0]
        shots[name] = raw_to_rgb(p)
        png(os.path.join(out, name + '.png'), 320, 320, round_rgba(shots[name], 320))
    print(len(shots), 'screens')
    return shots


def tour(shots, names, path, size=300, delay=170):
    frames = []
    for n in names:
        rgba = round_rgba(shots[n], size)
        frames.append([None if rgba[i * 4 + 3] == 0 else tuple(rgba[i * 4:i * 4 + 3]) for i in range(size * size)])
    gif(path, frames, size, size, [delay] * len(frames), transparent=True)
    print(os.path.relpath(path, ROOT), len(frames), 'frames')


# ---------------------------------------------------------------- sprite GIFs
def sprite_gif(path, cells, cols, scale=3, steps=12, ms=14):
    """cells: (name, arch, stage). Every cell walks in place through its own animation."""
    walk = [p for act, poses, _ in sl.ACTIONS if act == 2 for p in poses]
    idle = [p for act, poses, _ in sl.ACTIONS if act == 0 for p in poses]
    seq = walk + walk + idle + idle
    cw = 40 * scale + 8
    rows = (len(cells) + cols - 1) // cols
    w, h = cols * cw, rows * cw
    frames = []
    for k in range(steps):
        img = [BG] * (w * h)
        for i, (name, arch, stage) in enumerate(cells):
            g = sl.draw(name, arch, seq[k % len(seq)], stage=stage)
            ox, oy = (i % cols) * cw + 4, (i // cols) * cw + 4
            for y, row in enumerate(g):
                for x, c in enumerate(row):
                    if c is None:
                        continue
                    v = sl.PAL[c]
                    rgb = ((v >> 16) & 255, (v >> 8) & 255, v & 255)
                    for dy in range(scale):
                        base = (oy + y * scale + dy) * w + ox + x * scale
                        for dx in range(scale):
                            img[base + dx] = rgb
        frames.append(img)
    gif(path, frames, w, h, [ms] * len(frames))
    print(os.path.relpath(path, ROOT), len(frames), 'frames')


def main():
    os.makedirs(DOCS, exist_ok=True)
    animals = {a['name']: a for a in json.load(open(os.path.join(HERE, 'animals.json')))}

    def pick(*names):
        return [(n, animals[n]['arch']) for n in names]
    growth = pick('LION', 'ELEPHANT', 'GIRAFFE', 'FLAMINGO', 'CROCODILE', 'GIANT PANDA')
    sprite_gif(os.path.join(DOCS, 'growing-up.gif'), [(n, a, s) for n, a in growth for s in (0, 1, 2)], cols=6, scale=2)
    parade = pick('LION', 'ZEBRA', 'TIGER', 'SCARLET MACAW', 'RHINOCEROS', 'KING COBRA', 'JAGUAR', 'TOCO TOUCAN',
                  'HIPPOPOTAMUS', 'RED PANDA', 'OSTRICH', 'DART FROG')
    sprite_gif(os.path.join(DOCS, 'animals.gif'), [(n, a, 2) for n, a in parade], cols=6, scale=2)

    shots = screens(capture())
    tour(shots, ['main_cub', 'card_growth', 'grow_dialog', 'main_juvenile', 'main_adult', 'card_growth'], os.path.join(DOCS, 'tour-growth.gif'))
    tour(shots, ['main_adult', 'guide_africa', 'ranger_ladder', 'battle_ranger', 'battle_wild', 'map', 'shop', 'game_whois'], os.path.join(DOCS, 'tour-play.gif'))


if __name__ == '__main__':
    main()
