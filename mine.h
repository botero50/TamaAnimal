#pragma once
#include <stdint.h>
#include "dex.h"

// The Mine (map page 3): tap the rock until it breaks. Strong miners break it in fewer taps.
// A dig costs energy and there are a few a day. The loot: coins, food, seeds, pieces of amber
// (AMBER_PER_FOSSIL of them hold a fossil) and, rarely, a fossil at once: the next egg is prehistoric.
// The idea follows Mu-Muu's fork of the engine (MIT, see CREDITS.md); the rules, the numbers and the art are ours.
//
// Pure logic with host tests (mine_test.cpp).

#define MINE_DIGS_PER_DAY 5
#define MINE_ENERGY_COST 10
#define MINE_MIN_ENERGY 10          // too tired below this
#define AMBER_PER_FOSSIL 10

static inline uint8_t mineTapsFor(uint8_t type1, uint8_t type2, uint16_t atk) {
  auto rank = [](uint8_t t) -> int {
    if (t == PT_DIG || t == PT_STRENGTH) return 8;      // diggers and the strong
    if (t == PT_ARMOR || t == PT_SIZE) return 10;       // hard heads and the big
    return 14;
  };
  int a = rank(type1), b = type2 == PT_NONE ? 14 : rank(type2);
  int base = a < b ? a : b;
  base -= (int)(atk / 40);                              // a strong arm saves taps
  if (base < 4) base = 4;
  return (uint8_t)base;
}

enum MineKind : uint8_t { ML_COINS = 0, ML_AMBER, ML_SNACK, ML_SEEDS, ML_FOSSIL };

// roll 0..99
static inline MineKind mineLootKind(uint8_t roll) {
  if (roll < 3) return ML_FOSSIL;
  if (roll < 28) return ML_AMBER;
  if (roll < 73) return ML_COINS;
  if (roll < 88) return ML_SEEDS;
  return ML_SNACK;
}

// 2..5 coins for a coin find
static inline uint8_t mineCoinsFor(uint8_t roll) { return (uint8_t)(2 + roll % 4); }

// the animals a fossil egg can hatch: the prehistoric ones of Africa (131-143) and the Americas (237-255)
#define FOSSIL_A0 131
#define FOSSIL_A1 143
#define FOSSIL_B0 237
#define FOSSIL_B1 255
#define FOSSIL_COUNT ((FOSSIL_A1 - FOSSIL_A0 + 1) + (FOSSIL_B1 - FOSSIL_B0 + 1))
static inline int16_t fossilAt(int i) {
  const int na = FOSSIL_A1 - FOSSIL_A0 + 1;
  return (int16_t)(i < na ? FOSSIL_A0 + i : FOSSIL_B0 + (i - na));
}
static inline bool isFossilDex(int16_t d) {
  return (d >= FOSSIL_A0 && d <= FOSSIL_A1) || (d >= FOSSIL_B0 && d <= FOSSIL_B1);
}

// A piece of amber: AMBER_PER_FOSSIL of them hold a fossil. While a fossil already waits (or none can
// hatch) the amber waits at one short, so the next one is not wasted.
struct AmberResult { uint8_t amber; bool fossil; };
static inline AmberResult mineAddAmber(uint8_t amber, bool canHold) {
  AmberResult r = { amber, false };
  if (amber + 1 < AMBER_PER_FOSSIL) { r.amber = (uint8_t)(amber + 1); return r; }
  if (!canHold) { r.amber = AMBER_PER_FOSSIL - 1; return r; }
  r.amber = 0;
  r.fossil = true;
  return r;
}

// digs left today: `day` is the day of the last count, `used` how many were done then
static inline uint8_t mineDigsLeft(uint32_t day, uint8_t used, uint32_t today) {
  if (day != today) return MINE_DIGS_PER_DAY;
  return used < MINE_DIGS_PER_DAY ? (uint8_t)(MINE_DIGS_PER_DAY - used) : 0;
}
