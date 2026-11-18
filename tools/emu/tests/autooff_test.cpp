// The screen turns itself off when nobody touches it. It used to stop at "nearly off" and keep
// drawing forever -- only PWR turned it off -- so an idle device drained its battery.
#include "Arduino.h"
#include "Arduino_GFX_Library.h"
#include "Preferences.h"
#include "pet.h"
#include "audio.h"
#include "party.h"
#include <cstdio>

uint32_t g_seed = 0xA0FF;
FakeSerial Serial;
FakeESP ESP;
FakeWire Wire;
volatile int g_touchX = 0, g_touchY = 0;
volatile bool g_touchDown = false;
void FakeESP::restart() { exit(0); }
int FakeSerial::available() { return 0; }
String FakeSerial::readStringUntil(char) { return String(""); }

void setup(); void loop(); void emuSetMillis(uint32_t); void emuFireInterrupt();
extern Pet pet;
extern uint32_t lastInteract, lastSerialAt;
extern bool screenOff; extern uint8_t gTapWake;
extern uint8_t dimStage;
extern Arduino_CO5300 *panel;

static int bad = 0;
static void ck(bool ok, const char *w) { printf("%s  %s\n", ok ? "PASS" : "FAIL", w); if (!ok) bad++; }
static uint32_t T0 = 5000000;
static void at(uint32_t ms) { emuSetMillis(T0 + ms); loop(); loop(); }
static void ready() {
  pet.fullness = pet.joy = pet.energy = pet.hygiene = 80;
  pet.poops = 0; pet.sickness = SICK_NONE; pet.sleeping = false; pet.ceremony = CER_NONE;
  while (pet.hasLearnOffer()) pet.declineLearn();
}
static void fresh() {                 // the screen on, just touched
  ready();
  emuSetMillis(T0);
  if (screenOff) { screenOff = false; pet.setScreenOff(false); }
  lastInteract = T0; lastSerialAt = 0;
  loop();
}

int main() {
  setup();
  pet.dbgHatchAs(1, false);

  fresh();
  at(60000);
  ck(!screenOff && dimStage == 0, "a minute after a touch it is still bright");
  at(100000);
  ck(!screenOff && dimStage == 1, "after 90 s it dims");
  at(160000);
  ck(!screenOff && dimStage == 2, "after 150 s it is nearly off");
  at(185000);
  ck(screenOff, "after 3 minutes untouched it turns OFF, not just dim");
  ck(panel->brightness == 0, "with the panel at zero brightness");
  ck(pet.screenIsOff, "and the pet knows, as when PWR is pressed");
  at(600000);
  ck(screenOff, "and stays off");

  // a touch wakes it (tap to wake on)
  gTapWake = true;
  g_touchX = 233; g_touchY = 233; g_touchDown = true; emuFireInterrupt();
  at(600100);
  g_touchDown = false; emuFireInterrupt();
  at(600300);
  ck(!screenOff && panel->brightness > 0, "a touch turns it back on");
  at(600300 + 185000);
  ck(screenOff, "and three quiet minutes later it is off again");

  // sending sprites from the installer keeps it up
  fresh();
  for (uint32_t t = 10000; t <= 400000; t += 10000) { lastSerialAt = T0 + t; at(t); }
  ck(!screenOff, "while the installer is sending, the screen stays on");
  at(400000 + 185000);
  ck(screenOff, "and turns off once it has finished");

  if (bad) { printf("FAILURES: %d\n", bad); return 1; }
  printf("auto-off test passed\n");
  return 0;
}
