// The face the creature wears (Pet::look): the most urgent need first, in the same order as the
// line under it on the main screen, so the face and the words never disagree.
#include "Arduino.h"
#include "Preferences.h"
#include "pet.h"
#include "party.h"
#include "faces.h"
#include <cstdio>
uint32_t g_seed = 11; FakeSerial Serial; FakeESP ESP; FakeWire Wire;
volatile int g_touchX = 0, g_touchY = 0; volatile bool g_touchDown = false; bool wasPressed = false;
static uint32_t g_ms = 0;
uint32_t millis() { return g_ms; } void FakeESP::restart() { exit(0); }
int FakeSerial::available() { return 0; } String FakeSerial::readStringUntil(char) { return String(""); }
void sfxPlay(uint8_t) {}
static int bad = 0;
static void ck(bool ok, const char *w) { printf("%s  %s\n", ok ? "PASS" : "FAIL", w); if (!ok) bad++; }

static void fresh(Pet &p) {
  p.begin();
  p.dbgHatchAs(1, false);
  p.sleeping = false;
  p.lastSeenEpoch = 20100 * 86400u + 12 * 3600;
  p.lastCareEpoch = p.lastSeenEpoch;       // just looked after: not lonely
  p.fullness = p.joy = p.energy = p.hygiene = 70;
  p.sickness = SICK_NONE;
  p.grumpy = false;
}

int main() {
  Pet p;
  fresh(p);
  ck(p.look() == LOOK_CONTENT, "all needs met: content");
  ck(p.mood() == MOOD_HAPPY, "and its mood is happy, so it roams");

  fresh(p); p.joy = 95; p.fullness = p.energy = p.hygiene = 80;
  ck(p.look() == LOOK_HAPPY, "very happy and well cared for: the happy face");
  fresh(p); p.joy = 95; p.hygiene = 40;
  ck(p.look() == LOOK_CONTENT, "very happy but a bit grubby: just content");

  fresh(p); p.joy = 10;
  ck(p.look() == LOOK_SAD, "joy low: sad");
  fresh(p); p.energy = 10;
  ck(p.look() == LOOK_TIRED, "energy low: tired");
  fresh(p); p.hygiene = 10;
  ck(p.look() == LOOK_DIRTY, "hygiene low: dirty");
  fresh(p); p.fullness = 10;
  ck(p.look() == LOOK_HUNGRY, "food low: hungry");
  fresh(p); p.sickness = SICK_COLD;
  ck(p.look() == LOOK_SICK, "a cold: sick");
  fresh(p); p.sickness = SICK_STOMACH;
  ck(p.look() == LOOK_SICK, "a stomach ache: sick");

  // the order when several needs are low at once
  fresh(p); p.sickness = SICK_COLD; p.fullness = p.hygiene = p.energy = p.joy = 5;
  ck(p.look() == LOOK_SICK, "sick beats everything");
  p.sickness = SICK_NONE;
  ck(p.look() == LOOK_HUNGRY, "then hunger");
  p.fullness = 70;
  ck(p.look() == LOOK_DIRTY, "then dirt");
  p.hygiene = 70;
  ck(p.look() == LOOK_TIRED, "then tiredness");
  p.energy = 70;
  ck(p.look() == LOOK_SAD, "then sadness");

  // every need face is one the old mood also called sad, so an old card still shows its pained look
  for (int need = 0; need < 4; need++) {
    fresh(p);
    uint8_t *v[] = { &p.fullness, &p.hygiene, &p.energy, &p.joy };
    *v[need] = 5;
    if (p.mood() != MOOD_SAD) { ck(false, "a low need is a sad mood too"); break; }
  }
  ck(true, "each low need also reads as a sad mood");

  fresh(p); p.grumpy = true;
  ck(p.look() == LOOK_SAD, "grumpy after a night with the light on: sad");

  // the built-in drawings (a board without a card) have every face, and each stays on the drawing
  {
    bool all = true, inside = true;
    for (int sp = 0; sp < 9; sp++)
      for (int f = 0; f < FACE_COUNT; f++) {
        const uint8_t *q = SPECIES_FACES[sp][f];
        if (q[0] == 0) all = false;
        for (int i = 0; i < q[0]; i++) if (q[1 + i * 3] >= 32 || q[2 + i * 3] >= 32) inside = false;
      }
    ck(all, "each of the nine built-in animals has every face");
    ck(inside, "and every face pixel is inside its 32x32 drawing");
  }

  if (bad) { printf("FAILURES: %d\n", bad); return 1; }
  printf("mood test passed\n");
  return 0;
}
