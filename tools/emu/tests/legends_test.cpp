// The Legends region (1.3.0): 60 myths and legends, the Spirit type, and a save written
// before the region existed, whose WORLD index now belongs to LEGENDS.
#include "Arduino.h"
#include "Preferences.h"
#include "pet.h"
#include "party.h"
#include "moves.h"
#include "types.h"
#include "cries.h"
#include <cstdio>
#include <cstring>
uint32_t g_seed = 29; FakeSerial Serial; FakeESP ESP; FakeWire Wire;
volatile int g_touchX = 0, g_touchY = 0; volatile bool g_touchDown = false; bool wasPressed = false;
static uint32_t g_ms = 0;
uint32_t millis() { return g_ms; } void FakeESP::restart() { exit(0); }
int FakeSerial::available() { return 0; } String FakeSerial::readStringUntil(char) { return String(""); }
void sfxPlay(uint8_t) {}
static int bad = 0;
static void ck(bool ok, const char *w) { printf("%s  %s\n", ok ? "PASS" : "FAIL", w); if (!ok) bad++; }

int main() {
  // the region itself
  const uint8_t LG = 3;
  ck(DEX_COUNT == 408, "the field guide holds the 325 animals, the 60 legends and the 23 memes");
  ck(!strcmp(REGIONS[LG].name, "LEGENDS") && REGIONS[LG].lo == 326 && REGIONS[LG].hi == 385, "region 3 is LEGENDS, 326-385");
  ck(REGION_ALL == REGION_COUNT - 1 && !strcmp(REGIONS[REGION_ALL].name, "WORLD") && REGIONS[REGION_ALL].hi == DEX_COUNT,
     "WORLD is still the last region and spans everything");
  ck(!strcmp(TRAINER_SETS[LG].region, "LEGENDS"), "the Legends rangers have their ladder");
  // and the Memes region after it (1.4.0)
  ck(!strcmp(REGIONS[4].name, "MEMES") && REGIONS[4].lo == 386 && REGIONS[4].hi == 408 && GYM_REGIONS == 5 &&
     !strcmp(TRAINER_SETS[4].region, "MEMES"), "region 4 is MEMES, 386-408, with its own rangers");
  bool memesOk = true;
  for (int d = 386; d <= 408; d++) if (learnCount(d) < 4 || CRY_MON[d - 1].base >= CRY_BASES) memesOk = false;
  ck(memesOk, "every meme has moves and a call");
  bool startersIn = true;
  for (int i = 0; i < REGIONS[LG].starterCount; i++)
    if (REGIONS[LG].starters[i] < 326 || REGIONS[LG].starters[i] > 385 || DEX_TBL[REGIONS[LG].starters[i]].rarity != R_COMUN) startersIn = false;
  ck(startersIn, "its starters are common legends of the region");
  bool teamsIn = true;
  for (int t = 0; t < TRAINER_COUNT; t++)
    for (int m = 0; m < TRAINER_SETS[LG].list[t].count; m++)
      if (TRAINER_SETS[LG].list[t].team[m].dex < 326) teamsIn = false;
  ck(teamsIn, "every Legends ranger fields legends only");

  // the Spirit type
  ck(!strcmp(typeName(PT_SPIRIT), "SPIRIT") && TYPE_COUNT == 14, "SPIRIT is the 14th type, with a name");
  ck(typeEffPct(PT_PACK, PT_SPIRIT, T_NONE) == 200 && typeEffPct(PT_INTELLIGENCE, PT_SPIRIT, T_NONE) == 200,
     "a pack and a clever mind beat a spirit");
  ck(typeEffPct(PT_STRENGTH, PT_SPIRIT, T_NONE) == 50 && typeEffPct(PT_SPEED, PT_SPIRIT, T_NONE) == 50,
     "brute force and speed pass through it");
  ck(typeEffPct(PT_SPIRIT, PT_SOUND, T_NONE) == 200 && typeEffPct(PT_SPIRIT, PT_ARMOR, T_NONE) == 50,
     "a spirit beats Sound and is resisted by Armor");
  int spirits = 0;
  bool learnsOwn = true, movesOk = true, calls = true;
  for (int d = 326; d <= 385; d++) {
    if (DEX_TBL[d].type1 == PT_SPIRIT || DEX_TBL[d].type2 == PT_SPIRIT) {
      spirits++;
      bool has = false;
      for (int i = 0; i < learnCount(d); i++) if (MOVE_TBL[learnMove(d, i)].type == PT_SPIRIT) has = true;
      if (!has) learnsOwn = false;
    }
    if (learnCount(d) < 4) movesOk = false;
    for (int i = 0; i < learnCount(d); i++) if (learnMove(d, i) >= MOVE_COUNT || learnMove(d, i) == MV_NONE) movesOk = false;
    if (CRY_MON[d - 1].base >= CRY_BASES) calls = false;
  }
  ck(spirits >= 12, "the ghosts and spirits of the legends are Spirit type");
  ck(learnsOwn, "and every one of them learns a Spirit move");
  ck(movesOk, "every legend learns at least four real moves");
  ck(calls, "every legend has a call");
  int leg = 0, rare = 0, com = 0;
  for (int d = 326; d <= 385; d++) { uint8_t r = DEX_TBL[d].rarity; if (r == R_LEGENDARIO) leg++; else if (r == R_RARO) rare++; else com++; }
  ck(com >= 25 && leg <= 15, "the region is mostly common legends, like the others");

  // eggs from the region, once its pack is on the card
  {
    Pet p; p.begin(); p.dbgHatchAs(1, false);
    bool inRegion = true;
    for (int i = 0; i < 300; i++) { int16_t e = p.eggRoll(LG, R_COMUN); if (e < 326 || e > 385) inRegion = false; }
    ck(inRegion, "a Legends egg holds a legend");
  }

  // a save from before the region: WORLD was index 3, which is LEGENDS now
  {
    Pet p; p.begin(); p.dbgHatchAs(1, false); p.setRegion(1); p.renameTrainer("ANA");   // save() runs
    Preferences raw; raw.begin("tamanimals", false);
    ck(raw.getUChar("rlay", 0) == REGION_ALL, "a new save records where WORLD sits");
    raw.putUChar("reg", 3);                       // the old build's WORLD
    raw.remove("rlay");
    int16_t old[4] = { 0, 0, 0, 201 };            // and the egg it remembered for WORLD
    raw.putBytes("eggR", old, sizeof(old));
    raw.end();
    Pet q; q.begin();
    ck(q.region == REGION_ALL, "an old save's WORLD loads as WORLD, not as LEGENDS");
    ck(q.eggByRegion[REGION_ALL] == 201 && q.eggByRegion[LG] == 0, "and its remembered egg moves with it");
  }
  // a save from 1.3.x: WORLD was index 4, which is MEMES now
  {
    Pet p; p.begin(); p.dbgHatchAs(1, false); p.setRegion(1); p.renameTrainer("ANA");
    Preferences raw; raw.begin("tamanimals", false);
    raw.putUChar("reg", 4);
    raw.putUChar("rlay", 4);
    int16_t old[5] = { 0, 0, 0, 0, 202 };
    raw.putBytes("eggR", old, sizeof(old));
    raw.end();
    Pet q; q.begin();
    ck(q.region == REGION_ALL && q.eggByRegion[REGION_ALL] == 202 && q.eggByRegion[4] == 0,
       "a 1.3 save's WORLD loads as WORLD, not as MEMES, its egg with it");
  }
  {
    Pet p; p.begin(); p.dbgHatchAs(1, false); p.setRegion(LG); p.renameTrainer("ANA");
    Pet q; q.begin();
    ck(q.region == LG, "a new save that chose LEGENDS keeps LEGENDS");
  }
  {
    Pet p; p.begin(); p.dbgHatchAs(1, false); p.setRegion(1); p.renameTrainer("ANA");
    Preferences raw; raw.begin("tamanimals", false);
    raw.remove("rlay"); raw.end();
    Pet q; q.begin();
    ck(q.region == 1, "an old save in a real region keeps its region");
  }

  if (bad) { printf("FAILURES: %d\n", bad); return 1; }
  printf("legends test passed\n");
  return 0;
}
