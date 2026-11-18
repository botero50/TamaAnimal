#!/usr/bin/env python3
"""Tamanimals' own pixel-art engine: every animal is drawn by code from a body plan.

Nothing is traced or derived from any existing sprite. Standard library only.
A pose is a dict: leg (0-3 walk phase), dy (bob), head (head offset), eye
('open', 'shut', 'hurt', 'sad', 'happy', 'tired', 'sick'), mouth (bool), low (lying down), lunge (x shift),
and the extras: tear / sweat / drool (drop offset), cheek (pale), dirt (mud on the lower body).
"""
import zlib

# Letter -> RGB888. The same letters species.h maps to RGB565.
PAL = {
    'k': 0x1b1b25, 'w': 0xffffff, 'y': 0xf8d990, 'Y': 0xe0b860, 'o': 0xf5863d, 'O': 0xd65f28, 'r': 0xe8503a, 'R': 0xb53224,
    'f': 0xffd95e, 't': 0x8fd6b4, 'T': 0x5fae8c, 'g': 0x58b868, 'G': 0x3c8a4c, 'd': 0x3f7e62, 'p': 0xf08aa4, 'P': 0xc75f80,
    'b': 0x7cc4ea, 'B': 0x4f93c4, 'N': 0x3a6fa0, 'M': 0x2a5278, 'c': 0xb07a45, 'C': 0x7e5530, 'l': 0x9aa9e0, 'L': 0x6f7cb8,
    's': 0xaab0bc, 'S': 0x787e8c,
}
LETTERS = list(PAL)
SHADE = {'w': 's', 's': 'S', 'S': 'k', 'y': 'Y', 'Y': 'c', 'f': 'Y', 'o': 'O', 'O': 'R', 'r': 'R', 'R': 'C', 't': 'T', 'T': 'd', 'g': 'G',
         'G': 'd', 'd': 'M', 'p': 'P', 'P': 'R', 'b': 'B', 'B': 'N', 'N': 'M', 'M': 'k', 'c': 'C', 'C': 'S', 'l': 'L', 'L': 'M', 'k': 'k'}


def rgb565(v):
    r, g, b = (v >> 16) & 255, (v >> 8) & 255, v & 255
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)


class Canvas:
    def __init__(self, size, zoom=1.0):
        """zoom < 1 draws the same plan smaller, still standing on the ground and centred."""
        self.n = size
        self.f = size / 40.0 * zoom
        self.ox = int(round(size / 2 * (1 - zoom)))
        self.oy = int(round(size * 38 / 40 * (1 - zoom)))
        self.g = [[None] * size for _ in range(size)]

    def put(self, x, y, c):
        x, y = x + self.ox, y + self.oy
        if 0 <= x < self.n and 0 <= y < self.n:
            self.g[y][x] = c

    def oval(self, cx, cy, rx, ry, c):
        f = self.f
        cx, cy, rx, ry = cx * f, cy * f, max(rx * f, .6), max(ry * f, .6)
        for y in range(int(cy - ry) - 1, int(cy + ry) + 2):
            for x in range(int(cx - rx) - 1, int(cx + rx) + 2):
                if ((x + .5 - cx) / rx) ** 2 + ((y + .5 - cy) / ry) ** 2 <= 1:
                    self.put(x, y, c)

    def rect(self, x0, y0, x1, y1, c):
        f = self.f
        for y in range(int(round(y0 * f)), max(int(round(y1 * f)), int(round(y0 * f)) + 1)):
            for x in range(int(round(x0 * f)), max(int(round(x1 * f)), int(round(x0 * f)) + 1)):
                self.put(x, y, c)

    def line(self, x0, y0, x1, y1, w, c):
        steps = max(abs(x1 - x0), abs(y1 - y0), 1) * 2
        for i in range(int(steps) + 1):
            t = i / steps
            self.oval(x0 + (x1 - x0) * t, y0 + (y1 - y0) * t, w / 2, w / 2, c)

    def dot(self, x, y, c):
        f = self.f
        self.put(int(x * f), int(y * f), c)

    def finish(self):
        """Rim shading on the underside, then a one-pixel dark outline."""
        n, g = self.n, self.g
        out = [row[:] for row in g]
        for y in range(n):
            for x in range(n):
                c = g[y][x]
                if c and c != 'k' and (y + 1 >= n or g[y + 1][x] is None):
                    out[y][x] = SHADE.get(c, c)
        for y in range(n):
            for x in range(n):
                if g[y][x] is None and any(0 <= x + a < n and 0 <= y + b < n and g[y + b][x + a] is not None
                                           for a, b in ((1, 0), (-1, 0), (0, 1), (0, -1))):
                    out[y][x] = 'k'
        return out


def eye(c, x, y, pose, big=False):
    if not hasattr(c, 'eyes'):
        c.eyes = []
    c.eyes.append((x, y))
    kind = pose.get('eye', 'open')
    if kind == 'shut':
        c.rect(x - 1, y, x + 2, y + 1, 'k')
    elif kind == 'hurt':
        for a, b in ((-1, -1), (1, 1), (-1, 1), (1, -1), (0, 0)):
            c.dot(x + a, y + b, 'k')
    elif kind == 'sad':        # looking down, the brow raised towards the snout
        c.rect(x - 1, y, x + 1, y + 2, 'w')
        c.rect(x, y + 1, x + 1, y + 2, 'k')
        c.dot(x - 1, y - 1, 'k')
        c.dot(x, y - 2, 'k')
        c.dot(x + 1, y - 2, 'k')
    elif kind == 'happy':      # a smiling arc
        c.dot(x - 1, y + 1, 'k')
        c.dot(x, y, 'k')
        c.dot(x + 1, y + 1, 'k')
    elif kind == 'tired':      # a heavy lid over half the eye
        c.rect(x - 1, y, x + 1, y + 2, 'w')
        c.rect(x, y + 1, x + 1, y + 2, 'k')
        c.rect(x - 1, y, x + 2, y + 1, 'k')
    elif kind == 'sick':       # a woozy squiggle
        for a, b in ((-1, 1), (0, 0), (1, 1), (2, 0)):
            c.dot(x + a, y + b, 'k')
    else:
        c.rect(x - 1, y - 1, x + 2 if big else x + 1, y + 2, 'w')
        c.rect(x, y - 1, x + 2 if big else x + 1, y + 2, getattr(c, 'eyecol', 'k'))


