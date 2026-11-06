#!/usr/bin/env python3
"""Generates dex.h, moves.h and trainers.h from tools/gen/animals.json.

Everything here is original to Tamanimals: the 13 trait-based types and their
chart, the move list, the learnsets (derived from each animal's traits) and the
ranger roster. Standard library only.

    python tools/gen/gen_dex.py
"""
import json
import os
import zlib

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.join(HERE, '..', '..')
ANIMALS = json.load(open(os.path.join(HERE, 'animals.json')))
TYPES = ['STRENGTH', 'SPEED', 'AGILITY', 'INTELLIGENCE', 'FLIGHT', 'SWIM', 'DIG', 'HIDE', 'VENOM', 'ARMOR', 'SIZE', 'SOUND', 'PACK']
ACCENT = {'STRENGTH': 0xC186, 'SPEED': 0xFD20, 'AGILITY': 0xFEA0, 'INTELLIGENCE': 0xA254, 'FLIGHT': 0x7E9F, 'SWIM': 0x34DF,
          'DIG': 0xB3C8, 'HIDE': 0x5249, 'VENOM': 0x8A9C, 'ARMOR': 0x8C71, 'SIZE': 0x5DCD, 'SOUND': 0xF454, 'PACK': 0xCC47}

# attacker: (strong against, weak against, cannot affect). Written for this game.
CHART = {
    'STRENGTH': (['ARMOR', 'PACK', 'DIG'], ['FLIGHT', 'VENOM'], []),
    'SPEED': (['SIZE', 'DIG', 'HIDE'], ['ARMOR', 'AGILITY'], []),
    'AGILITY': (['SPEED', 'VENOM'], ['PACK', 'SOUND'], []),
    'INTELLIGENCE': (['STRENGTH', 'SIZE', 'PACK'], ['HIDE', 'SOUND'], []),
    'FLIGHT': (['STRENGTH', 'DIG', 'VENOM'], ['SOUND', 'ARMOR'], []),
    'SWIM': (['DIG', 'ARMOR'], ['SIZE', 'SWIM'], []),
    'DIG': (['VENOM', 'SOUND'], ['SWIM', 'SIZE'], ['FLIGHT']),
    'HIDE': (['INTELLIGENCE', 'SOUND'], ['SPEED', 'PACK'], []),
    'VENOM': (['SIZE', 'AGILITY'], ['ARMOR', 'VENOM', 'DIG'], []),
    'ARMOR': (['FLIGHT', 'SPEED'], ['STRENGTH', 'ARMOR'], []),
    'SIZE': (['SWIM', 'DIG'], ['INTELLIGENCE', 'VENOM'], []),
    'SOUND': (['FLIGHT', 'INTELLIGENCE'], ['SOUND', 'DIG'], []),
    'PACK': (['AGILITY', 'HIDE', 'FLIGHT'], ['STRENGTH', 'SIZE'], []),
}

# name, type, category, power, accuracy, effect, param, stat mask, stages, target, ailment, chance
def hit(name, ty, cat, power, acc=100, eff='EF_NONE', param=0, ail='AIL_NONE', chance=0):
    return (name, ty, cat, power, acc, eff, param, '0', 0, 'TG_FOE', ail, chance)


def stat(name, ty, mask, stages, target='TG_SELF'):
    return (name, ty, 'MC_STATUS', 0, 0, 'EF_STAGE', 0, mask, stages, target, 'AIL_NONE', 0)


