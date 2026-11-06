# Desktop emulator and tests

The firmware's real sources, compiled against stubs of the hardware in this folder. The stubs are
`Arduino.h`, `Arduino_GFX_Library.h`, `Preferences.h`, `TouchDrvCSTXXX.hpp`, `Wire.h` and `host_impl.cpp`.

## Tests

No board and no SDL needed, just `g++`:

```bash
python tools/emu/tests/run_headless.py          # everything
python tools/emu/tests/run_headless.py battle   # only the suites matching "battle"
```

## The windowed emulator

Needs SDL2 (`brew install sdl2`, or `apt install libsdl2-dev`) and bash:

```bash
python tools/gen/gen_sprites.py
bash tools/emu/build.sh
tools/emu/tamanimals-emu
```

`build.sh` regenerates `proto.h` and `sketch.cpp` from `Tamanimals.ino` on every build.
