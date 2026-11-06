# Planned features (not in the build)

These headers were written for features that have never compiled or run, so the firmware does not
include them. They are kept here so the work is not lost.

Covers: daily rewards, expanded achievements, educational animal facts, token and respect-based
catching, guilds, leaderboards, weekly challenges, a battle pass, cosmetics, habitat unlocks, session
safeguards, emotional sprite states, HD sprite loading and NVS persistence.

## Before adding one to the build

Each needs real integration work, not just an `#include`:

- Several define types that clash with the engine (`Pet`, `PetMood`, `MOOD_HAPPY`, `LANG_COUNT`,
  `RARITY_*`) or read `Pet` fields that do not exist.
- Some screens read physical buttons (`BTN_A`, `BTN_UP`). This board is touch only.
- Save and load must go through the engine's `save.cpp` rather than a separate store.
- Anything that sells something, such as the battle pass, needs a real design first. The board has no
  payment path.

Bring them in one at a time, with a test in `tools/emu/tests/`.