P, S = 'MC_PHYS', 'MC_SPEC'
MOVES = [
    ('-', 'STRENGTH', 'MC_STATUS', 0, 0, 'EF_NONE', 0, '0', 0, 'TG_SELF', 'AIL_NONE', 0),
    hit('POUNCE', 'STRENGTH', P, 40), hit('HEADBUTT RUN', 'STRENGTH', P, 60), hit('MAUL', 'STRENGTH', P, 75),
    hit('BRAWL', 'STRENGTH', P, 90), hit('RAMPAGE', 'STRENGTH', P, 115, 100, 'EF_RECOIL', 3),
    hit('DASH', 'SPEED', P, 40, 100, 'EF_PRIORITY', 1), hit('CLAW SWIPE', 'SPEED', P, 55), hit('SPRINT KICK', 'SPEED', P, 75),
    hit('BLITZ', 'SPEED', P, 90), hit('STAMPEDE RUN', 'SPEED', P, 110, 85),
    hit('NIP FLURRY', 'AGILITY', P, 15, 85, 'EF_MULTI'), hit('FEINT HOP', 'AGILITY', S, 60, 0, 'EF_NEVER_MISS'),
    hit('TUMBLE', 'AGILITY', P, 75), hit('ACRO LEAP', 'AGILITY', P, 95, 90),
    hit('TRICKERY', 'INTELLIGENCE', S, 40), hit('RIDDLE', 'INTELLIGENCE', S, 65, 100, 'EF_NONE', 0, 'AIL_CONFUSE', 15),
    hit('INSIGHT', 'INTELLIGENCE', S, 90), hit('SHARP MIND', 'INTELLIGENCE', S, 0, 100, 'EF_FIXED_LVL'),
    hit('WING FLAP', 'FLIGHT', P, 35), hit('UPDRAFT', 'FLIGHT', S, 60), hit('TALON DIVE', 'FLIGHT', P, 90, 95, 'EF_CHARGE', 1),
    hit('SKY STRIKE', 'FLIGHT', P, 110, 85),
    hit('SPLASH JET', 'SWIM', S, 40), hit('FIN SLAP', 'SWIM', P, 60), hit('RIPTIDE', 'SWIM', S, 75),
    hit('DEEP DIVE', 'SWIM', P, 85, 100, 'EF_CHARGE', 1), hit('TIDAL SURGE', 'SWIM', S, 110, 80),
    hit('SCRABBLE', 'DIG', P, 40), hit('DUST KICK', 'DIG', P, 60), hit('BURROW', 'DIG', P, 80, 100, 'EF_CHARGE', 1),
    hit('TREMOR', 'DIG', P, 100),
    hit('STALK', 'HIDE', P, 40, 100, 'EF_PRIORITY', 1), hit('AMBUSH', 'HIDE', P, 70), hit('MOON SHADE', 'HIDE', S, 0, 100, 'EF_FIXED_LVL'),
    hit('NIGHT RAID', 'HIDE', P, 90), hit('DUSK VEIL', 'HIDE', S, 80),
    hit('STINGER', 'VENOM', P, 20, 100, 'EF_NONE', 0, 'AIL_POISON', 30), hit('TOXIC SPIT', 'VENOM', S, 40, 100, 'EF_NONE', 0, 'AIL_POISON', 15),
    hit('VENOM FANG', 'VENOM', P, 65, 100, 'EF_NONE', 0, 'AIL_POISON', 30), hit('NEUROTOXIN', 'VENOM', S, 90, 100, 'EF_NONE', 0, 'AIL_PARA', 25),
    hit('SHELL BASH', 'ARMOR', P, 50), hit('SPIKE ROLL', 'ARMOR', P, 70), hit('PLATE RAM', 'ARMOR', P, 90),
    stat('HARDEN UP', 'ARMOR', 'ST_DEF', 2),
    hit('STOMP', 'SIZE', P, 65), hit('TRAMPLE', 'SIZE', P, 85), hit('BULLDOZE', 'SIZE', P, 100),
    hit('HEAVY SLAM', 'SIZE', P, 120, 100, 'EF_RECOIL', 3), hit('TUSK SWING', 'SIZE', P, 75),
    hit('CHIRP', 'SOUND', S, 40), hit('LOUD CALL', 'SOUND', S, 65, 100, 'EF_NONE', 0, 'AIL_CONFUSE', 10),
    hit('SONIC CRY', 'SOUND', S, 90), hit('THUNDER ROAR', 'SOUND', S, 110, 80, 'EF_NONE', 0, 'AIL_PARA', 20),
    stat('SHRIEK', 'SOUND', 'ST_DEF', -2, 'TG_FOE'),
    hit('TEAM NIP', 'PACK', P, 25, 95, 'EF_MULTI'), hit('FLANK', 'PACK', P, 60), hit('SWARM', 'PACK', P, 90),
    hit('GANG RUSH', 'PACK', P, 110, 85),
    # shared utility
    stat('WAR DANCE', 'STRENGTH', 'ST_ATK', 2), stat('LIMBER UP', 'AGILITY', 'ST_SPE', 2), stat('FOCUS', 'INTELLIGENCE', 'ST_SPA', 2),
    stat('STAND TALL', 'SIZE', 'ST_SPD', 2), stat('HUNT CALL', 'PACK', 'ST_ATK | ST_SPE', 1), stat('BRACE', 'ARMOR', 'ST_ATK | ST_DEF', 1),
    stat('SNARL', 'SOUND', 'ST_ATK', -1, 'TG_FOE'), stat('STARE DOWN', 'HIDE', 'ST_DEF', -1, 'TG_FOE'),
    stat('DIRT CLOUD', 'DIG', 'ST_SPE', -1, 'TG_FOE'),
    ('LICK WOUNDS', 'HIDE', 'MC_STATUS', 0, 0, 'EF_HEAL', 50, '0', 0, 'TG_SELF', 'AIL_NONE', 0),
    ('LONG NAP', 'SIZE', 'MC_STATUS', 0, 0, 'EF_HEAL', 50, '0', 0, 'TG_SELF', 'AIL_NONE', 0),
    hit('SWEAT SPRAY', 'VENOM', S, 35, 100, 'EF_NONE', 0, 'AIL_BURN', 20), hit('SUN SCORCH', 'FLIGHT', S, 75, 100, 'EF_NONE', 0, 'AIL_BURN', 15),
    hit('CHILL BREATH', 'SWIM', S, 70, 100, 'EF_NONE', 0, 'AIL_FREEZE', 10), hit('DOZE SONG', 'SOUND', S, 30, 100, 'EF_NONE', 0, 'AIL_SLEEP', 25),
    hit('LEECH BITE', 'VENOM', P, 60, 100, 'EF_DRAIN', 50), hit('NECTAR SIP', 'FLIGHT', S, 40, 100, 'EF_DRAIN', 50),
    hit('KNOCK ASIDE', 'STRENGTH', P, 0, 100, 'EF_FIXED', 40), hit('OVERREACH', 'SIZE', S, 140, 90, 'EF_RECHARGE'),
    hit('STRUGGLE', 'STRENGTH', P, 50, 0, 'EF_RECOIL', 4),
]
MOVE_ID = {m[0]: i for i, m in enumerate(MOVES)}
assert all(len(m[0]) <= 12 for m in MOVES), [m[0] for m in MOVES if len(m[0]) > 12]
assert len(MOVE_ID) == len(MOVES)

