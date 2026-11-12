#!/usr/bin/env python3
"""Fails if third-party game names creep back into this repository or its firmware.

Scans every tracked text file, plus the printable strings inside the built firmware, against a
list of terms this project removed. The list holds only salted SHA-256 prefixes, so it does not
republish the names it guards against.

    python tools/check_ip.py

LICENSE and CREDITS.md may name the upstream projects: the MIT licence requires those notices.
"""
import hashlib
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TERMS = os.path.join(ROOT, 'tools', 'gen', 'ip_terms.txt')
SALT = b'tamanimals:'
ALLOWED = {'LICENSE', 'CREDITS.md', 'tools/gen/ip_terms.txt'}
BINARY = ('.png', '.ttf', '.bin', '.pak', '.bundle')
WORD = re.compile(r"[a-zéA-Z][a-zéA-Z']+")


def digest(term):
    return hashlib.sha256(SALT + term.lower().encode()).hexdigest()[:16]


CJK_RUN = re.compile(r'[぀-ヿ㐀-鿿가-힯]+')


def hits_in(text, bad):
    words = WORD.findall(text)
    found = set()
    # Chinese, Japanese and Korean have no spaces, so check every 2-5 character stretch
    for run in CJK_RUN.findall(text):
        for n in range(2, 6):
            for i in range(len(run) - n + 1):
                if digest(run[i:i + n]) in bad:
                    found.add(run[i:i + n])
    for i, w in enumerate(words):
        if digest(w) in bad:
            found.add(w)
        if i + 1 < len(words) and digest(w + ' ' + words[i + 1]) in bad:
            found.add(w + ' ' + words[i + 1])
    return found


def main():
    if hasattr(sys.stdout, 'reconfigure'):
        sys.stdout.reconfigure(encoding='utf-8', errors='replace')   # the names it reports may be CJK
    bad = set(open(TERMS).read().split())
    files = subprocess.check_output(['git', 'ls-files', '-co', '--exclude-standard'], cwd=ROOT, text=True).splitlines()
    problems = 0
    scanned = 0
    for f in files:
        path = os.path.join(ROOT, f)
        if f in ALLOWED or f.endswith(BINARY) or not os.path.isfile(path):
            continue
        scanned += 1
        found = hits_in(open(path, encoding='utf-8', errors='replace').read(), bad)
        if found:
            problems += 1
            print(f'{f}: {sorted(found)[:12]}')
    app = os.path.join(ROOT, 'web', 'firmware', 'app.bin')
    if os.path.exists(app):
        strings = re.findall(rb'[\x20-\x7e]{3,}', open(app, 'rb').read())
        found = hits_in(b'\n'.join(strings).decode('ascii'), bad)
        scanned += 1
        if found:
            problems += 1
            print(f'web/firmware/app.bin: {sorted(found)[:20]}')
    print(f'{scanned} files checked, {problems} with protected terms')
    sys.exit(1 if problems else 0)


if __name__ == '__main__':
    main()
