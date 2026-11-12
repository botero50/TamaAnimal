// Growth stages: every animal hatches as a cub, grows to a juvenile and then an adult,
// on the same rules evolution had (stats >= 40, delayed by care mistakes and by an
// early retire), and keeps its stage through saves, the team and battles. A save from
// before growth stages must load as an adult, never shrink.
#include "Arduino.h"
#include "Preferences.h"
#include "pet.h"
#include "party.h"
#include "battle.h"
#include "dex.h"
#include <cstdio>
#include <cstring>

uint32_t g_seed = 0x6B0D;
FakeSerial Serial;
FakeESP ESP;
FakeWire Wire;
volatile int g_touchX = 0, g_touchY = 0;
volatile bool g_touchDown = false;
bool wasPressed = false;
static uint32_t gNow = 1;
uint32_t millis() { return gNow; }
void FakeESP::restart() { exit(0); }
int FakeSerial::available() { return 0; }
String FakeSerial::readStringUntil(char) { return String(""); }
void sfxPlay(uint8_t) {}

static int bad = 0;
static void ck(bool ok, const char *what) { printf("%s  %s\n", ok ? "PASS" : "FAIL", what); if (!ok) bad++; }

static void atLevel(Pet &p, int lvl) { p.ageMinutes = (uint32_t)(lvl - 1) * MINUTES_PER_LEVEL; }
static void cared(Pet &p) { p.fullness = p.joy = p.energy = p.hygiene = 90; p.careMistakes = 0; }

int main() {
  Pet p;
  p.begin();
  p.dbgHatchAs(1, false);
  cared(p);
  ck(p.growth == GROW_CUB, "an animal hatches as a cub");

  atLevel(p, GROW_JUV_LEVEL - 1);
  ck(!p.canEvolveNow(), "a cub does not grow before its level");
  atLevel(p, GROW_JUV_LEVEL);
  ck(p.canEvolveNow() && p.wantEvolveButton(), "and is offered to grow up at it");
  p.fullness = 20;
  ck(!p.canEvolveNow(), "but not while a need is below 40");
  cared(p);
  p.declineEvolve();
  ck(!p.wantEvolveButton(), "'not yet' holds the offer back");
  atLevel(p, GROW_JUV_LEVEL + 1);
  ck(p.wantEvolveButton(), "and it comes back at the next level");

  int16_t sp = p.speciesId;
  p.evolve();
  ck(p.growth == GROW_JUV && p.prevGrowth == GROW_CUB, "growing up makes it a juvenile");
  ck(p.speciesId == sp, "and it stays the same species");
  ck(!p.canEvolveNow(), "a juvenile waits for the next stage");
  atLevel(p, GROW_ADULT_LEVEL);
  ck(p.canEvolveNow(), "and grows again at its level");
  p.evolve();
  ck(p.growth == GROW_ADULT && p.speciesId == sp, "into an adult of the same species");
  atLevel(p, 70);
  ck(!p.canEvolveNow() && p.nextGrowLevel() == 0, "an adult never grows further");

  // the delays evolution had
  {
    Pet q; q.begin(); q.dbgHatchAs(2, false); cared(q);
    q.careMistakes = 3;
    atLevel(q, GROW_JUV_LEVEL + 2);
    ck(!q.canEvolveNow(), "care mistakes delay growing up");
    atLevel(q, GROW_JUV_LEVEL + 3);
    ck(q.nextGrowLevel() == GROW_JUV_LEVEL + 3 && q.canEvolveNow(), "by one level each");
  }

  // saves
  {
    Pet a; a.begin(); a.dbgHatchAs(3, false); cared(a);
    atLevel(a, GROW_JUV_LEVEL); a.evolve();   // growing up saves
    Pet b; b.begin();
    ck(b.speciesId == 3 && b.growth == GROW_JUV, "the stage survives a restart");
    Preferences pr; pr.begin("tamanimals", false); pr.remove("grow"); pr.end();
    Pet c; c.begin();
    ck(c.growth == GROW_ADULT, "a save from before growth stages loads as an adult");
  }

  // the team
  {
    Pet a; a.begin(); a.dbgHatchAs(4, false); cared(a);
    a.release();                                 // a farewell banks it, the way the firmware does
    gNow += CEREMONY_MS + 1000;
    a.update(gNow);
    ck(a.endedKind != CER_NONE && a.endedMon.dex == 4 && a.endedMon.growth == GROW_CUB, "a banked cub stays a cub");
    party.begin();
    party.replaceAt(0, a.endedMon);
    party.save();
    party.begin();
    ck(party.slots[0].dex == 4 && party.slots[0].growth == GROW_CUB, "and keeps its stage in the saved team");
    Combatant cb; combatantFromParty(cb, party.slots[0]);
    ck(cb.growth == GROW_CUB, "and fights as a cub");
    Combatant cp; combatantFromPet(cp, p);
    ck(cp.growth == GROW_ADULT, "the pet fights at its own stage");
    Pet r; r.begin(); r.reviveFrom(party.slots[0]);
    ck(r.growth == GROW_CUB, "and coming back from the team keeps it");

    // an older team, saved before growth existed: shorter records
    const size_t oldStride = offsetof(PartyMon, growth);
    uint8_t blob[sizeof(PartyMon) * PARTY_SLOTS];
    memset(blob, 0, sizeof(blob));
    for (int i = 0; i < PARTY_SLOTS; i++) {
      PartyMon m; m.dex = 10 + i; m.level = 30; m.growth = GROW_CUB;   // growth must NOT carry over
      memcpy(blob + i * oldStride, &m, oldStride);
    }
    Preferences pr; pr.begin("tamanimals", false); pr.putBytes("party", blob, oldStride * PARTY_SLOTS); pr.end();
    party.begin();
    bool adults = true, kept = true;
    for (int i = 0; i < PARTY_SLOTS; i++) {
      if (party.slots[i].growth != GROW_ADULT) adults = false;
      if (party.slots[i].dex != 10 + i) kept = false;
    }
    ck(kept, "an older team still loads every member");
    ck(adults, "as adults");
  }

  if (bad) { printf("FAILURES: %d\n", bad); return 1; }
  printf("growth test passed\n");
  return 0;
}