# Stat profile per body plan: hp, atk, def, spe, spa, spd (roughly summing to 100).
PROFILE = {
    'cat': (15, 24, 13, 24, 12, 12), 'dog': (15, 21, 14, 22, 14, 14), 'giraffe': (20, 15, 15, 15, 17, 18), 'zebra': (15, 17, 15, 26, 13, 14),
    'elephant': (27, 22, 20, 7, 12, 12), 'rhino': (20, 24, 26, 10, 8, 12), 'hippo': (27, 20, 21, 8, 12, 12), 'bovine': (20, 23, 19, 15, 10, 13),
    'antelope': (14, 17, 14, 27, 14, 14), 'camel': (21, 15, 17, 15, 14, 18), 'pig': (18, 21, 19, 14, 13, 15), 'primate': (15, 15, 13, 18, 24, 15),
    'bear': (24, 25, 18, 10, 11, 12), 'sloth': (24, 14, 18, 6, 18, 20), 'armadillo': (15, 15, 30, 10, 12, 18), 'dolphin': (17, 13, 13, 21, 22, 14),
    'manatee': (28, 11, 18, 7, 16, 20), 'croc': (20, 26, 23, 10, 9, 12), 'turtle': (17, 11, 31, 6, 14, 21), 'snake': (14, 22, 13, 20, 18, 13),
    'lizard': (14, 18, 15, 21, 17, 15), 'frog': (14, 13, 12, 22, 23, 16), 'fish': (13, 22, 13, 24, 15, 13), 'scorpion': (13, 23, 24, 15, 13, 12),
    'wader': (15, 16, 13, 18, 18, 20), 'flightless': (19, 21, 13, 25, 10, 12), 'penguin': (18, 16, 18, 16, 16, 16), 'raptor': (14, 25, 12, 25, 13, 11),
    'parrot': (14, 13, 12, 20, 25, 16), 'bird': (14, 15, 12, 25, 18, 16), 'small': (14, 18, 15, 22, 16, 15),
}
TOTAL = {1: 290, 2: 390, 3: 470, 4: 560}
REGIONS = [('AFRICA', 1, 145, [1, 2, 3]), ('ASIA', 146, 200, [146, 147, 171]), ('AMERICAS', 201, 325, [201, 202, 205])]
ALL_STARTERS = [1, 2, 3, 146, 147, 171, 201, 202, 205]
CLASSIC = [1, 2, 3, 201, 146]


