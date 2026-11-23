// The Mine (mine.h and the Pet's dig / amber / fossil state).
#include "Arduino.h"
#include "Preferences.h"
#include "pet.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
uint32_t g_seed = 31; FakeSerial Serial; FakeESP ESP; FakeWire Wire;
volatile int g_touchX = 0, g_touchY = 0; volatile bool g_touchDown = false; bool wasPressed = false;
static uint32_t g_ms = 0; uint32_t millis() { return g_ms; }
void FakeESP::restart() { exit(0); }
int FakeSerial::available() { return 0; } String FakeSerial::readStringUntil(char) { return String(""); }
void sfxPlay(uint8_t) {}
static int bad = 0;
static void ck(bool ok, const char *w) { printf("%s  %s\n", ok ? "PASS" : "FAIL", w); if (!ok) bad++; }
static const uint32_t DAY = 86400, D0 = 20000 * DAY + 12 * 3600;

int main() {
  // taps: strong types and a strong arm break the rock sooner
  ck(mineTapsFor(PT_DIG, PT_NONE, 0) == 8 && mineTapsFor(PT_STRENGTH, PT_NONE, 0) == 8, "diggers and the strong: 8 taps");
  ck(mineTapsFor(PT_ARMOR, PT_NONE, 0) == 10 && mineTapsFor(PT_SIZE, PT_NONE, 0) == 10, "armor and size: 10");
  ck(mineTapsFor(PT_SWIM, PT_NONE, 0) == 14 && mineTapsFor(PT_FLIGHT, PT_SOUND, 0) == 14, "the rest: 14");
  ck(mineTapsFor(PT_ARMOR, PT_DIG, 0) == 8 && mineTapsFor(PT_SWIM, PT_STRENGTH, 0) == 8, "the better of the two talents counts");
  ck(mineTapsFor(PT_SWIM, PT_NONE, 120) == 11 && mineTapsFor(PT_DIG, PT_NONE, 400) == 4 && mineTapsFor(PT_SWIM, PT_NONE, 4000) == 4, "attack saves taps, never under 4");
  // the loot table by roll
  int cnt[5] = { 0 };
  for (int r = 0; r < 100; r++) cnt[mineLootKind((uint8_t)r)]++;
  ck(cnt[ML_FOSSIL] == 3 && cnt[ML_AMBER] == 25 && cnt[ML_COINS] == 45 && cnt[ML_SEEDS] == 15 && cnt[ML_SNACK] == 12, "loot odds: 3 % fossil, 25 % amber, 45 % coins, 15 % seeds, 12 % snack");
  bool coinsOk = true; for (int r = 0; r < 256; r++) { int c = mineCoinsFor((uint8_t)r); if (c < 2 || c > 5) coinsOk = false; }
  ck(coinsOk, "a coin find is 2 to 5 coins");

  // fossils
  ck(FOSSIL_COUNT == 32 && isFossilDex(132) && isFossilDex(239) && isFossilDex(255) && !isFossilDex(144) && !isFossilDex(1) && !isFossilDex(0), "32 prehistoric animals");
  bool all = true; for (int i = 0; i < FOSSIL_COUNT; i++) { int16_t d = fossilAt(i); if (!isFossilDex(d) || d < 1 || d > DEX_COUNT) all = false; }
  ck(all, "every pick is one of them");
  // amber
  AmberResult s = { 0, false };
  for (int i = 0; i < 9; i++) { s = mineAddAmber(s.amber, true); ck(!s.fossil, "no fossil before the tenth piece"); if (bad) break; }
  s = mineAddAmber(s.amber, true);
  ck(s.fossil && s.amber == 0, "ten pieces hold exactly one fossil");
  AmberResult w = mineAddAmber(9, false);
  ck(!w.fossil && w.amber == 9, "while one waits, the amber waits at nine");
  // digs a day
  ck(mineDigsLeft(5, 0, 5) == 5 && mineDigsLeft(5, 3, 5) == 2 && mineDigsLeft(5, 5, 5) == 0 && mineDigsLeft(5, 9, 5) == 0, "digs left today");
  ck(mineDigsLeft(4, 5, 5) == 5, "a new day starts again");

  // a companion
  {
    Pet q; q.begin(); q.dbgHatchAs(18, false); q.setClock(D0);   // an aardvark: a digger
    ck(q.mineTaps() <= 8, "a digger needs 8 taps at most");
    ck(q.canMine() && q.mineDigsLeft() == MINE_DIGS_PER_DAY, "fresh: five digs");
    q.energy = 50;
    Pet::MineFind a = q.mineDig();
    ck(q.energy == 50 - MINE_ENERGY_COST && q.mineDigsLeft() == MINE_DIGS_PER_DAY - 1, "a dig costs 10 energy and one of the five");
    ck((int)a.kind >= 0 && (int)a.kind <= ML_FOSSIL, "and finds something");
    q.energy = 5;
    ck(!q.canMine(), "too tired: no dig");
    uint8_t before = q.mineDigsLeft(); Pet::MineFind none = q.mineDig();
    ck(q.mineDigsLeft() == before && q.energy == 5 && none.coins == 0, "a refused dig costs nothing");
    q.energy = 100;
    int digs = 1;
    while (q.canMine()) { q.mineDig(); q.energy = 100; digs++; }
    ck(digs == MINE_DIGS_PER_DAY && q.mineDigsLeft() == 0, "five digs, then it stops");
    q.setClock(D0 + DAY);
    ck(q.mineDigsLeft() == MINE_DIGS_PER_DAY && q.canMine(), "next day: five again");
    q.sleeping = true; ck(!q.canMine(), "not while asleep"); q.sleeping = false;
  }

  // loot over many days: the rates come out near the table, a fossil is kept once
  {
    Pet q; q.begin(); q.dbgHatchAs(18, false); q.setClock(D0);
    int kinds[5] = { 0 }, n = 0, fossils = 0; bool amberOk = true;
    for (int d = 0; d < 400; d++) {
      q.setClock(D0 + (d + 1) * DAY);
      for (int i = 0; i < MINE_DIGS_PER_DAY; i++) {
        q.energy = 100;
        Pet::MineFind r = q.mineDig(); kinds[r.kind]++; n++; if (r.fossil || r.kind == ML_FOSSIL) fossils++;
        if (q.mineAmber >= AMBER_PER_FOSSIL) amberOk = false;
        if (r.kind == ML_COINS) ck(r.coins <= 5, "coins within range");
      }
      if (bad) break;
    }
    ck(n == 2000, "2000 digs");
    ck(kinds[ML_AMBER] > 2000 * 20 / 100 && kinds[ML_AMBER] < 2000 * 32 / 100, "about a quarter are amber");
    ck(kinds[ML_COINS] > 2000 * 38 / 100 && kinds[ML_COINS] < 2000 * 52 / 100, "about 45 % coins");
    ck(amberOk, "amber stays under ten");
    ck(fossils == 1 && q.mineAmber == AMBER_PER_FOSSIL - 1, "one fossil while it waits, and the amber waits at nine");
    ck(isFossilDex((int16_t)q.fossilDex), "the waiting fossil is a fossil species");
  }

  // the fossil egg
  {
    Pet q; q.begin(); q.dbgHatchAs(18, false); q.setClock(D0);
    q.fossilDex = 132;
    q.newEgg();
    ck(q.isEgg() && q.eggPeek() == 132 && q.eggFossil, "the next egg is the fossil");
    q.saveNow();
    Pet r; r.begin();
    ck(r.isEgg() && r.eggPeek() == 132 && r.eggFossil && r.fossilDex == 132, "it survives a save");
    q.setRegion(2);
    ck(q.eggPeek() == 132, "a region change does not reroll it");
    q.dbgHatchAs(q.eggPeek(), false);
    ck(!q.isEgg() && q.speciesId == 132 && q.fossilDex == 0 && !q.eggFossil, "it hatches a sabertooth and the fossil is used up");
    q.newEgg();
    ck(q.eggPeek() != 132 || !q.eggFossil, "the next egg is random again");
    // a first-game starter pick never takes the fossil
    Pet t; t.begin(); t.fossilDex = 131; t.newEgg();
    ck(!t.eggFossil || !t.awaitingStarter(), "no fossil while the player picks a starter");
  }

  // save round trip
  {
    Pet q; q.begin(); q.dbgHatchAs(18, false); q.setClock(D0);
    q.energy = 100; q.mineDig(); q.mineAmber = 7; q.fossilDex = 241;
    q.saveNow();
    Pet r; r.begin();
    ck(r.mineDay == q.mineDay && r.mineToday == 1 && r.mineAmber == 7 && r.fossilDex == 241, "digs, amber and fossil survive a save");
    r.setClock(D0);
    ck(r.mineDigsLeft() == MINE_DIGS_PER_DAY - 1, "and the day's count with them");
  }

  if (bad) { printf("FAILURES: %d\n", bad); return 1; }
  printf("mine test passed\n");
  return 0;
}