def extras(c, pose, seed):
    """Tears, sweat, drool, a pale cheek and mud: placed from where the eyes were drawn."""
    eyes = getattr(c, 'eyes', [])
    if eyes:
        x, y = eyes[-1]
        if 'tear' in pose:
            t = pose['tear']
            c.dot(x, y + 2 + t, 'b')
            c.dot(x, y + 3 + t, 'B')
        if 'sweat' in pose:
            t = pose['sweat']
            c.dot(x + 3, y - 4 + t, 'b')
            c.dot(x + 3, y - 3 + t, 'b')
        if pose.get('cheek'):
            c.dot(x - 1, y + 2, 't')
            c.dot(x, y + 2, 't')
        if 'drool' in pose:
            t = pose['drool']
            c.dot(x + 3, y + 5 + t, 'b')
    if pose.get('dirt'):
        pts = [(gx, gy) for gy, row in enumerate(c.g) for gx, v in enumerate(row) if v is not None]
        if pts:
            top = min(p[1] for p in pts)
            bot = max(p[1] for p in pts)
            low = [p for p in pts if p[1] >= top + (bot - top) * .45]
            r = seed
            for _ in range(max(5, len(low) // 30)):
                r = (r * 1103515245 + 12345) & 0x7fffffff
                gx, gy = low[r % len(low)]
                for a, b in ((0, 0), (1, 0), (0, 1)):
                    if 0 <= gy + b < c.n and 0 <= gx + a < c.n and c.g[gy + b][gx + a] is not None:
                        c.g[gy + b][gx + a] = 'C'


def legs(c, xs, top, foot, w, col, dark, pose):
    """Four legs in two pairs; phase moves opposite pairs."""
    ph = pose.get('leg', 0)
    if pose.get('low'):
        return
    swing = {0: (1, -1), 1: (0, 0), 2: (-1, 1), 3: (0, 0)}[ph]
    for i, x in enumerate(xs):
        s = swing[i % 2]
        lift = 1 if s < 0 and ph in (0, 2) else 0
        tone = dark if i in (1, 3) else col
        c.rect(x + s, top, x + s + w, foot - lift, tone)


def quad(c, a, pose):
    """Four-legged mammal. a holds the proportions and the markings."""
    col, dark, belly = a['col'], a['dark'], a['belly']
    dy = pose.get('dy', 0)
    low = pose.get('low')
    lunge = pose.get('lunge', 0)
    br = a.get('body', 1.0)
    foot = 38
    leglen = a.get('leg', 8)
    if low:
        leglen = 1
    by = foot - leglen - 6 * br + dy
    bx = 18 + lunge
    rx, ry = 11 * br * a.get('long', 1.0), 7 * br
    hx = bx + rx - 1 + a.get('neckx', 2)
    hy = by - a.get('neck', 5) + pose.get('head', 0)
    if low:
        hy = by + 1
    hr = a.get('head', 5)
    # tail
    tx = bx - rx
    tail = a.get('tail', 'thin')
    if tail == 'thin':
        c.line(tx + 1, by - 2, tx - 4, by - 4 + (pose.get('leg', 0) % 2), 1.3, dark)
    elif tail == 'tuft':
        c.line(tx + 1, by - 2, tx - 4, by + 3, 1.2, dark)
        c.oval(tx - 4, by + 4, 2, 2, a['mark'])
    elif tail == 'bushy':
        c.oval(tx - 3, by - 3, 4, 2.5, col)
    elif tail == 'many':                       # a fan of bushy tails with pale tips
        n = a.get('tails', 3)
        for i in range(n):
            t = i / max(1, n - 1)
            ex, ey = tx - 9 + t * 6, by - 13 + t * 15
            c.line(tx + 1, by - 2, ex, ey, 1.3, a.get('tailcol', 'f') if i % 2 else col)
            c.oval(ex, ey, 1.4, 1.2, a['mark'])
    xs = [bx - rx * .75, bx - rx * .35, bx + rx * .25, bx + rx * .6]
    n = a.get('legs_n', 4)
    if n == 8:
        xs = [v for x in xs for v in (x - 1.6, x + 1.4)]
    elif n == 3:
        xs = [xs[0], xs[2], xs[3]]
    elif n == 1:
        xs = [bx - 1]
    legs(c, xs, by + ry * .5, foot + dy, a.get('legw', 3), col, dark, pose)
    c.oval(bx, by, rx, ry, col)
    c.oval(bx + 1, by + ry * .55, rx * .75, ry * .42, belly)
    if a.get('hump'):
        c.oval(bx - 2, by - ry - 1, 5, 4, col)
    # markings on the body
    m = a.get('pattern')
    if m == 'stripes':
        for i in range(-3, 4):
            c.line(bx + i * 3, by - ry + 1, bx + i * 3 - 1, by + ry * .4, 1.2, a['mark'])
    elif m == 'spots':
        for i, (sx, sy) in enumerate(((-6, -2), (-2, -4), (3, -3), (6, 0), (-4, 2), (1, 1))):
            c.oval(bx + sx * br, by + sy * br, 1.3, 1.1, a['mark'])
    elif m == 'patches':
        for sx, sy in ((-6, -2), (0, -3), (5, -1), (-2, 2)):
            c.oval(bx + sx * br, by + sy * br, 2.2, 1.9, a['mark'])
    if a.get('spikes'):                        # a row of spines down the back
        for i in range(-3, 3):
            c.line(bx + i * 3, by - ry + 1, bx + i * 3 - 1, by - ry - 3, 1.3, a['mark'])
    if a.get('wings'):                         # a feathered wing, raised on the up beat
        up = pose.get('leg', 0) in (1, 3) or pose.get('dy', 0) < 0 or lunge > 0
        rx0, ry0 = bx - 1, by - ry + 2
        tip = (bx - 9, by - ry - 11) if up else (bx - 13, by - ry - 4)
        wc = a.get('wingcol', belly)
        for i in range(4):
            c.line(rx0 + i * 1.5, ry0, tip[0] + i * 2.2, tip[1] + i * 1.6, 2.4, wc if i % 2 == 0 else SHADE.get(wc, wc))
    # neck and head
    if a.get('neck', 5) > 0 and not low:
        c.line(bx + rx * .6, by - 1, hx - 1, hy + 1, a.get('neckw', 5), col)
    if a.get('hmane') and not low:             # a horse's mane along the neck
        c.line(bx + rx * .45, by - ry + 1, hx - 2, hy - a.get('head', 5) + 1, 2.2, a['mark'])
    def head(hx, hy, a):
        col = a['col']
        if a.get('mane'):
            c.oval(hx - 1, hy, hr + 3.5, hr + 3, a['mark'])
        ear = a.get('ear', 'round')
        if ear == 'round':
            c.oval(hx - 3, hy - hr, 2, 2, dark)
        elif ear == 'point':
            c.line(hx - 3, hy - hr + 1, hx - 4, hy - hr - 3, 2, dark)
            c.line(hx + 1, hy - hr + 1, hx + 1, hy - hr - 3, 1.6, dark)
        elif ear == 'long':
            c.line(hx - 3, hy - hr + 1, hx - 7, hy - hr - 1, 2, dark)
        elif ear == 'big':
            c.oval(hx - 5, hy - 1, 5, 6, dark)
        c.oval(hx, hy, hr, hr * .92, a.get('headcol', col))
        snout = a.get('snout', 2)
        if snout:
            c.oval(hx + hr - 1, hy + 1.5, snout + 1, 2, belly)
            c.dot(hx + hr + snout - 1, hy + 1, 'k')
        if a.get('horn') == 'nose':
            c.line(hx + hr, hy, hx + hr + 3, hy - 5, 1.8, 's')
        elif a.get('horn') == 'curve':
            c.line(hx - 2, hy - hr, hx - 7, hy - hr - 5, 1.4, 'C')
            c.line(hx, hy - hr, hx - 3, hy - hr - 6, 1.4, 'C')
        elif a.get('horn') == 'uni':
            c.line(hx + 1, hy - hr + 1, hx + 4, hy - hr - 5, 1.4, a.get('horncol', 'f'))
        elif a.get('horn') == 'ossicone':
            c.line(hx - 1, hy - hr, hx - 1, hy - hr - 3, 1, 'C')
            c.line(hx + 2, hy - hr, hx + 2, hy - hr - 3, 1, 'C')
        if a.get('trunk'):
            c.line(hx + hr - 1, hy + 2, hx + hr + 3, hy + 9 + pose.get('head', 0), 2.6, col)
        if a.get('tusk'):
            c.line(hx + hr - 1, hy + 4, hx + hr + 2, hy + 5, 1.1, 'w')
        if a.get('qbeak'):                         # an eagle's hooked beak
            bc = a.get('beakcol', 'f')
            c.line(hx + hr - 1, hy, hx + hr + 3, hy, 2.4, bc)
            c.line(hx + hr + 3, hy, hx + hr + 3, hy + 2.5, 1.3, bc)
        eye(c, hx + 1, hy - 1, pose)
        if pose.get('mouth'):
            c.rect(hx + 2, hy + 3, hx + hr, hy + 4, 'R')


    for k, (ox, oy) in enumerate(((-6, -6), (1, 7), (-9, 2))[:a.get('heads', 1) - 1]):
        back = dict(a)
        back['col'] = a['dark'] if a['dark'] != a['col'] else a['belly']
        if not low:
            c.line(bx + rx * .6, by - 1, hx + ox - 1, hy + oy + 1, a.get('neckw', 5) * .8, back['col'])
        head(hx + ox, hy + oy, back)
    head(hx, hy, a)

def bird(c, a, pose):
    col, dark, belly = a['col'], a['dark'], a['belly']
    dy = pose.get('dy', 0)
    low = pose.get('low')
    lunge = pose.get('lunge', 0)
    leglen = 0 if low else a.get('leg', 7)
    foot = 38
    by = foot - leglen - 7 + dy
    bx = 18 + lunge
    rx, ry = a.get('rx', 9), a.get('ry', 7)
    hx, hy = bx + rx - 2 + a.get('neckx', 1), by - ry - a.get('neck', 1) + pose.get('head', 0) - 1
    if low:
        hy = by - 3
    hr = a.get('head', 4.5)
    tl = a.get('tail', 8)
    c.line(bx - rx + 1, by, bx - rx - tl * .7, by + a.get('taildrop', 2), a.get('tailw', 3.5), a['mark'])
    if not low:
        ph = pose.get('leg', 0)
        off = {0: 1, 1: 0, 2: -1, 3: 0}[ph]
        c.rect(bx - 2 + off, by + ry - 1, bx - 1 + off, foot + dy, a['leg_col'])
        c.rect(bx + 2 - off, by + ry - 1, bx + 3 - off, foot + dy, a['leg_col'])
        c.rect(bx - 3 + off, foot + dy - 1, bx + 1 + off, foot + dy, a['leg_col'])
        c.rect(bx + 1 - off, foot + dy - 1, bx + 5 - off, foot + dy, a['leg_col'])
    c.oval(bx, by, rx, ry, col)
    c.oval(bx + 2, by + ry * .4, rx * .7, ry * .55, belly)
    flap = pose.get('leg', 0) in (1, 3) or pose.get('lunge', 0) > 0
    if flap and not low:
        c.oval(bx - 3, by - ry - 1, 6, 4, a['mark'])
    else:
        c.oval(bx - 2, by, rx * .62, ry * .6, a['mark'])
    if a.get('neck', 1) > 2 and not low:
        c.line(bx + rx * .55, by - ry * .3, hx - 1, hy + 1, a.get('neckw', 2.6), col)
    if a.get('crest'):
        c.line(hx - 2, hy - hr + 1, hx - 5, hy - hr - 3, 1.6, a['mark'])
    c.oval(hx, hy, hr, hr, a.get('headcol', col))
    if a.get('comb'):
        for k in (-1.5, 0, 1.5):
            c.oval(hx + k, hy - hr - .5 + abs(k) * .3, 1.3, 1.6, 'r')
        c.oval(hx + hr - 1, hy + 2.6, 1, 1.5, 'r')
    b = a.get('beak', 'short')
    bc = a.get('beakcol', 'f')
    if b == 'short':
        c.line(hx + hr - 1, hy + .5, hx + hr + 2, hy + 1, 1.8, bc)
    elif b == 'long':
        c.line(hx + hr - 1, hy + 1, hx + hr + 7, hy + 2, 1.5, bc)
    elif b == 'hook':
        c.line(hx + hr - 1, hy, hx + hr + 2, hy, 2.2, bc)
        c.line(hx + hr + 2, hy, hx + hr + 2, hy + 2.5, 1.3, bc)
    elif b == 'huge':
        c.oval(hx + hr + 2, hy + 1, 4.5, 2.8, bc)
    eye(c, hx + 1, hy - 1, pose)
    if pose.get('mouth'):
        c.dot(hx + hr + 1, hy + 2, 'R')


def primate(c, a, pose):
    col, dark, belly = a['col'], a['dark'], a['belly']
    dy, lunge = pose.get('dy', 0), pose.get('lunge', 0)
    low = pose.get('low')
    bx, by = 19 + lunge, (31 if not low else 33) + dy
    if a.get('tail', 'thin') == 'thin':
        c.line(bx - 7, by + 3, bx - 13, by - 5 + pose.get('leg', 0) % 2, 1.5, dark)
    ph = pose.get('leg', 0)
    if not low:
        c.rect(bx - 6, by + 4, bx - 3, 38 + dy - (ph == 0), dark)
        c.rect(bx + 3, by + 4, bx + 6, 38 + dy - (ph == 2), dark)
    bs, hs = a.get('bs', 1.0), a.get('hs', 1.0)     # body and head scale (young ones: small body, big head)
    c.oval(bx, by, 8 * bs, 7.5 * bs, col)
    if a.get('shag'):
        for i in range(-3, 4):
            c.line(bx + i * 2.3 * bs, by + 4 * bs, bx + i * 2.3 * bs + .6, by + 8.5 * bs, 1.4, a['mark'])
    c.oval(bx, by + 1, 5 * bs, 5 * bs, belly)
    c.line(bx - 7 * bs, by - 3, bx - 9 * bs, by + 5 - (ph == 1) * 2, 2.6, col)
    c.line(bx + 7 * bs, by - 3, bx + 9 * bs, by + 5 - (ph == 3) * 2, 2.6, col)
    hx, hy = bx + 1, by - 4 - 7 * bs * hs + pose.get('head', 0)
    er = a.get('earr', 2.4)
    if a.get('shag'):
        c.oval(hx, hy + 2, 7.6 * hs, 7.4 * hs, a['mark'])
    c.oval(hx - 6 * hs, hy, er * hs, er * hs, belly)
    c.oval(hx + 6 * hs, hy, er * hs, er * hs, belly)
    c.oval(hx, hy, 6.4 * hs, 6 * hs, col)
    c.oval(hx + 1, hy + 2, 4 * hs, 3.2 * hs, a.get('face', belly))
    if a.get('nose'):
        c.line(hx + 2, hy + 1, hx + 2 + a['nose'], hy + 2.5, 2.2, a.get('nosecol', col))
    if a.get('mark') != col:
        c.oval(hx, hy - 4, 4, 1.6, a['mark'])
    eye(c, hx - 1, hy - 1, pose)
    eye(c, hx + 3, hy - 1, pose)
    if pose.get('mouth'):
        c.rect(hx, hy + 3, hx + 3, hy + 4, 'R')


def snake(c, a, pose):
    col, dark, belly = a['col'], a['dark'], a['belly']
    dy, lunge = pose.get('dy', 0), pose.get('lunge', 0)
    ph = pose.get('leg', 0)
    wob = {0: 0, 1: 1, 2: 0, 3: -1}[ph]
    c.oval(17, 33 + dy, 12, 4.5, col)
    c.oval(19 + wob, 29 + dy, 9, 4, col)
    for i in range(-4, 5):
        c.oval(17 + i * 2.6, 33 + dy, .9, .9, a['mark'])
    for i in range(-3, 4):
        c.oval(19 + wob + i * 2.4, 29 + dy, .8, .8, a['mark'])
    hy = (pose.get('low') and 27 or 15) + dy + pose.get('head', 0)
    hx = 25 + lunge + wob
    nh = a.get('heads', 1)
    for k in range(nh - 1, 0, -1):             # furthest first
        kx = hx - 6 * k
        ky = hy + (5 if k % 2 else -2) + (2 if pose.get('low') else 0)
        tone = dark if k % 2 else col
        if not pose.get('low'):
            c.line(19 - k * 2, 29 + dy, kx, ky + 2, 3, tone)
        c.oval(kx + 1, ky, 3.8, 3.1, tone)
        eye(c, kx + 1, ky - 1, pose)
    if not pose.get('low'):
        c.line(25, 27 + dy, hx, hy + 2, 4, col)
    if a.get('feathers'):                      # the quetzal ruff of a feathered serpent
        for k, fc in enumerate(('r', 'T', 'g', 'f', 'T')):
            c.line(hx - 1, hy + 2, hx - 7 + k * .6, hy - 3 + k * 2.4, 1.8, fc)
    if a.get('hood'):
        c.oval(hx, hy + 3, 5, 4, dark)
    c.oval(hx + 1, hy, 4.4, 3.4, col)
    if a.get('feathers'):
        for k in range(3):
            c.line(hx - 1, hy - 2, hx - 3 + k * 1.5, hy - 7 + k * .5, 1.3, ('g', 'T', 'r')[k])
    eye(c, hx + 1, hy - 1, pose)
    if pose.get('mouth') or lunge:
        c.line(hx + 5, hy + 1, hx + 8, hy + 2, .9, 'r')


def low_reptile(c, a, pose):
    col, dark, belly = a['col'], a['dark'], a['belly']
    dy, lunge = pose.get('dy', 0), pose.get('lunge', 0)
    by = 31 + dy
    bx = 17 + lunge
    long = a.get('long', 1.0)
    c.line(bx - 8 * long, by, bx - 17 * long, by + 3, 3, col)
    if not pose.get('low'):
        ph = pose.get('leg', 0)
        o = {0: 1, 1: 0, 2: -1, 3: 0}[ph]
        for i, x in enumerate((bx - 7, bx + 5)):
            s = o if i else -o
            c.rect(x + s, by + 2, x + s + 3, 38 + dy, dark)
    c.oval(bx, by, 10 * long, 5, col)
    c.oval(bx + 1, by + 3, 8 * long, 2, belly)
    for i in range(-3, 4):
        c.oval(bx + i * 2.6, by - 3, 1, 1, a['mark'])
    hx, hy = bx + 11 * long, by - 2 + pose.get('head', 0)
    jaw = a.get('jaw', 5)
    hs = a.get('hs', 1.0)
    c.oval(hx, hy, 4.5 * hs, 3.5 * hs, col)
    c.rect(hx + 2, hy - 1, hx + 2 + jaw, hy + 2, col)
    if pose.get('mouth') or lunge:
        c.rect(hx + 2, hy + 2, hx + 2 + jaw, hy + 3, 'R')
        c.rect(hx + 2, hy + 3, hx + 2 + jaw, hy + 4, col)
    if a.get('frill'):
        c.oval(hx - 3, hy, 3, 5, a['mark'])
    eye(c, hx, hy - 2, pose)


def turtle(c, a, pose):
    col, dark = a['col'], a['dark']
    dy, lunge = pose.get('dy', 0), pose.get('lunge', 0)
    ph = pose.get('leg', 0)
    bx, by = 18 + lunge, 29 + dy
    if not pose.get('low'):
        o = {0: 1, 1: 0, 2: -1, 3: 0}[ph]
        c.rect(bx - 8 - o, by + 4, bx - 5 - o, 37 + dy, a['belly'])
        c.rect(bx + 5 + o, by + 4, bx + 8 + o, 37 + dy, a['belly'])
    hx, hy = bx + 13, by + (3 if pose.get('low') else 0) + pose.get('head', 0)
    c.line(bx + 8, by + 3, hx - 1, hy + 1, 3.2, a['belly'])
    c.oval(hx, hy, 3.6, 3.2, a['belly'])
    c.oval(bx, by, 11, 7.4, col)
    c.rect(bx - 11, by + 4, bx + 12, by + 7, dark)
    for sx in (-6, 0, 6):
        c.oval(bx + sx, by - 2, 2.5, 2, a['mark'])
    eye(c, hx + 1, hy - 1, pose)


def frog(c, a, pose):
    col, belly = a['col'], a['belly']
    dy, lunge = pose.get('dy', 0), pose.get('lunge', 0)
    bx, by = 20 + lunge, 30 + dy
    c.oval(bx - 9, by + 5, 5, 3, col)
    c.oval(bx + 9, by + 5, 5, 3, col)
    c.oval(bx, by, 10, 8, col)
    c.oval(bx, by + 3, 7, 4.5, belly)
    if a.get('pattern') == 'spots':
        for sx, sy in ((-5, -4), (4, -5), (-7, 0), (7, -1)):
            c.oval(bx + sx, by + sy, 1.5, 1.2, a['mark'])
    c.oval(bx - 5, by - 8 + pose.get('head', 0), 3.6, 3.6, col)
    c.oval(bx + 5, by - 8 + pose.get('head', 0), 3.6, 3.6, col)
    eye(c, bx - 5, by - 8 + pose.get('head', 0), pose)
    eye(c, bx + 5, by - 8 + pose.get('head', 0), pose)
    c.rect(bx - 4, by - 1, bx + 5, by, 'R' if pose.get('mouth') else a['dark'])


def fish(c, a, pose):
    col, belly = a['col'], a['belly']
    dy, lunge = pose.get('dy', 0), pose.get('lunge', 0)
    w = {0: 0, 1: 1, 2: 0, 3: -1}[pose.get('leg', 0)]
    bx, by = 20 + lunge, 24 + dy + (6 if pose.get('low') else 0)
    rx, ry = a.get('rx', 12), a.get('ry', 7)
    c.line(bx - rx, by, bx - rx - 6, by - 5 + w, 2.4, a['mark'])
    c.line(bx - rx, by, bx - rx - 6, by + 5 + w, 2.4, a['mark'])
    c.oval(bx, by, rx, ry, col)
    c.oval(bx + 1, by + ry * .45, rx * .8, ry * .45, belly)
    c.line(bx - 2, by - ry + 1, bx - 5, by - ry - 3 + a.get('finup', 0), a.get('finw', 2.5), a['mark'])
    if a.get('snout'):
        c.line(bx + rx - 1, by + 1, bx + rx + a['snout'], by + 1.5, 2.2, col)
    eye(c, bx + rx - 5, by - 2, pose)
    if pose.get('mouth') or lunge:
        c.rect(bx + rx - 4, by + 2, bx + rx, by + 3, 'R')


def scorpion(c, a, pose):
    col, dark = a['col'], a['dark']
    dy, lunge = pose.get('dy', 0), pose.get('lunge', 0)
    ph = pose.get('leg', 0) % 2
    bx, by = 18 + lunge, 31 + dy
    for i, x in enumerate((-7, -3, 1, 5)):
        c.line(bx + x, by + 2, bx + x - 2 + (i + ph) % 2, 38 + dy, 1.2, dark)
    c.oval(bx, by, 10, 4.5, col)
    for i in range(3):
        c.oval(bx - 10 - i * 2, by - 3 - i * 4, 2.2, 2.2, col)
    c.oval(bx - 13, by - 18, 2.2, 2.2, 'r')
    c.line(bx + 9, by, bx + 15 + lunge, by - 3, 2, col)
    c.oval(bx + 16 + lunge, by - 4, 3.2, 2.4, dark)
    eye(c, bx + 6, by - 2, pose)


def bat(c, a, pose):
    """Wings spread wide, two ears, front-facing; folded up when it sleeps."""
    col, dark, belly = a['col'], a['dark'], a['belly']
    dy, lunge = pose.get('dy', 0), pose.get('lunge', 0)
    low = pose.get('low')
    up = pose.get('leg', 0) in (1, 3) or dy < 0 or lunge > 0
    bx, by = 20 + lunge, 24 + dy + (6 if low else 0)
    if not low:
        f = -4 if up else 2
        for s in (-1, 1):
            tip = (bx + s * 15, by - 6 + f)
            mid = (bx + s * 9, by + 4 + f / 2)
            c.oval(bx + s * 8, by + f / 2, 7, 4, a['mark'])
            c.line(bx + s * 3, by - 2, tip[0], tip[1], 1.4, dark)
            c.line(tip[0], tip[1], mid[0], mid[1], 1.0, dark)
            c.line(bx + s * 3, by - 2, mid[0] - s * 3, mid[1] + 1, 1.0, dark)
    else:
        c.oval(bx, by + 1, 7, 7.5, a['mark'])
    c.oval(bx, by, 5, 6, col)
    c.oval(bx, by + 2, 3.2, 3.6, belly)
    c.line(bx - 2, by + 5, bx - 2, by + 9, 1, dark)
    c.line(bx + 2, by + 5, bx + 2, by + 9, 1, dark)
    hx, hy = bx, by - 7 + pose.get('head', 0)
    c.line(hx - 3, hy - 1, hx - 4, hy - 6, 1.8, col)
    c.line(hx + 3, hy - 1, hx + 4, hy - 6, 1.8, col)
    c.oval(hx, hy, 4.2, 3.6, col)
    c.oval(hx, hy + 2, 1.6, 1.1, belly)
    eye(c, hx - 2, hy - 1, pose)
    eye(c, hx + 2, hy - 1, pose)
    if pose.get('mouth') or lunge > 0:
        c.rect(hx - 1, hy + 2, hx + 2, hy + 3, 'R')
        c.dot(hx - 1, hy + 3, 'w')
        c.dot(hx + 1, hy + 3, 'w')


def spider(c, a, pose):
    """Eight legs, a round body; a woven pattern for the weaving spider."""
    col, dark = a['col'], a['dark']
    dy, lunge = pose.get('dy', 0), pose.get('lunge', 0)
    ph = pose.get('leg', 0) % 2
    bx, by = 17 + lunge, 29 + dy + (3 if pose.get('low') else 0)
    for side in (0, 1):
        for i, (kx, fx) in enumerate(((-4, -12), (-1, -7), (3, 9), (6, 14))):
            lift = 1 if (i + side + ph) % 2 else 0
            knee = (bx + kx + side * 2, by - 6 - lift)
            foot = (bx + fx + (1 if side else -1), 38 + dy - lift)
            tone = dark if side == 0 else a.get('legcol', dark)
            c.line(bx + 2, by, knee[0], knee[1], 1.3, tone)
            c.line(knee[0], knee[1], foot[0], foot[1], 1.1, tone)
    c.oval(bx - 5, by - 1, 7.5, 6.5, col)
    weave = a.get('weave')
    if weave:
        for row in range(-2, 3):
            for k in range(-2, 2):
                c.oval(bx - 5 + k * 3 + (row % 2) * 1.5, by - 1 + row * 2.2, .9, .9, weave[(row + k) % len(weave)])
    c.oval(bx + 4, by + 1, 4.5, 4, col)
    eye(c, bx + 6, by, pose)
    c.dot(bx + 4, by - 2, 'k')
    c.dot(bx + 7, by - 2, 'k')
    c.line(bx + 8, by + 2, bx + 9, by + 4, 1, dark)
    if pose.get('mouth') or lunge > 0:
        c.dot(bx + 8, by + 3, 'R')


def octopus(c, a, pose):
    """A big round head and six curling arms."""
    col, belly = a['col'], a['belly']
    dy, lunge = pose.get('dy', 0), pose.get('lunge', 0)
    w = {0: 0, 1: 1, 2: 0, 3: -1}[pose.get('leg', 0)]
    bx, by = 19 + lunge, 22 + dy + (5 if pose.get('low') else 0)
    for i, k in enumerate((-8, -5, -2, 2, 5, 8)):
        x0 = bx + k * .7
        sw = w if i % 2 else -w
        ex, ey = x0 + k * .5 + sw, by + 11
        c.line(x0, by + 4, ex, ey, 2.4, col)
        c.line(ex, ey, ex + (2 if k > 0 else -2), 38 + dy, 1.7, col)
        c.dot(ex, ey - 2, belly)
    c.oval(bx - 1, by - 4, 9, 9.5, col)
    for sx, sy in ((-5, -8), (-1, -10), (3, -7), (-6, -3)):
        c.oval(bx + sx, by + sy, 1.3, 1.1, a['mark'])
    c.oval(bx, by + 3, 7, 3.6, col)
    eye(c, bx - 3, by, pose, big=True)
    eye(c, bx + 3, by, pose, big=True)
    if pose.get('mouth') or lunge > 0:
        c.oval(bx, by + 4, 1.4, 1, 'R')


def plesio(c, a, pose):
    """A lake monster: a round body with flippers and a long neck held high."""
    col, dark, belly = a['col'], a['dark'], a['belly']
    dy, lunge = pose.get('dy', 0), pose.get('lunge', 0)
    low = pose.get('low')
    f = {0: 1, 1: 0, 2: -1, 3: 0}[pose.get('leg', 0)]
    bx, by = 15 + lunge, 31 + dy
    c.line(bx - 9, by, bx - 15, by + 2, 2.4, col)
    c.oval(bx + 5 + f, by + 5, 4, 1.6, dark)
    c.oval(bx - 6 - f, by + 5, 3.6, 1.5, dark)
    c.oval(bx, by, 10, 5.5, col)
    c.oval(bx + 1, by + 3, 8, 2, belly)
    for sx in (-5, -1, 3):
        c.oval(bx + sx, by - 3, 1.4, 1.1, a['mark'])
    c.oval(bx + 6 - f, by + 5.5, 4.2, 1.8, col)
    c.oval(bx - 5 + f, by + 5.5, 3.8, 1.7, col)
    if low:
        hx, hy, mid = bx + 13, by - 1, (bx + 10, by - 2)
    else:
        hx, hy, mid = bx + 16, by - 17 + pose.get('head', 0), (bx + 12, by - 7)
    c.line(bx + 7, by - 2, mid[0], mid[1], 3, col)
    c.line(mid[0], mid[1], hx - 1, hy + 1, 2.6, col)
    c.oval(hx, hy, 3.6, 2.8, col)
    c.rect(hx + 2, hy, hx + 5, hy + 1.5, col)
    eye(c, hx, hy - 1, pose)
    if pose.get('mouth') or lunge > 0:
        c.rect(hx + 2, hy + 1.5, hx + 5, hy + 2.2, 'R')


def spirit(c, a, pose):
    """A floating figure in a long robe, with long hair, a hat or a sack as its legend has it."""
    col = a['col']
    dy, lunge = pose.get('dy', 0), pose.get('lunge', 0)
    low = pose.get('low')
    w = {0: 0, 1: 1, 2: 0, 3: -1}[pose.get('leg', 0)]
    bx, by = 19 + lunge, (25 if not low else 29) + dy
    hx, hy = bx + 1, by - 9 + pose.get('head', 0)
    hair = a.get('hair')
    if hair:
        c.oval(hx - 1, hy + 4, 6.5, 8.5, hair)
    if a.get('sack'):
        c.line(bx - 4, by - 3, bx - 7, by - 1, 1, 'C')
        c.oval(bx - 8, by + 3, 4, 4.5, 'c')
    c.oval(bx, by + 1, 7, 8, col)
    for i, k in enumerate((-5, 0, 5)):
        c.oval(bx + k + (w if i % 2 else -w), by + 9, 2.6, 2.4, col)
    c.line(bx - 5, by - 3, bx - 8, by + 3 - w, 2, col)
    c.line(bx + 5, by - 3, bx + 8, by + 3 + w, 2, col)
    c.oval(hx, hy, 5, 5, a.get('face', col))
    if hair:
        c.oval(hx, hy - 4, 5, 2, hair)
    eye(c, hx - 2, hy, pose)
    eye(c, hx + 2, hy, pose)
    if pose.get('mouth') or lunge > 0:
        c.oval(hx, hy + 3, 1.3, 1.1, 'k')


def wisp(c, a, pose):
    """Three flames that move together, with a face in the middle one."""
    dy, lunge = pose.get('dy', 0), pose.get('lunge', 0)
    fl = {0: 0, 1: 1, 2: 0, 3: -1}[pose.get('leg', 0)]
    bx, by = 19 + lunge, 28 + dy + (3 if pose.get('low') else 0)
    for i, k in enumerate((-7, 7, 0)):
        big = k == 0
        r = 4.5 if big else 3.5
        x, y = bx + k, by + (0 if big else 3)
        sw = fl if i % 2 else -fl
        tip = y - (13 if big else 9) + sw
        c.line(x, y - 1, x + sw, tip, 3 if big else 2.4, a['mark'])
        c.oval(x, y, r, r, a['col'])
        c.line(x, y - 1, x, tip + 3, 1.6 if big else 1.2, a['belly'])
        c.oval(x, y + 1, r * .6, r * .55, a['belly'])
    eye(c, bx - 1.5, by, pose)
    eye(c, bx + 1.5, by, pose)
    if pose.get('mouth') or lunge > 0:
        c.rect(bx - 1, by + 2.5, bx + 1, by + 3.2, 'R')


def surface(c, top=True):
    """Topmost (or bottom-most) painted pixel of each column, as (x, y)."""
    out = []
    for x in range(c.n):
        ys = [y for y in range(c.n) if c.g[y][x] is not None]
        if ys:
            out.append((x, min(ys) if top else max(ys)))
    return out


def decorate(c, a, pose, seed):
    """What a legend wears or gives off: a hat, a headdress, plants, flames, glowing hooves, confetti."""
    eyes = getattr(c, 'eyes', [])
    ex, ey = eyes[-1] if eyes else (24, 14)
    hat = a.get('hat')
    if hat:
        x, y = ex + a.get('hatx', -1), ey + a.get('haty', -4)
        if hat == 'vueltiao':                  # the sombrero vueltiao: cream with black bands
            c.rect(x - 6, y, x + 6, y + 1, 'y')
            c.rect(x - 3, y - 3, x + 3, y, 'y')
            c.line(x - 3, y - 1.6, x + 3, y - 1.6, .7, 'k')
            c.line(x - 6, y + .6, x + 6, y + .6, .5, 'k')
        elif hat == 'wide':                    # a huge black hat
            c.rect(x - 8, y, x + 8, y + 1.4, 'k')
            c.oval(x, y - 2, 4.5, 3, 'k')
            c.line(x - 4, y - 1, x + 4, y - 1, .8, 'S')
        elif hat == 'straw':
            c.rect(x - 5, y, x + 5, y + 1, 'Y')
            c.oval(x, y - 1.5, 3, 2.2, 'Y')
            c.line(x - 3, y - .6, x + 3, y - .6, .7, 'C')
        elif hat == 'fedora':
            c.rect(x - 5, y, x + 5, y + 1, 'w')
            c.oval(x, y - 2, 3.2, 2.6, 'w')
            c.line(x - 3, y - 1, x + 3, y - 1, .8, 'k')
    top = (ex + a.get('hatx', -1), ey + a.get('haty', -4))
    if a.get('helmet'):                        # a hard hat or a motorbike helmet
        hc = a['helmet']
        c.oval(top[0], top[1] + 1, 5, 3.4, hc)
        c.line(top[0] - 5, top[1] + 2.6, top[0] + 5.5, top[1] + 2.6, 1.1, SHADE.get(hc, 'k'))
    if a.get('crown'):                         # a beauty queen's crown
        x, y = top
        c.rect(x - 3, y, x + 3, y + 1, 'f')
        for k in (-3, 0, 3):
            c.line(x + k, y, x + k * .8, y - 2.5, .9, 'f')
        c.dot(x, y, 'r')
    if a.get('halo'):                          # a golden halo floating over the head
        x, y = top
        c.line(x - 3.5, y - 2, x + 3.5, y - 2, 1.1, 'f')
    glasses = a.get('glasses')
    if glasses:                                # round glasses or dark sunglasses, over every eye
        for x, y in eyes:
            if glasses == 'sun':
                c.rect(x - 1.5, y - 1.5, x + 2.5, y + 1.8, 'k')
                c.dot(x - 1, y - 1, 'w')
            else:
                for (x0, y0, x1, y1) in ((x - 2, y - 2, x + 2, y - 2), (x - 2, y + 2, x + 2, y + 2), (x - 2, y - 2, x - 2, y + 2), (x + 2, y - 2, x + 2, y + 2)):
                    c.line(x0, y0, x1, y1, .6, 'k')
        if len(eyes) >= 2:
            (x0, y0), (x1, _) = eyes[-2], eyes[-1]
            c.line(x0 + 2, y0 - 1, x1 - 2, y0 - 1, .6, 'k')
    if a.get('mask'):                          # a face mask -- worn over the eyes
        xs = [x for x, _ in eyes] or [ex]
        y = ey
        c.rect(min(xs) - 2.5, y - 1.6, max(xs) + 3, y + 1.8, a['mask'])
        c.line(min(xs) - 2.5, y, min(xs) - 5, y - 2, .6, 'w')
    if a.get('tie'):                           # a necktie
        c.line(ex - 1, ey + 5, ex - 1.4, ey + 10, 1.5, a['tie'])
        c.dot(ex - 1, ey + 5, SHADE.get(a['tie'], 'k'))
    if a.get('mic'):                           # a singer's microphone
        c.line(ex + 4, ey + 4, ex + 6, ey + 8, 1, 'S')
        c.oval(ex + 3.6, ey + 3.4, 1.2, 1.2, 'k')
    if a.get('headdress'):                     # a striped royal headcloth
        c.line(ex - 5, ey - 4, ex - 6, ey + 6, 2.4, 'f')
        c.line(ex - 5, ey - 4, ex + 3, ey - 4, 1.4, 'f')
        for i in range(3):
            c.line(ex - 7, ey - 2 + i * 3, ex - 4, ey - 2 + i * 3, .8, 'N')
        c.line(ex - 4, ey - 4, ex + 3, ey - 4, .6, 'N')
    if a.get('snakebeak'):                     # an eagle holding a serpent
        c.line(ex + 5, ey + 3, ex + 7, ey + 8, 1.1, 'g')
        c.dot(ex + 7, ey + 9, 'G')
    eg = int(ex * c.f) + c.ox
    r = seed
    flick = pose.get('leg', 0) + pose.get('dy', 0) * 3

    def rnd():
        nonlocal r
        r = (r * 1103515245 + 12345) & 0x7fffffff
        return r
    if a.get('plants'):                        # moss, leaves and flowers growing on its back
        for x, y in surface(c):
            if abs(x - eg) <= 2 or rnd() % 3:
                continue
            flower = rnd() % (2 if a.get('flowers') else 4) == 0
            for k, cc in enumerate(('g', 'p' if rnd() % 2 else 'f') if flower else ('g', 'G')):
                if y - 1 - k >= 0:
                    c.g[y - 1 - k][x] = cc
    if a.get('flames'):                        # flames flickering along its back
        for x, y in surface(c):
            if abs(x - eg) <= 1:
                continue
            h = 1 + (x * 7 + flick + seed) % 3
            for k in range(h):
                if y - 1 - k >= 0:
                    c.g[y - 1 - k][x] = ('f', 'o', 'r')[min(k, 2)]
    if a.get('glowfeet'):                      # hooves that strike sparks
        bottom = surface(c, top=False)
        low = max(y for _, y in bottom)
        for x, y in bottom:
            if y >= low - 1:
                c.g[y][x] = 'f' if (x + flick) % 2 else 'o'
    conf = a.get('confetti')
    if conf:                                   # carnival ribbons and confetti
        pts = [(x, y) for y, row in enumerate(c.g) for x, v in enumerate(row) if v not in (None, 'w') and abs(x - eg) > 2]
        for _ in range(max(6, len(pts) // 25)):
            x, y = pts[rnd() % len(pts)]
            c.g[y][x] = conf[rnd() % len(conf)]


# name fragments -> (base, dark, belly, mark, pattern)
LOOK = [
    ('ZEBRA', dict(col='w', dark='s', belly='w', mark='k', pattern='stripes')),
    ('LION', dict(col='Y', dark='c', belly='y', mark='C', mane=True, tail='tuft')),
    ('TIGER', dict(col='o', dark='O', belly='w', mark='k', pattern='stripes')),
    ('CHEETAH', dict(col='y', dark='Y', belly='w', mark='k', pattern='spots', long=1.1, leg=10)),
    ('SNOW LEOPARD', dict(col='s', dark='S', belly='w', mark='k', pattern='spots', tail='bushy')),
    ('LEOPARD', dict(col='Y', dark='c', belly='y', mark='C', pattern='spots')),
    ('JAGUAR', dict(col='f', dark='Y', belly='y', mark='k', pattern='spots', body=1.1)),
    ('SERVAL', dict(col='y', dark='Y', belly='w', mark='k', pattern='spots', ear='point', leg=10)),
    ('OCELOT', dict(col='Y', dark='c', belly='y', mark='k', pattern='patches')),
    ('MARGAY', dict(col='c', dark='C', belly='y', mark='k', pattern='spots')),
    ('CARACAL', dict(col='Y', dark='c', belly='y', mark='k', ear='point')),
    ('GENET', dict(col='s', dark='S', belly='w', mark='k', pattern='spots', long=1.15, leg=5)),
    ('CIVET', dict(col='S', dark='k', belly='s', mark='k', pattern='spots', long=1.1, leg=5)),
    ('PUMA', dict(col='c', dark='C', belly='y', mark='C')),
    ('SABER', dict(col='Y', dark='c', belly='y', mark='C', tusk=True)), ('SMILODON', dict(col='c', dark='C', belly='y', mark='C', tusk=True)),
    ('MACHAIRODUS', dict(col='Y', dark='c', belly='y', mark='C', tusk=True)), ('THYLACOSMILUS', dict(col='S', dark='k', belly='s', mark='C', tusk=True)),
    ('HYENA', dict(col='s', dark='S', belly='y', mark='C', pattern='spots', ear='round')),
    ('PAINTED', dict(col='Y', dark='C', belly='w', mark='k', pattern='patches', ear='big')), ('CAPE DOG', dict(col='c', dark='C', belly='w', mark='k', pattern='patches', ear='big')),
    ('FENNEC', dict(col='y', dark='Y', belly='w', mark='y', ear='big', tail='bushy', body=.8, leg=6)),
    ('MANED', dict(col='o', dark='O', belly='y', mark='k', leg=13, ear='big')), ('DHOLE', dict(col='O', dark='R', belly='y', mark='C', tail='bushy')),
    ('TANUKI', dict(col='c', dark='C', belly='y', mark='k', tail='bushy', leg=6)), ('BUSH DOG', dict(col='C', dark='S', belly='c', mark='k', leg=5)),
    ('GIRAFFE', dict(col='f', dark='Y', belly='y', mark='C', pattern='patches')),
    ('ASIAN ELEPHANT', dict(col='S', dark='k', belly='s', mark='S')), ('ELEPHANT', dict(col='s', dark='S', belly='s', mark='s')),
    ('PANDA', dict(col='w', dark='k', belly='w', mark='k', pattern='patches')), ('RED PANDA', dict(col='O', dark='R', belly='k', mark='w', tail='bushy')),
    ('SUN BEAR', dict(col='k', dark='k', belly='f', mark='k')), ('SLOTH BEAR', dict(col='k', dark='k', belly='w', mark='k')), ('SPECTACLED', dict(col='C', dark='S', belly='y', mark='k')),
    ('ORANGUTAN', dict(col='O', dark='R', belly='o', mark='O', tail='none')), ('CHIMP', dict(col='C', dark='k', belly='y', mark='C', tail='none')),
    ('MANDRILL', dict(col='S', dark='k', belly='r', mark='B', face='r')), ('GELADA', dict(col='c', dark='C', belly='r', mark='C')),
    ('LEMUR', dict(col='s', dark='S', belly='w', mark='k')), ('HOWLER', dict(col='k', dark='k', belly='C', mark='k')), ('COLOBUS', dict(col='k', dark='k', belly='w', mark='w')),
    ('TAPIR', dict(col='k', dark='k', belly='w', mark='k', trunk=False)), ('WARTHOG', dict(col='S', dark='k', belly='s', mark='k', tusk=True)), ('WILD BOAR', dict(col='C', dark='S', belly='c', mark='k', tusk=True)),
    ('CAPYBARA', dict(col='c', dark='C', belly='y', mark='c', tail='none', leg=5, snout=3)), ('MEERKAT', dict(col='y', dark='Y', belly='w', mark='k', leg=6)),
    ('HONEY BADGER', dict(col='k', dark='k', belly='k', mark='w')), ('RATEL', dict(col='S', dark='k', belly='k', mark='w')), ('PORCUPINE', dict(col='S', dark='k', belly='s', mark='w', pattern='stripes')),
    ('FLAMINGO', dict(col='p', dark='P', belly='p', mark='P', beakcol='k')), ('SCARLET IBIS', dict(col='r', dark='R', belly='r', mark='R', beakcol='y')), ('ROSE SPOON', dict(col='p', dark='P', belly='w', mark='r', beakcol='y')),
    ('SECRETARY', dict(col='s', dark='S', belly='w', mark='k', crest=True, beakcol='y')), ('SHOEBILL', dict(col='S', dark='k', belly='s', mark='k', beakcol='Y')), ('RED CRANE', dict(col='w', dark='s', belly='w', mark='k', crest=True, beakcol='k')),
    ('OSTRICH', dict(col='k', dark='S', belly='w', mark='w', beakcol='p', headcol='p')), ('CASSOWARY', dict(col='k', dark='S', belly='k', mark='B')),
    ('PENGUIN', dict(col='k', dark='k', belly='w', mark='k', beakcol='f')),
    ('SCARLET', dict(col='r', dark='R', belly='f', mark='B', beakcol='w')), ('HYACINTH', dict(col='N', dark='M', belly='B', mark='M', beakcol='k')), ('MACAW', dict(col='B', dark='N', belly='f', mark='r', beakcol='k')),
    ('TOUCAN', dict(col='k', dark='k', belly='w', mark='k', beakcol='o')), ('HORNBILL', dict(col='k', dark='k', belly='w', mark='w', beakcol='f')), ('PARROT', dict(col='g', dark='G', belly='f', mark='r', beakcol='k')),
    ('CONURE', dict(col='f', dark='Y', belly='o', mark='g', beakcol='k')), ('PARAKEET', dict(col='g', dark='G', belly='f', mark='B', beakcol='r')),
    ('PEACOCK', dict(col='N', dark='M', belly='B', mark='T', tail=14, crest=True, beakcol='y')), ('GOLDEN PHEASANT', dict(col='r', dark='R', belly='f', mark='f', tail=13, beakcol='y', headcol='f')), ('MANDARIN', dict(col='o', dark='O', belly='w', mark='B', beakcol='r')),
    ('HUMMINGBIRD', dict(col='t', dark='T', belly='w', mark='p', beakcol='k')), ('CANARY', dict(col='f', dark='Y', belly='f', mark='Y', beakcol='o')), ('SUNBIRD', dict(col='B', dark='N', belly='f', mark='g', beakcol='k')),
    ('KINGFISHER', dict(col='B', dark='N', belly='o', mark='b', beakcol='k')), ('TROGON', dict(col='g', dark='G', belly='r', mark='w', beakcol='y')), ('COTINGA', dict(col='b', dark='B', belly='P', mark='N', beakcol='k')),
    ('TANAGER', dict(col='r', dark='R', belly='r', mark='k', beakcol='s')), ('MANAKIN', dict(col='k', dark='k', belly='k', mark='k', headcol='r', beakcol='s')),
    ('OWL', dict(col='C', dark='S', belly='y', mark='c', beakcol='Y', crest=True)), ('CONDOR', dict(col='k', dark='k', belly='S', mark='w', headcol='p', beakcol='y')), ('VULTURE', dict(col='C', dark='S', belly='c', mark='S', headcol='s', beakcol='s')),
    ('COBRA', dict(col='Y', dark='c', belly='y', mark='C', hood=True)), ('MAMBA', dict(col='g', dark='G', belly='t', mark='G')), ('BLACK', dict(col='S', dark='k', belly='s', mark='k')), ('CORAL', dict(col='r', dark='R', belly='f', mark='k')),
    ('GREEN', dict(col='g', dark='G', belly='f', mark='f')), ('EMERALD', dict(col='g', dark='G', belly='f', mark='w')), ('YELLOW', dict(col='f', dark='Y', belly='y', mark='k')), ('ANACONDA', dict(col='d', dark='M', belly='t', mark='k')),
    ('PYTHON', dict(col='c', dark='C', belly='y', mark='k')), ('BOA', dict(col='y', dark='c', belly='y', mark='C')), ('DART FROG', dict(col='B', dark='N', belly='b', mark='k', pattern='spots')),
    ('TREE FROG', dict(col='g', dark='G', belly='w', mark='r')), ('SALAMANDER', dict(col='C', dark='k', belly='c', mark='S', pattern='spots')),
    ('KOI', dict(col='w', dark='s', belly='w', mark='o')), ('BETTA', dict(col='N', dark='M', belly='r', mark='r', finw=4)), ('PIRANHA', dict(col='s', dark='S', belly='r', mark='S')),
    ('DOLPHIN', dict(col='p', dark='P', belly='w', mark='P', snout=5)), ('CHINESE', dict(col='s', dark='S', belly='w', mark='S', snout=5)), ('MANATEE', dict(col='S', dark='k', belly='s', mark='S', rx=14, ry=8)),
    ('KOMODO', dict(col='S', dark='k', belly='s', mark='k', long=1.25)), ('CHAMELEON', dict(col='g', dark='G', belly='f', mark='o', frill=True, jaw=2)), ('IGUANA', dict(col='g', dark='G', belly='t', mark='d', frill=True, jaw=2)),
    ('FRILLED', dict(col='c', dark='C', belly='y', mark='o', frill=True, jaw=2)), ('AGAMA', dict(col='B', dark='N', belly='o', mark='o', jaw=2)), ('GECKO', dict(col='t', dark='T', belly='w', mark='o', jaw=1)),
    ('CAIMAN', dict(col='d', dark='M', belly='y', mark='k')), ('GHARIAL', dict(col='T', dark='d', belly='y', mark='k', jaw=9)), ('SCORPION', dict(col='C', dark='k', belly='c', mark='k')),
]
BASE = {
    'cat': dict(col='Y', dark='c', belly='y', mark='C', leg=8, ear='round', tail='thin'), 'dog': dict(col='c', dark='C', belly='y', mark='C', leg=8, ear='point', tail='bushy'),
    'giraffe': dict(leg=12, neck=17, neckw=4, neckx=1, head=4, ear='point', horn='ossicone', tail='tuft', body=.82), 'zebra': dict(leg=11, neck=8, neckw=5, ear='point', tail='tuft'),
    'elephant': dict(leg=9, body=1.2, head=6, neck=2, ear='big', trunk=True, tusk=True, legw=4, snout=0, tail='thin'), 'rhino': dict(col='s', dark='S', belly='s', mark='s', leg=6, body=1.2, neck=1, horn='nose', ear='point', legw=4),
    'hippo': dict(col='L', dark='M', belly='l', mark='L', leg=4, body=1.25, neck=0, head=6, snout=4, ear='round', legw=4), 'bovine': dict(col='C', dark='S', belly='c', mark='k', leg=9, body=1.1, neck=2, horn='curve', ear='long', tail='tuft', legw=3.4),
    'antelope': dict(col='c', dark='C', belly='w', mark='C', leg=11, body=.9, neck=7, neckw=3.4, horn='curve', ear='long', head=4, legw=2.2), 'camel': dict(col='Y', dark='c', belly='y', mark='c', leg=11, body=.9, neck=11, neckw=3.6, hump=True, ear='round', head=4, tail='tuft'),
    'pig': dict(col='C', dark='S', belly='c', mark='k', leg=5, body=1.05, neck=0, snout=3, ear='point'), 'primate': dict(col='C', dark='S', belly='y', mark='C'), 'bear': dict(col='C', dark='S', belly='c', mark='C', leg=6, body=1.2, neck=1, head=6, ear='round', tail='none', legw=4),
    'sloth': dict(col='c', dark='C', belly='y', mark='C', leg=5, body=1.05, neck=1, head=5, ear='none', tail='none', snout=1), 'armadillo': dict(col='S', dark='k', belly='s', mark='s', pattern='stripes', leg=3, long=.95, neck=0, head=4, snout=3, ear='round'),
    'dolphin': dict(col='s', dark='S', belly='w', mark='S', snout=5, rx=13, ry=6), 'manatee': dict(rx=14, ry=8), 'croc': dict(col='d', dark='M', belly='y', mark='k', long=1.1, jaw=7), 'turtle': dict(col='d', dark='M', belly='y', mark='G'),
    'snake': dict(col='g', dark='G', belly='t', mark='d'), 'lizard': dict(col='c', dark='C', belly='y', mark='C', jaw=2, long=.9), 'frog': dict(col='g', dark='G', belly='y', mark='d'), 'fish': dict(col='b', dark='B', belly='w', mark='N'), 'scorpion': dict(),
    'wader': dict(col='w', dark='s', belly='w', mark='S', leg=13, neck=9, neckw=2.2, rx=7, ry=5.5, head=3, beak='long', tail=5, tailw=2.5, leg_col='k'), 'flightless': dict(leg=13, neck=11, neckw=2.4, rx=9, ry=7, head=3.2, tail=6, beak='short', leg_col='p', col='S', dark='k', belly='s', mark='k'),
    'penguin': dict(leg=2, neck=0, rx=7.5, ry=11, head=5, tail=2, beak='short', leg_col='o'), 'raptor': dict(col='C', dark='S', belly='y', mark='S', leg=5, neck=0, rx=9, ry=8, beak='hook', tail=8, leg_col='f', beakcol='f'),
    'parrot': dict(col='r', dark='R', belly='y', mark='B', leg=3, neck=0, rx=7, ry=8, head=5, beak='hook', tail=12, taildrop=8, leg_col='S', beakcol='k'), 'bird': dict(col='B', dark='N', belly='y', mark='N', leg=5, neck=0, rx=7, ry=6, head=4, beak='short', tail=7, leg_col='k'),
    'small': dict(col='c', dark='C', belly='y', mark='k', leg=4, body=.82, long=1.15, neck=2, head=4.5, ear='round', tail='bushy', snout=2),
}
TONES = [('c', 'C', 'y'), ('Y', 'c', 'y'), ('C', 'S', 'c'), ('S', 'k', 's'), ('s', 'S', 'w'), ('o', 'O', 'y'), ('y', 'Y', 'w')]
BIRD_TONES = [('B', 'N', 'w', 'N'), ('r', 'R', 'y', 'k'), ('g', 'G', 'f', 'd'), ('f', 'Y', 'w', 'k'), ('l', 'L', 'w', 'L'), ('C', 'S', 'y', 'k'), ('t', 'T', 'w', 'd'), ('p', 'P', 'w', 'P'), ('S', 'k', 's', 'k')]


def look(name, arch, extra=None):
    """Combine body-plan defaults, name-driven markings and a stable colour variation."""
    a = dict(BASE.get(arch, {}))
    h = zlib.crc32(name.encode())
    matched = False
    for frag, d in LOOK:
        if frag in name:
            a.update(d)
            matched = True
            break
    if not matched:
        if arch in ('bird',):
            t = BIRD_TONES[h % len(BIRD_TONES)]
            a.update(col=t[0], dark=t[1], belly=t[2], mark=t[3])
        elif arch in ('cat', 'dog', 'small', 'antelope', 'bovine', 'pig', 'lizard', 'primate'):
            t = TONES[h % len(TONES)]
            a.update(col=t[0], dark=t[1], belly=t[2])
            if arch in ('cat',) and h & 8:
                a.update(pattern='spots', mark='k')
        elif arch == 'wader':
            t = [('w', 's', 'w', 'S'), ('s', 'S', 'w', 'k'), ('B', 'N', 'w', 'M'), ('S', 'k', 's', 'k'), ('y', 'Y', 'w', 'C')][h % 5]
            a.update(col=t[0], dark=t[1], belly=t[2], mark=t[3])
        elif arch == 'raptor':
            t = [('C', 'S', 'y', 'S'), ('S', 'k', 'w', 'k'), ('c', 'C', 'w', 'C'), ('k', 'k', 'w', 'S')][h % 4]
            a.update(col=t[0], dark=t[1], belly=t[2], mark=t[3], headcol='w' if h & 4 else t[0])
        elif arch == 'snake':
            t = [('g', 'G', 't', 'd'), ('c', 'C', 'y', 'C'), ('S', 'k', 's', 'k'), ('r', 'R', 'f', 'k'), ('Y', 'c', 'y', 'C')][h % 5]
            a.update(col=t[0], dark=t[1], belly=t[2], mark=t[3])
        elif arch == 'parrot':
            t = [('r', 'R', 'y', 'B'), ('g', 'G', 'f', 'r'), ('B', 'N', 'f', 'r'), ('f', 'Y', 'o', 'g')][h % 4]
            a.update(col=t[0], dark=t[1], belly=t[2], mark=t[3])
    a.setdefault('col', 'c')
    a.setdefault('dark', SHADE.get(a['col'], 'C'))
    a.setdefault('belly', 'y')
    a.setdefault('mark', a['dark'])
    a.setdefault('leg_col', 'k')
    if extra:                                  # a legend's own look wins over everything
        a.update(extra)
    return a


PLAN = {'wader': bird, 'flightless': bird, 'penguin': bird, 'raptor': bird, 'parrot': bird, 'bird': bird, 'primate': primate, 'snake': snake,
        'lizard': low_reptile, 'croc': low_reptile, 'turtle': turtle, 'frog': frog, 'fish': fish, 'dolphin': fish, 'manatee': fish, 'scorpion': scorpion,
        'bat': bat, 'spider': spider, 'octopus': octopus, 'plesio': plesio, 'spirit': spirit, 'wisp': wisp}


# Growth stages: 0 cub, 1 juvenile, 2 adult. Young ones are drawn from the same plan with a young
# body's proportions: a bigger head, shorter legs and neck, no horns, tusks or mane, softer colours.
STAGE_CUB, STAGE_JUV, STAGE_ADULT = 0, 1, 2
STAGE_PREFIX = {STAGE_CUB: 'c', STAGE_JUV: 'j', STAGE_ADULT: 'p'}
QUADS = set('cat dog giraffe zebra elephant rhino hippo bovine antelope camel pig bear sloth armadillo small'.split())
BIRDS = set('wader flightless penguin raptor parrot bird'.split())
SOFTER = {'Y': 'y', 'c': 'Y', 'C': 'c', 'O': 'o', 'G': 'g', 'B': 'b', 'R': 'r', 'P': 'p', 'd': 'T', 'L': 'l', 'M': 'N'}


def stage_look(a, arch, stage):
    """Returns (plan, look, zoom) for a growth stage."""
    plan = PLAN.get(arch, quad)
    if stage >= STAGE_ADULT:
        return plan, a, 1.0
    cub = stage == STAGE_CUB
    t = 0.0 if cub else 0.55

    def mix(baby, adult):
        return baby + (adult - baby) * t
    zoom = 1.0
    if arch in QUADS:
        a['body'] = a.get('body', 1.0) * mix(.68, 1.0)
        a['leg'] = a.get('leg', 8) * mix(.55, 1.0)
        a['neck'] = a.get('neck', 5) * mix(.55, 1.0)
        a['head'] = a.get('head', 5) * mix(1.12, 1.0)
        a['neckw'] = a.get('neckw', 5) * mix(.75, 1.0)
        a['legw'] = a.get('legw', 3) * mix(.85, 1.0)
        a['snout'] = max(1, round(a.get('snout', 2) * mix(.6, 1.0)))
        if cub:
            a.pop('horn', None)
            a['tusk'] = False
            if a.get('mane'):                    # lion cubs: no mane, faint spots
                a['mane'] = False
                a['pattern'] = 'spots'
                a['mark'] = 'c'
            if arch in ('bear', 'dog'):
                a['ear'] = 'round'
            if a.get('tail') == 'tuft':
                a['tail'] = 'thin'
        elif a.get('horn') == 'curve':
            a['horn'] = 'ossicone'               # little nubs
    elif arch in BIRDS:
        a['rx'] = a.get('rx', 9) * mix(.8, 1.0)
        a['ry'] = a.get('ry', 7) * mix(.95, 1.0)
        a['head'] = a.get('head', 4.5) * mix(1.25, 1.0)
        a['leg'] = a.get('leg', 7) * mix(.45, 1.0)
        a['neck'] = a.get('neck', 1) * mix(.3, 1.0)
        a['tail'] = a.get('tail', 8) * mix(.25, 1.0)
        if cub:                                  # a downy chick
            a['crest'] = False
            if a.get('beak') in ('long', 'huge'):
                a['beak'] = 'short'
            down = 's' if arch in ('raptor', 'penguin') else 'y'
            a.update(col=down, belly='w', mark=down, dark='S' if down == 's' else 'Y', headcol=down)
            if arch == 'penguin':
                a.update(col='S', dark='k', mark='S', headcol='S')
    elif arch == 'primate':
        a['bs'] = a.get('bs', 1.0) * mix(.7, 1.0)
        a['hs'] = mix(.95, 1.0)
        zoom = mix(.85, 1.0)
    elif arch == 'frog' and cub:                 # frogs start as tadpoles
        plan = fish
        a.update(rx=6, ry=4.5, mark=a['dark'], belly=a['col'], finw=1.5, finup=3)
        zoom = .8
    elif arch in ('croc', 'lizard', 'snake', 'turtle', 'frog'):
        a['hs'] = mix(1.35, 1.0)
        a['long'] = a.get('long', 1.0) * mix(.8, 1.0)
        a['jaw'] = max(1, round(a.get('jaw', 5) * mix(.5, 1.0)))
        zoom = mix(.62, .85)
    else:                                        # fish, dolphins, manatees, scorpions
        zoom = mix(.58, .8)
    if cub and arch not in ('cat',):
        a['col'] = SOFTER.get(a['col'], a['col'])
    return plan, a, zoom


def draw(name, arch, pose, size=40, stage=STAGE_ADULT, extra=None):
    plan, a, zoom = stage_look(look(name, arch, extra), arch, stage)
    c = Canvas(size, zoom)
    c.eyecol = a.get('eyecol', 'k')
    plan(c, a, pose)
    decorate(c, a, pose, zlib.crc32(name.encode()))
    extras(c, pose, zlib.crc32(name.encode()))
    return c.finish()


# 18 actions, ids as sdmon.h: IDLE WALKL WALKR SLEEP EAT HURT ATTACK POSE HOP NOD BREATH SIT,
# then the moods: SAD SICK TIRED HUNGRY DIRTY HAPPY
ACTIONS = [
    (0, [dict(), dict(dy=1, head=1)], 420),
    (1, [dict(leg=i, dy=(i % 2) * -1) for i in range(4)], 150),
    (2, [dict(leg=i, dy=(i % 2) * -1) for i in range(4)], 150),
    (3, [dict(low=True, eye='shut'), dict(low=True, eye='shut', dy=1)], 650),
    (4, [dict(head=3, mouth=True), dict(head=4)], 240),
    (5, [dict(eye='hurt', lunge=-2, mouth=True), dict(eye='hurt', lunge=-1, head=1)], 180),
    (6, [dict(lunge=-1), dict(lunge=3, mouth=True), dict(lunge=1, mouth=True)], 130),
    (7, [dict(head=-2, mouth=True), dict(head=-1)], 320),
    (8, [dict(), dict(dy=-4, leg=1), dict(dy=-1)], 130),
    (9, [dict(head=2), dict(head=-1)], 260),
    (10, [dict(), dict(dy=1)], 600),
    (11, [dict(low=True)], 900),
    (12, [dict(eye='sad', head=2, dy=1, tear=0), dict(eye='sad', head=2, dy=1, tear=2)], 520),
    (13, [dict(eye='sick', head=2, cheek=True, sweat=0, lunge=-1), dict(eye='sick', head=2, cheek=True, sweat=1)], 260),
    (14, [dict(eye='tired', head=2, dy=1), dict(eye='tired', head=3, dy=1), dict(eye='shut', head=4, dy=1)], 700),
    (15, [dict(eye='sad', head=-1, mouth=True, drool=0), dict(eye='sad', mouth=True, drool=1)], 380),
    (16, [dict(eye='tired', dirt=True), dict(eye='tired', dirt=True, dy=1)], 520),
    (17, [dict(eye='happy', mouth=True), dict(eye='happy', mouth=True, head=-2), dict(eye='happy', head=-1)], 200),
]
ACTION_WALK_LEFT = 1


def grid_indexed(grid, order):
    """Flatten a letter grid into palette indices (0xFF transparent)."""
    idx = {l: i for i, l in enumerate(order)}
    return bytes(0xFF if c is None else idx[c] for row in grid for c in row)


def tpk2(name, arch, stage=STAGE_ADULT, extra=None):
    """A complete sprite file in the firmware's TPK2 format."""
    import struct
    out = bytearray(b'TPK2')
    out.append(len(ACTIONS))
    out += struct.pack('<H', len(LETTERS))
    for l in LETTERS:
        out += struct.pack('<H', rgb565(PAL[l]))
    for act, poses, ms in ACTIONS:
        out += bytes((act, 40, 40, len(poses)))
        for _ in poses:
            out += struct.pack('<H', ms)
        for p in poses:
            g = draw(name, arch, p, stage=stage, extra=extra)
            if act == ACTION_WALK_LEFT:
                g = [list(reversed(r)) for r in g]
            out += grid_indexed(g, LETTERS)
    return bytes(out)
