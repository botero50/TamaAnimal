#!/usr/bin/env python3
"""HD rendering for the creatures: a bigger canvas, light and shadow, coloured outlines and eye shine.

The body plans in spritelib.py stay the same; this draws them at 56x56 and then paints them
the way a pixel artist would:
  - every colour gets a ramp: highlight, base, shadow, deep shadow (made from the base, so a
    species keeps its colours);
  - light comes from the top left: a rim of highlight along edges facing it, and each run of
    one colour darkens toward its bottom, with a dithered step between the tones;
  - the outline takes the deep shade of the colour it borders (black only along the bottom
    and right, where the shadow is), so shapes read without a heavy black line;
  - a white glint in every open eye.
Standard library only.
"""
import spritelib as sl

HD_SIZE = 56
HOOVED = set('zebra antelope bovine giraffe camel rhino hippo elephant pig'.split())


def _mix(a, b, t):
    return tuple(int(round(a[i] + (b[i] - a[i]) * t)) for i in range(3))


def _rgb(v):
    return ((v >> 16) & 255, (v >> 8) & 255, v & 255)


def _hsv_shift(c, amount):
    """Darken toward a cooler, more saturated tone (shadows) or lighten toward a warm white (light)."""
    if amount > 0:
        return _mix(c, (255, 250, 232), amount)
    t = -amount
    cool = (max(0, c[0] - 40), max(0, c[1] - 30), min(255, c[2] + 10))
    return _mix(c, _mix(cool, (20, 18, 34), .55), t)


# tone 0 base, 1 highlight, 2 shadow, 3 deep (outline)
RAMP = {}
for L, v in sl.PAL.items():
    base = _rgb(v)
    RAMP[L] = (base, _hsv_shift(base, .32), _hsv_shift(base, .32), _hsv_shift(base, .72))
RAMP['k'] = ((27, 27, 37), (60, 60, 78), (16, 16, 24), (10, 10, 16))
RAMP['w'] = ((255, 255, 255), (255, 255, 255), (214, 220, 232), (120, 124, 140))


def draw_hd(name, arch, pose, stage=sl.STAGE_ADULT, extra=None, size=HD_SIZE):
    """A frame as rows of (letter, tone) or None."""
    plan, a, zoom = sl.stage_look(sl.look(name, arch, extra), arch, stage)
    if arch in HOOVED and 'hoof' not in a:
        a['hoof'] = 'k' if a['col'] not in ('k', 'S', 'M') else 'C'
    c = sl.Canvas(size, zoom)
    c.hd = True
    c.iris = a.get('iris', 'C' if a['col'] not in ('C', 'c') else 'Y')
    c.eyecol = a.get('eyecol', 'k')
    plan(c, a, pose)
    sl.decorate(c, a, pose, sl.zlib.crc32(name.encode()))
    sl.extras(c, pose, sl.zlib.crc32(name.encode()))
    g = c.g
    n = size
    out = [[None if v is None else (v, 0) for v in row] for row in g]

    def at(x, y):
        return g[y][x] if 0 <= x < n and 0 <= y < n else None
    # 1. each vertical run of one colour darkens toward its bottom; the top of it catches light
    for x in range(n):
        y = 0
        while y < n:
            v = g[y][x]
            if v is None or v == 'k':
                y += 1
                continue
            y0 = y
            while y < n and g[y][x] == v:
                y += 1
            run = y - y0
            if run >= 5:                         # the lower third of the run in shadow
                for yy in range(y0 + int(run * .66 + .5), y):
                    out[yy][x] = (v, 2)
    # 2. a rim of light along edges that face the top left, and of shadow on the others
    for y in range(n):
        for x in range(n):
            v = g[y][x]
            if v is None or v == 'k':
                continue
            up, left = at(x, y - 1), at(x - 1, y)
            down, right = at(x, y + 1), at(x + 1, y)
            if (up != v and up != 'k') or (left is None):
                if out[y][x][1] != 2:
                    out[y][x] = (v, 1)
            elif down is None or right is None:
                out[y][x] = (v, 2)
    # 3. eye glints: the top-left pixel of every open pupil
    for ex, ey in getattr(c, 'eyes', []):
        if pose.get('eye', 'open') != 'open':
            continue
        gx, gy = int(ex * c.f) + c.ox, int((ey - 1) * c.f) + c.oy
        if 0 <= gx < n and 0 <= gy < n and g[gy][gx] not in (None, 'w'):
            out[gy][gx] = ('w', 0)
    # 4. the outline: the deep tone of the colour it borders; black along the bottom and right
    final = [row[:] for row in out]
    for y in range(n):
        for x in range(n):
            if g[y][x] is not None:
                continue
            nb = [(dx, dy) for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)) if at(x + dx, y + dy) is not None]
            if not nb:
                continue
            lit = any(d in nb for d in ((1, 0), (0, 1)))        # the body is right of / below this pixel
            dark = any(d in nb for d in ((-1, 0), (0, -1)))     # the body is left of / above it
            src = at(x + nb[0][0], y + nb[0][1])
            final[y][x] = ('k', 0) if dark and not lit else (src, 3)
    return final


def to_rgb(cell):
    if cell is None:
        return None
    L, tone = cell
    return RAMP[L][tone]