def h(s):
    return zlib.crc32(s.encode())


def stats(a):
    prof, total = PROFILE[a['arch']], TOTAL[a['rarity']]
    out = []
    for i, w in enumerate(prof):
        wiggle = ((h(a['name'] + str(i)) % 21) - 10)
        out.append(max(15, min(200, total * w // 100 + wiggle)))
    return out


def gen_dex():
    o = ['#pragma once', '#include <stdint.h>', '',
         '// GENERATED by tools/gen/gen_dex.py from tools/gen/animals.json - do not edit',
         '', f'#define DEX_COUNT {len(ANIMALS)}', '',
         '// Thirteen trait-based types, and a chart written for this game.',
         'enum PowerType : uint8_t {',
         '  ' + ', '.join('PT_' + t for t in TYPES) + ',', '  PT_NONE = 255', '};',
         f'#define TYPE_COUNT {len(TYPES)}', '#define POWER_TYPE_COUNT TYPE_COUNT', '#define T_NONE PT_NONE', '',
         '// Chart in TENTHS (0 immune, 5 resisted, 10 neutral, 20 strong). Multiplying the two',
         '// defender columns gives a percentage directly: 20*20 = 400, 10*10 = 100, 5*10 = 50.',
         'static const uint8_t TYPE_FX[TYPE_COUNT][TYPE_COUNT] = {']
    for a in TYPES:
        strong, weak, none = CHART[a]
        row = [20 if d in strong else 5 if d in weak else 0 if d in none else 10 for d in TYPES]
        o.append('  { ' + ', '.join(f'{v:2d}' for v in row) + f' }},  // {a.lower()}')
    o += ['};', '',
          '// rarity: 0 = never hatches, then the three tiers an egg rolls. animals.json grades animals',
          '// 1-4 for their stats; 1 and 2 both hatch as common, or half of them could never appear.',
          'enum : uint8_t { R_EVO = 0, R_COMUN = 1, R_RARO = 2, R_LEGENDARIO = 3 };', '',
          'struct DexEntry {',
          '  const char *name;', '  uint16_t evolvesTo;   // animals do not evolve; kept 0 for the shared engine',
          '  uint8_t evolveLevel;', '  uint8_t rarity;', '  uint16_t accent;      // RGB565 colour of the main type',
          '  uint8_t bHp, bAtk, bDef, bSpe;', '  uint8_t bSpA, bSpD;',
          '  uint8_t biome;        // 0 meadow 1 beach 2 forest 3 volcano 4 mountain 5 snow',
          '  uint8_t type1, type2; // type2 = T_NONE if it has one', '};', '',
          'static const DexEntry DEX_TBL[DEX_COUNT + 1] = {',
          '  { "?", 0, 0, 0, 0x2946, 50, 50, 50, 50, 50, 50, 0, PT_NONE, PT_NONE },']
    for a in ANIMALS:
        hp, at, de, sp, sa, sd = stats(a)
        t2 = 'PT_' + a['type2'] if a['type2'] else 'PT_NONE'
        tier = {1: 1, 2: 1, 3: 2, 4: 3}[a['rarity']]
        o.append(f'  {{ "{a["name"]}", 0, 0, {tier}, 0x{ACCENT[a["type1"]]:04X}, {hp}, {at}, {de}, {sp}, {sa}, {sd}, {a["biome"]}, PT_{a["type1"]}, {t2} }},  // {a["id"]}')
    o += ['};', '',
          '// the first egg: a friendly mix',
          f'static const int16_t CLASSIC_DEX[] = {{ {", ".join(map(str, CLASSIC))} }};',
          f'#define NUM_CLASSIC_DEX {len(CLASSIC)}', '',
          '// Where an egg may come from.',
          'struct RegionInfo {', '  const char *name;', '  uint16_t lo, hi;', '  const int16_t *starters;', '  uint8_t starterCount;', '};']
    for n, _lo, _hi, st in REGIONS:
        o.append(f'static const int16_t REGION_START_{n}[] = {{ {", ".join(map(str, st))} }};')
    o += [f'static const int16_t REGION_START_ALL[] = {{ {", ".join(map(str, ALL_STARTERS))} }};',
          f'#define REGION_COUNT {len(REGIONS) + 1}', f'#define REGION_ALL {len(REGIONS)}',
          'static const RegionInfo REGIONS[REGION_COUNT] = {']
    for n, lo, hi, st in REGIONS:
        o.append(f'  {{ "{n}", {lo}, {hi}, REGION_START_{n}, {len(st)} }},')
    o += [f'  {{ "WORLD", 1, DEX_COUNT, REGION_START_ALL, {len(ALL_STARTERS)} }},', '};', '']
    write('dex.h', o)


def learnset(a):
    """Level-up moves from the animal's own traits, plus body-plan extras."""
    by_type = {}
    for name, m in zip(MOVE_ID, MOVES):
        if m[3] > 0 and name not in ('STRUGGLE', 'OVERREACH'):
            by_type.setdefault(m[1], []).append(name)
    for k in by_type:
        by_type[k].sort(key=lambda n: MOVES[MOVE_ID[n]][3])
    t1 = by_type[a['type1']]
    t2 = by_type.get(a['type2'], by_type['STRENGTH'] if a['type1'] != 'STRENGTH' else by_type['SPEED'])
    arch = a['arch']
    support = {'cat': 'WAR DANCE', 'dog': 'HUNT CALL', 'primate': 'FOCUS', 'parrot': 'FOCUS', 'bird': 'LIMBER UP', 'raptor': 'WAR DANCE',
               'bear': 'BRACE', 'turtle': 'HARDEN UP', 'armadillo': 'HARDEN UP', 'elephant': 'STAND TALL', 'hippo': 'STAND TALL',
               'snake': 'STARE DOWN', 'lizard': 'LIMBER UP', 'frog': 'FOCUS', 'antelope': 'LIMBER UP', 'zebra': 'LIMBER UP',
               'sloth': 'LONG NAP', 'manatee': 'LONG NAP', 'rhino': 'BRACE', 'bovine': 'BRACE'}.get(arch, 'SNARL')
    plan = [(t1[0], 1), ('SNARL' if h(a['name']) % 2 else 'STARE DOWN', 1), (t2[0], 7), (support, 12), (t1[min(1, len(t1) - 1)], 16),
            (t2[min(1, len(t2) - 1)], 24), ('LICK WOUNDS' if h(a['name'] + 'h') % 3 == 0 else 'DIRT CLOUD', 30),
            (t1[min(2, len(t1) - 1)], 34), (t2[min(2, len(t2) - 1)], 42), (t1[-1], 52)]
    seen, out = set(), []
    for name, lvl in plan:
        if name not in seen:
            seen.add(name)
            out.append((MOVE_ID[name], lvl))
    return out


def gen_moves():
    o = ['#pragma once', '#include <stdint.h>', '#include "dex.h"', '',
         '// GENERATED by tools/gen/gen_dex.py - do not edit', '',
         'enum : uint8_t { MC_PHYS = 0, MC_SPEC = 1, MC_STATUS = 2 };', '',
         'enum : uint8_t {', '  EF_NONE = 0,', '  EF_STAGE,       // statMask + stages on target',
         '  EF_RECOIL,      // param = denominator (3 -> 1/3 of the damage dealt)',
         '  EF_DRAIN,       // param = % of the damage dealt that is healed',
         '  EF_FIXED_LVL,   // damage = the attacker level, ignores stats', '  EF_FIXED,       // param = fixed damage',
         '  EF_PRIORITY,    // param = priority, acts first', '  EF_NEVER_MISS,  // does not roll accuracy',
         '  EF_MULTI,       // 2-5 hits, power is per hit', '  EF_HEAL,        // param = % of max vitality healed',
         '  EF_RECHARGE,    // loses the next turn, exposed', '  EF_CHARGE,      // turn 1 charges; param 1 = invulnerable meanwhile', '};',
         'enum : uint8_t { ST_ATK = 1, ST_DEF = 2, ST_SPA = 4, ST_SPD = 8, ST_SPE = 16 };',
         'enum : uint8_t { TG_SELF = 0, TG_FOE = 1 };',
         '// Status ailments live in the battle state and clear when it ends.',
         'enum : uint8_t {', '  AIL_NONE = 0, AIL_PARA = 1, AIL_BURN = 2, AIL_POISON = 3,', '  AIL_SLEEP = 4, AIL_FREEZE = 5, AIL_CONFUSE = 6,', '};', '',
         'struct MoveEntry {', '  const char *name;', '  uint8_t type;      // PowerType', '  uint8_t cat;       // MC_*',
         '  uint8_t power;     // 0 for status and fixed damage', '  uint8_t acc;       // percentage; 0 = cannot miss',
         '  uint8_t effect;    // EF_*', '  int8_t  param;     // payload of the effect', '  uint8_t statMask;  // ST_* (EF_STAGE only)',
         '  int8_t  stages;    // +/- levels (EF_STAGE only)', '  uint8_t target;    // TG_*', '  uint8_t ailment;   // AIL_*, 0 = none',
         '  uint8_t ailChance; // percent, 0 = never', '};', '',
         'enum MoveId : uint8_t {', '  MV_NONE = 0,']
    for m in MOVES[1:]:
        o.append('  MV_' + m[0].replace(' ', '_').replace('-', '_') + ',')
    o += ['};', '', f'#define MOVE_COUNT {len(MOVES)}', '', 'static const MoveEntry MOVE_TBL[MOVE_COUNT] = {']
    for i, m in enumerate(MOVES):
        o.append(f'  {{ "{m[0]}", PT_{m[1]}, {m[2]}, {m[3]}, {m[4]}, {m[5]}, {m[6]}, {m[7]}, {m[8]}, {m[9]}, {m[10]}, {m[11]} }},  // {i}')
    o += ['};', '', '// Learnsets in CSR format: species n occupies LEARN_TBL[LEARN_OFS[n] .. LEARN_OFS[n + 1]).',
          'struct LearnEntry { uint8_t move; uint8_t level; };']
    flat, ofs = [], [0, 0]
    for a in ANIMALS:
        flat += learnset(a)
        ofs.append(len(flat))
    o.append(f'static const LearnEntry LEARN_TBL[{len(flat)}] = {{')
    for i in range(0, len(flat), 8):
        o.append('  ' + ' '.join(f'{{ {mv:3d}, {lv:3d} }},' for mv, lv in flat[i:i + 8]))
    o += ['};', '', 'static const uint16_t LEARN_OFS[DEX_COUNT + 2] = {']
    for i in range(0, len(ofs), 16):
        o.append('  ' + ', '.join(map(str, ofs[i:i + 16])) + ',')
    o += ['};', '',
          'static inline uint8_t learnCount(int16_t dex) {', '  if (dex < 1 || dex > DEX_COUNT) return 0;',
          '  return (uint8_t)(LEARN_OFS[dex + 1] - LEARN_OFS[dex]);', '}', '',
          'static inline uint8_t learnMove(int16_t dex, uint8_t i) {', '  if (i >= learnCount(dex)) return MV_STRUGGLE;',
          '  return LEARN_TBL[LEARN_OFS[dex] + i].move;', '}', '',
          'static inline uint8_t learnLevel(int16_t dex, uint8_t i) {', '  if (i >= learnCount(dex)) return 0;',
          '  return LEARN_TBL[LEARN_OFS[dex] + i].level;', '}', '']
    write('moves.h', o)


# Rangers: 8 reserve wardens, the Grand 4 and the Head Ranger of each region.
# name, reserve, speciality, team size, level of the team's lead
ROSTER = {
    'AFRICA': [('AMARA', 'SERENGETI', 'SPEED', 2, 12), ('KOFI', 'OKAVANGO', 'SWIM', 2, 19), ('ZAWADI', 'KALAHARI', 'DIG', 3, 24),
               ('IMANI', 'KRUGER', 'SIZE', 3, 28), ('TENDAI', 'NAMIB', 'VENOM', 4, 36), ('ADJOA', 'CONGO', 'INTELLIGENCE', 4, 38),
               ('BAKO', 'MASAI MARA', 'STRENGTH', 4, 43), ('NEO', 'ETOSHA', 'PACK', 5, 46), ('SEFU', 'SUMMIT', 'FLIGHT', 5, 52),
               ('NIA', 'SUMMIT', 'AGILITY', 5, 53), ('JELANI', 'SUMMIT', 'ARMOR', 5, 54), ('ABENA', 'SUMMIT', 'HIDE', 5, 56),
               ('THABO', 'HEAD RANGER', 'STRENGTH', 6, 60)],
    'ASIA': [('MEI', 'SICHUAN', 'SIZE', 2, 10), ('ARJUN', 'RANTHAMBORE', 'SPEED', 3, 15), ('HANA', 'HOKKAIDO', 'FLIGHT', 3, 19),
             ('DAWA', 'TIBET', 'AGILITY', 3, 24), ('LAN', 'MEKONG', 'SWIM', 4, 28), ('RAKA', 'BORNEO', 'INTELLIGENCE', 4, 33),
             ('SARI', 'KOMODO', 'VENOM', 4, 38), ('TEMUR', 'GOBI', 'ARMOR', 5, 42), ('YUKI', 'SUMMIT', 'HIDE', 5, 46),
             ('VIKRAM', 'SUMMIT', 'STRENGTH', 5, 47), ('INDAH', 'SUMMIT', 'VENOM', 5, 48), ('BATU', 'SUMMIT', 'SIZE', 5, 50),
             ('SORA', 'HEAD RANGER', 'SPEED', 6, 54)],
    'AMERICAS': [('LUCIA', 'PANTANAL', 'SWIM', 2, 11), ('MATEO', 'PATAGONIA', 'PACK', 3, 16), ('INTI', 'ANDES', 'FLIGHT', 3, 21),
                 ('CAMILA', 'YASUNI', 'AGILITY', 3, 26), ('TUPAC', 'ATACAMA', 'DIG', 4, 31), ('RAFAEL', 'MANU', 'INTELLIGENCE', 4, 35),
                 ('VALERIA', 'CHOCO', 'VENOM', 4, 40), ('NAHUEL', 'TIERRA FUEGO', 'ARMOR', 5, 44), ('PAZ', 'SUMMIT', 'SOUND', 5, 48),
                 ('DIEGO', 'SUMMIT', 'SPEED', 5, 49), ('ISABEL', 'SUMMIT', 'HIDE', 5, 50), ('SANTOS', 'SUMMIT', 'SIZE', 5, 52),
                 ('AURORA', 'HEAD RANGER', 'STRENGTH', 6, 58)],
}


def team(region, lo, hi, who, ty, count, lead):
    pool = [a for a in ANIMALS if lo <= a['id'] <= hi]
    strong = lead >= 40
    cands = [a for a in pool if ty in (a['type1'], a['type2']) and (a['rarity'] < 4 or strong)]
    rest = [a for a in pool if a not in cands and a['rarity'] < 4]
    cands.sort(key=lambda a: (a['rarity'] if strong else -a['rarity'], h(who + a['name'])), reverse=True)
    rest.sort(key=lambda a: h(who + a['name']))
    picks = (cands + rest)[:count]
    out = []
    for i, a in enumerate(picks):
        level = lead - (count - 1 - i) * 2
        out.append((a['id'], max(5, level)))
    return out


def gen_trainers():
    o = ['#pragma once', '#include <stdint.h>', '#include "dex.h"', '',
         '// GENERATED by tools/gen/gen_dex.py - do not edit', '//',
         '// The ranger ladders: 8 reserve wardens, the Grand 4 and the Head Ranger of each region.', '',
         '#define TRAINER_TEAM_MAX 6', '#define MAX_TRAINER_LEVEL 100', '',
         'struct TrainerMon {', '  uint16_t dex;', '  uint8_t level;', '};', '',
         'struct Trainer {', '  const char *name;', '  const char *place;   // the reserve, or the Grand 4 summit',
         '  uint8_t type;        // the type they specialise in, for the UI accent', '  uint8_t count;',
         '  TrainerMon team[TRAINER_TEAM_MAX];', '};', '',
         '// index 0-7 are the stamps, 8-11 the Grand 4, 12 the Head Ranger', '#define TRAINER_COUNT 13', '#define TRAINER_GYMS 8', '#define TRAINER_ELITE4 4', '',
         'struct TrainerSet {', '  const Trainer *list;', '  const char *region;', '};', '']
    for rn, lo, hi, _st in REGIONS:
        o.append(f'static const Trainer TRAINERS_{rn}[TRAINER_COUNT] = {{')
        for who, place, ty, count, lead in ROSTER[rn]:
            mons = team(rn, lo, hi, who, ty, count, lead)
            body = ','.join('{%d,%d}' % m for m in mons)
            o.append(f'  {{ "{who}", "{place}", PT_{ty}, {count}, {{ {body} }} }},')
        o += ['};', '']
    o.append(f'#define GYM_REGIONS {len(REGIONS)}')
    o.append('static const TrainerSet TRAINER_SETS[GYM_REGIONS] = {')
    for rn, *_ in REGIONS:
        o.append(f'  {{ TRAINERS_{rn}, "{rn}" }},')
    o += ['};', '', '#define HARD_IV 31', '#define EASY_IV 16', '']
    write('trainers.h', o)


def write(name, lines):
    with open(os.path.join(ROOT, name), 'w', newline='\n', encoding='utf-8') as f:
        f.write('\n'.join(lines))
    print('wrote', name)


if __name__ == '__main__':
    gen_dex()
    gen_moves()
    gen_trainers()
