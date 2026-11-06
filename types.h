#pragma once
#include <stdint.h>
#include "dex.h"

// Type effectiveness helpers. The chart itself (TYPE_FX) and the PowerType enum
// are generated into dex.h by tools/gen/gen_dex.py.
//
// Everything is integer percentages: 100 = neutral, 200 = strong hit,
// 50 = resisted, 0 = immune, and 400 / 25 for the doubled-up dual-type cases.
// No floats anywhere, so this is cheap on the MCU.

// Effectiveness of an attacking type against a defender with one or two types.
// Pass T_NONE as def2 for single-typed defenders.
static inline uint16_t typeEffPct(uint8_t atk, uint8_t def1, uint8_t def2) {
  if (atk >= TYPE_COUNT || def1 >= TYPE_COUNT) return 100;
  uint16_t e = TYPE_FX[atk][def1];
  e *= (def2 < TYPE_COUNT) ? TYPE_FX[atk][def2] : 10;
  return e;  // tenths x tenths = percent
}

// Same, but for a species straight out of the field guide.
static inline uint16_t typeEffVsDex(uint8_t atk, int16_t dex) {
  if (dex < 1 || dex > DEX_COUNT) return 100;
  return typeEffPct(atk, DEX_TBL[dex].type1, DEX_TBL[dex].type2);
}

// Same-Type Attack Bonus: 1.5x when the move matches one of the user's types.
static inline bool hasStab(int16_t dex, uint8_t moveType) {
  if (dex < 1 || dex > DEX_COUNT) return false;
  return DEX_TBL[dex].type1 == moveType || DEX_TBL[dex].type2 == moveType;
}

// Short display name for a type. English in every language, matching how the
// species names in DEX_TBL are already English regardless of the UI language.
static inline const char *typeName(uint8_t t) {
  static const char *const N[TYPE_COUNT] = {
    "STRENGTH", "SPEED", "AGILITY", "SMARTS", "FLIGHT", "SWIM", "DIG",
    "STEALTH", "VENOM", "ARMOR", "SIZE", "SOUND", "PACK",
  };
  return (t < TYPE_COUNT) ? N[t] : "?";
}

// One colour per type, for the chips on move rows and the battle grid. The move
// list used to print the type as grey 6px text, which on a 466px round panel at
// arm's length is unreadable -- and type is the single most important thing
// about a move in a game whose whole combat model is the type chart.
//
// These are this game's own trait colours in RGB565. They are
// hand-written rather than generated because they are not derived from anything:
// DexEntry.accent is per SPECIES and says nothing about a move.
static const uint16_t TYPE_COL[TYPE_COUNT] = {
  0xC186, 0xFD20, 0xFEA0, 0xA254,   // STRENGTH, SPEED, AGILITY, SMARTS
  0x7E9F, 0x34DF, 0xB3C8, 0x5249,   // FLIGHT, SWIM, DIG, STEALTH
  0x8A9C, 0x8C71, 0x5DCD, 0xF454,   // VENOM, ARMOR, SIZE, SOUND
  0xCC47,                           // PACK
};

// Whether a type's colour is light enough to need dark text on it. Precomputed
// from the same luminance the eye uses (0.299/0.587/0.114) so the label never
// comes out grey-on-yellow.
static const uint8_t TYPE_COL_LIGHT[TYPE_COUNT] = { 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1 };

static inline uint16_t typeColor(uint8_t t) {
  return t < TYPE_COUNT ? TYPE_COL[t] : 0x8410;
}
static inline bool typeColorIsLight(uint8_t t) {
  return t < TYPE_COUNT && TYPE_COL_LIGHT[t];
}
