#!/usr/bin/env python3
"""Runs the test suites with nothing but g++: no SDL, no bash, no board.

Each suite compiles the REAL firmware sources against the emulator's hardware stubs. Objects are
built once and shared, so the whole run stays quick. Works on Windows, macOS and Linux.

    python tools/emu/tests/run_headless.py            # everything
    python tools/emu/tests/run_headless.py battle     # just the ones matching "battle"
"""
import concurrent.futures as cf
import glob
import os
import re
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
EMU = os.path.dirname(HERE)
ROOT = os.path.dirname(os.path.dirname(EMU))
CORE = ['gbsynth', 'pet', 'i18n', 'party', 'battle', 'link', 'linknow', 'save', 'steps', 'cry', 'talk', 'album', 'weather']
NEEDS_SKETCH = re.compile(r'needs_sketch\(\) \{ case "\$1" in ([^)]*)\)')
STANDALONE = {'synth_test', 'cry_test'}
NEEDS_HOST = {'sprite_test'}
TIMEOUT = 300


def sh(cmd, **kw):
    return subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, errors='replace', **kw)


def main():
    flt = sys.argv[1] if len(sys.argv) > 1 else ''
    out = os.path.join(ROOT, 'build', 'tests')
    os.makedirs(out, exist_ok=True)
    flags = ['-std=c++17', '-O0', '-w', '-I' + EMU, '-I' + ROOT, '-DSPRITE_DIR="' + os.path.join(ROOT, 'build', 'sdcard', 'mons').replace('\\', '/') + '"']

    runsh = open(os.path.join(HERE, 'run.sh'), encoding='utf-8').read()
    m = re.search(r'CORE=\(([^)]*)\)', runsh)
    core = [os.path.join(ROOT, os.path.basename(x.strip('"'))) for x in m.group(1).split()] if m else [os.path.join(ROOT, c + '.cpp') for c in CORE]
    core = [c.replace('$ROOT/', '') for c in core]
    sk = NEEDS_SKETCH.search(runsh)
    sketch_tests = set(sk.group(1).split('|')) if sk else set()

    # generate proto.h + sketch.cpp the way build.sh does
    sh([sys.executable, 'genproto.py', os.path.join(ROOT, 'Tamanimals.ino')], cwd=EMU)
    with open(os.path.join(EMU, 'sketch.cpp'), 'w', encoding='utf-8', newline='\n') as f:
        f.write('#include "proto.h"\n' + open(os.path.join(ROOT, 'Tamanimals.ino'), encoding='utf-8', errors='replace').read())

    def obj(src):
        name = os.path.splitext(os.path.basename(src))[0]
        o = os.path.join(out, name + '.o')
        r = sh(['g++', *flags, '-c', src, '-o', o])
        return name, o, r

    extra = [os.path.join(ROOT, 'cry.cpp')] if not any(c.endswith('cry.cpp') for c in core) else []
    sources = core + extra + [os.path.join(EMU, x) for x in ('sketch.cpp', 'host_impl.cpp', 'font.cpp', 'clock.cpp')]
    objs = {}
    with cf.ThreadPoolExecutor(max_workers=os.cpu_count() or 4) as ex:
        for name, o, r in ex.map(obj, sources):
            if r.returncode:
                print('=== object', name, 'DID NOT COMPILE\n', r.stdout[-1500:])
            objs[name] = o
    core_objs = [objs[os.path.splitext(os.path.basename(c))[0]] for c in core]

    def run(src):
        name = os.path.splitext(os.path.basename(src))[0]
        workdir = tempfile.mkdtemp(prefix=name + '_', dir=out)
        exe = os.path.join(workdir, name + '.exe')
        links = list(core_objs)
        if name in STANDALONE:
            links = [objs['gbsynth'], objs['cry']]
        if name in sketch_tests:
            links += [objs['sketch'], objs['host_impl'], objs['font'], objs['clock']]
        if name in NEEDS_HOST:
            links += [objs['host_impl'], objs['font']]
        r = sh(['g++', *flags, '-o', exe, src, *links])
        if r.returncode:
            return name, 'NOCOMPILE', r.stdout[-900:]
        try:
            t = sh([exe], cwd=workdir, timeout=TIMEOUT)
        except subprocess.TimeoutExpired:
            return name, 'TIMEOUT', ''
        finally:
            pass
        lines = [l for l in t.stdout.splitlines() if not re.match(r'(Tamanimals fw|emu:|RTC )', l)]
        shutil.rmtree(workdir, ignore_errors=True)
        fails = [l for l in lines if l.lstrip().startswith('FAIL') and l.strip() not in ('FAILED', 'FAILURES')]
        return name, ('PASS' if t.returncode == 0 else 'FAIL %d' % t.returncode), '\n'.join(fails[:30] + lines[-2:])

    wanted = flt.split(',')
    suites = sorted(s for s in glob.glob(os.path.join(HERE, '*_test.cpp')) if any(w in os.path.basename(s) for w in wanted))
    res = {}
    # a few suites time real milliseconds, so leave the machine some room
    with cf.ThreadPoolExecutor(max_workers=max(1, (os.cpu_count() or 4) // 3)) as ex:
        for name, st, txt in ex.map(run, suites):
            res[name] = st
            print(f'{st:10} {name}')
            if st != 'PASS':
                print('    ' + txt.replace('\n', '\n    ')[-1000:])
    ok = sum(v == 'PASS' for v in res.values())
    print(f'\nsuites passed: {ok}, failed: {len(res) - ok}')
    sys.exit(0 if ok == len(res) else 1)


main()
