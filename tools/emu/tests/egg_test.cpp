// Eggs never hold an animal the field guide already has while a new one is left in the tier.
// One rule (Pet::eggRoll) serves random eggs, the region switch, the check just before an egg
// cracks, and the Nursery's eggs. Ported from the engine this grew out of; the animals are
// picked from the field guide's own data, so the test does not depend on particular numbers.
#include "Arduino.h"
#include "Preferences.h"
#include "pet.h"
#include "party.h"
#include "noart.h"
#include <cstdio>
#include <vector>
uint32_t g_seed = 53; FakeSerial Serial; FakeESP ESP; FakeWire Wire;
volatile int g_touchX = 0, g_touchY = 0; volatile bool g_touchDown = false; bool wasPressed = false;
static uint32_t g_ms = 0;
uint32_t millis() { return g_ms; } void FakeESP::restart() { exit(0); }
int FakeSerial::available() { return 0; } String FakeSerial::readStringUntil(char) { return String(""); }
void sfxPlay(uint8_t) {}
static int bad = 0;
static void ck(bool ok, const char *w) { printf("%s  %s\n", ok ? "PASS" : "FAIL", w); if (!ok) bad++; }

static const uint32_t DAY = 86400;
static void reg(Pet &p, int d) { p.dexReg[(d - 1) >> 3] |= (uint8_t)(1 << ((d - 1) & 7)); }
static void unreg(Pet &p, int d) { p.dexReg[(d - 1) >> 3] &= (uint8_t)~(1 << ((d - 1) & 7)); }
static void fresh(Pet &p) {
  p.begin();
  p.dbgHatchAs(1, false);
  p.sleeping = false;
  p.lastSeenEpoch = 20100 * DAY + 12 * 3600;
  p.lastEnd = CER_FAREWELL;
  for (int d = 1; d <= DEX_COUNT; d++) reg(p, d);      // start from a complete field guide
}
static bool allowed(const Pet &p, int d) { return !p.isRegistered(d) || p.lineHasUnregistered(d); }
// the first n animals of a tier in a region (all of them have art: every animal is drawn)
static std::vector<int> pickIn(uint8_t region, uint8_t tier, int n) {
  std::vector<int> out;
  for (int d = REGIONS[region].lo; d <= REGIONS[region].hi && (int)out.size() < n; d++)
    if (DEX_TBL[d].rarity == tier && speciesHasArt(d)) out.push_back(d);
  return out;
}

int main() {
  const std::vector<int> missing = pickIn(0, R_COMUN, 3);     // three common animals of the first region
  const int other = pickIn(0, R_COMUN, 4)[3];                  // one more, which stays in the guide
  const uint8_t r2 = 1;                                        // a second region
  const int newInR2 = pickIn(r2, R_COMUN, 1)[0];
  ck(missing.size() == 3 && other && newInR2, "the test found its animals in the field guide");

  // the rule is respected by every random egg
  {
    Pet p; fresh(p); p.setRegion(0);
    for (int d : missing) unreg(p, d);
    bool ok = true; int hits = 0;
    for (int i = 0; i < 2000; i++) {
      int16_t e = p.pickEggSpecies();
      if (!allowed(p, e)) ok = false;
      for (int d : missing) if (e == d) hits++;
    }
    ck(ok, "2000 random eggs: never an animal the field guide has while a new one is left");
    ck(hits > 1500, "and nearly all of them are the few that are missing (common tier)");
  }
  // only a new animal is offered while one exists
  {
    Pet p; fresh(p);
    unreg(p, missing[0]);
    bool onlyNew = true;
    for (int i = 0; i < 400; i++) if (p.eggRoll(0, R_COMUN) != missing[0]) onlyNew = false;
    ck(onlyNew, "the one new animal is the egg, every time");
  }
  // a finished field guide still hatches something
  {
    Pet p; fresh(p);
    bool ok = true;
    for (int i = 0; i < 300; i++) { int16_t e = p.pickEggSpecies(); if (e < 1 || e > DEX_COUNT) ok = false; }
    ck(ok, "a complete field guide still gives an egg");
  }
  // changing region while an egg waits follows the same rule
  {
    Pet p; fresh(p);
    unreg(p, newInR2);
    bool ok = true;
    for (int i = 0; i < 300; i++) if (p.rollInRegion(r2, R_RARO) != newInR2) ok = false;
    ck(ok, "a region switch rolls the animal the field guide lacks, not a repeat");
  }
  // just before it cracks, an egg that has become a repeat is rolled again
  {
    Pet p; fresh(p); p.setRegion(0);
    unreg(p, missing[0]);
    p.dbgSetEgg(other, false);                         // an animal already in the guide
    p.recheckEgg();
    ck(p.eggPeek() == missing[0], "a repeat becomes the animal that is new");
    Pet q; fresh(q); q.setRegion(0);
    q.dbgSetEgg(other, false);
    q.recheckEgg();
    ck(q.eggPeek() == other, "with the whole field guide complete an egg is left as it is");
    Pet s; fresh(s); s.setRegion(0);
    unreg(s, missing[0]);
    s.dbgSetEgg(other, true);
    s.recheckEgg();
    ck(s.eggPeek() == other, "the first game's starter pick is never changed");
  }
  // the Nursery: a random animal with the same rule, never a legendary
  {
    bool ok = true, notLeg = true; int sawNew = 0;
    for (int i = 0; i < 300; i++) {
      Pet p; fresh(p); p.setRegion(0);
      for (int d : missing) unreg(p, d);
      p.nursery = NurseryState();
      p.nursery.aDex = missing[0] == 1 ? 2 : 1; p.nursery.bDex = other;
      p.nursery.aIv[0] = 30; p.nursery.bIv[0] = 11;
      p.nursery.since = p.lastSeenEpoch - NURSERY_SECS;
      if (!p.nurseryCollect()) { ok = false; break; }
      int16_t e = p.nursery.eggDex;
      if (!allowed(p, e)) ok = false;
      if (DEX_TBL[e].rarity == R_LEGENDARIO || DEX_TBL[e].rarity == R_EVO) notLeg = false;
      for (int d : missing) if (e == d) sawNew++;
    }
    ck(ok, "a Nursery egg follows the same rule");
    ck(notLeg, "and is a common or rare animal, never a legendary");
    ck(sawNew > 250, "and is nearly always one of the animals still missing");
  }
  if (bad) { printf("FAILURES: %d\n", bad); return 1; }
  printf("egg test passed\n");
  return 0;
}
