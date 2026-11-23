// The updates brought over in 1.6.0, on the real sketch: the moon of a sleeping animal on a dim screen,
// the idle timer, the standby clock and self-care, the Stadium, the care pace, light sleep, the battery
// saver, the Mine and the training fruits. Adapted from the upstream engine's own tests (MIT).
#include "Arduino.h"
#include "Arduino_GFX_Library.h"
#include "Preferences.h"
#include "pet.h"
#include "audio.h"
#include "party.h"
#include "standby.h"
#include "tourney.h"
#include "esp_sleep.h"
#include <chrono>
#include <thread>
#include <cstring>

uint32_t g_seed = 0x46A5;
FakeSerial Serial;
FakeESP ESP;
FakeWire Wire;
volatile int g_touchX = 0, g_touchY = 0;
volatile bool g_touchDown = false;
void FakeESP::restart() { exit(0); }
int FakeSerial::available() { return 0; }
String FakeSerial::readStringUntil(char) { return String(""); }

void setup(); void loop();
extern Pet pet;
extern bool battleOpen, clockOpen, screenOff, fpOpen, trainOpen;
extern uint8_t settingsPage, dimStage, fpMenuCat;
extern uint32_t lastInteract;
void clockTap(int16_t x, int16_t y);
void onTap(int16_t x, int16_t y);
uint8_t uiCurrentScreen();
void rtcSetEpoch(uint32_t e);
int uiSleepButton(int *cx, int *cy);
static const uint32_t DAY = 86400, D0 = 20000 * DAY + 12 * 3600;

static int bad = 0;
static void ck(bool ok, const char *what) {
  printf("%s  %s\n", ok ? "PASS" : "FAIL", what);
  if (!ok) bad++;
}
static void pumpMs(int ms) {
  for (int i = 0; i < ms / 30; i++) {
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    loop();
  }
}

static void click(int x, int y) {
  g_touchX = x; g_touchY = y; g_touchDown = true;
  emuFireInterrupt();
  pumpMs(150);
  g_touchDown = false;
  emuFireInterrupt();
  pumpMs(90);
}

static void readyPet() {
  pet.dbgHatchAs(1, false);
  pet.ceremony = CER_NONE;
  pet.sleeping = false;
  pet.fullness = pet.joy = pet.energy = pet.hygiene = 80;
  pet.sickness = SICK_NONE;
  pet.poops = 0;
  while (pet.hasLearnOffer()) pet.declineLearn();
  pet.setClock(D0);
  fpOpen = false; battleOpen = false; trainOpen = false; fpMenuCat = 0;
}

static void moonRepro() {
  readyPet();
  int lx = 0, ly = 0; uiSleepButton(&lx, &ly);
  uint32_t night = 20000 * DAY + 21 * 3600 + 30 * 60;
  rtcSetEpoch(night); pet.setClock(night, true); screenOff = false;
  pumpMs(200);
  pet.dozeOff();
  lastInteract = millis() - 160UL * 1000;   // dimmed all the way, before the screen turns itself off at 180 s
  pumpMs(200);
  ck(pet.sleeping && pet.lightOn, "it dozed off with the light on, and the screen dimmed with the idle time");
  ck(dimStage == 2, "(the screen is in its deepest idle dim)");
  click(lx, ly);
  ck(!pet.sleeping, "the FIRST tap on the moon wakes it, even on a screen dimmed by idling");

  // a touch elsewhere on a dimmed screen still only lights it, as before
  readyPet();
  pet.setClock(night, true);
  pet.dozeOff();
  lastInteract = millis() - 160UL * 1000;   // dimmed all the way, before the screen turns itself off at 180 s
  pumpMs(200);
  ck(pet.sleeping, "asleep again");
  click(233, 120);
  ck(pet.sleeping && dimStage == 0, "a touch away from the moon on a dim screen only lights it up");

  // the doze hour: 21 by default, or 22, 23, midnight
  readyPet();
  ck(pet.dozeStart == 21, "the doze hour is 21:00 by default");
  auto dozesAt = [&](uint8_t start, int hour) {
    readyPet();
    pet.dozeStart = start;
    uint32_t t = 20000 * DAY + hour * 3600 + 30 * 60;
    rtcSetEpoch(t); pet.setClock(t, true); screenOff = false;
    pet.sleeping = false;
    pet.dozeOff();
    return pet.sleeping;
  };
  ck(dozesAt(21, 21), "start 21: dozes at 21:30");
  ck(!dozesAt(23, 21) && !dozesAt(23, 22), "start 23: not at 21:30 nor 22:30");
  ck(dozesAt(23, 23), "start 23: dozes at 23:30");
  ck(!dozesAt(0, 23), "start 00: not at 23:30");
  ck(dozesAt(0, 1) && dozesAt(23, 3), "start 00 dozes at 01:30 and start 23 still at 03:30");
  ck(!dozesAt(23, 7), "and none of them after 06:00");
  ck(!dozesAt(22, 12), "never in the afternoon");

  // the Settings > Parent row cycles 21 -> 22 -> 23 -> 00 -> 21 and is saved
  readyPet();
  pet.dozeStart = 21;
  clockOpen = true; settingsPage = 5;
  pumpMs(150);
  extern void clockTap(int16_t x, int16_t y);
  uint8_t seen[5]; seen[0] = pet.dozeStart;
  for (int i = 1; i < 5; i++) { clockTap(233, 276); seen[i] = pet.dozeStart; }
  ck(seen[1] == 22 && seen[2] == 23 && seen[3] == 0 && seen[4] == 21, "the Parent page row cycles 21, 22, 23, 00 and back");
  { Preferences pr; pr.begin("tamanimals", true); ck(pr.getUChar("dzs", 99) == 21, "and it is saved under dzs"); pr.end(); }
  clockOpen = false;
}

static void idleWrapTest() {
  readyPet();
  uint32_t night = 20000 * DAY + 23 * 3600 + 57 * 60;
  rtcSetEpoch(night); pet.setClock(night, true); screenOff = false;
  pumpMs(120);
  lastInteract = millis() + 300;            // stamped after the clock the loop is about to read
  for (int i = 0; i < 4; i++) { pumpMs(30); lastInteract = millis() + 300; }
  ck(dimStage == 0, "a stamp just ahead of the clock does not read as a long idle (no dim blink)");
  ck(!pet.sleeping, "and does not doze an animal off at night");
}

extern bool g_emuUsb, gStandbyOn;

static void standbyTests() {
  // the rule
  StandbyCtx c = { true, true, true, false, STANDBY_IDLE_MS };
  ck(standbyDue(c), "plugged in, screen on, main screen, untouched 90 s: the clock");
  c.idleMs = STANDBY_IDLE_MS - 1; ck(!standbyDue(c), "not a millisecond sooner");
  c.idleMs = STANDBY_IDLE_MS; c.usb = false; ck(!standbyDue(c), "never on battery");
  c.usb = true; c.screenOn = false; ck(!standbyDue(c), "not with the screen off");
  c.screenOn = true; c.mainScreen = false; ck(!standbyDue(c), "not over a menu or a game");
  c.mainScreen = true; c.modal = true; ck(!standbyDue(c), "not during a ceremony, an evolution, the alarm or an egg");
  ck(selfCareDue(true, SELFCARE_IDLE_MS) && !selfCareDue(true, SELFCARE_IDLE_MS - 1) && !selfCareDue(false, SELFCARE_IDLE_MS * 9), "self-care: plugged in and untouched for 10 minutes");

  // the date and time helpers
  ck(weekdayOf(10957) == 6, "2000-01-01 was a Saturday");
  ck(weekdayOf(20779) == 0, "22 Nov 2026 is a Sunday");
  bool shiftOk = true, moves = false;
  for (uint32_t m = 0; m < 200; m++) {
    int dx, dy, nx, ny; standbyShift(m, dx, dy); standbyShift(m + 1, nx, ny);
    if (dx < -4 || dx > 4 || dy < -4 || dy > 4) shiftOk = false;
    if (dx != nx || dy != ny) moves = true;
  }
  ck(shiftOk && moves, "the burn-in shift stays within 4 px and changes with the minute");
  char t[8]; bool pm;
  standbyTimeText(t, sizeof(t), 0, 5, false, &pm);  ck(!strcmp(t, "12:05") && !pm, "12 h: midnight is 12:05 AM");
  standbyTimeText(t, sizeof(t), 13, 5, false, &pm); ck(!strcmp(t, "1:05") && pm, "12 h: 13:05 is 1:05 PM");
  standbyTimeText(t, sizeof(t), 13, 5, true, &pm);  ck(!strcmp(t, "13:05"), "24 h: 13:05");
  ck(standbyTextBlocks("12:05") == 25 && standbyTextBlocks("1:05") == 19, "the block width of the time text");

  // on the real loop: the clock appears, and a touch only leaves it
  readyPet();
  extern bool screenOff; extern bool menuOpen;
  screenOff = false; menuOpen = false;
  g_emuUsb = true;
  pumpMs(120); lastInteract = millis(); pumpMs(120);
  ck(!gStandbyOn, "just touched: no clock");
  lastInteract = millis() - STANDBY_IDLE_MS - 500; pumpMs(150);
  ck(gStandbyOn, "plugged in and untouched for 90 s: the standby clock shows");
  uint8_t foodBefore = pet.fullness;
  click(233, 330);
  ck(!gStandbyOn, "a touch leaves the clock");
  ck(!menuOpen && pet.fullness == foodBefore, "and only leaves it: that touch did nothing else");
  g_emuUsb = false; lastInteract = millis() - STANDBY_IDLE_MS - 500; pumpMs(150);
  ck(!gStandbyOn, "unplugged: no clock, however long it has been left");
  lastInteract = millis();

  // self-care: one low need a minute, in order, silently
  auto fresh = [&]() { readyPet(); pet.setPlugged(true); pet.sleeping = false; pet.poops = 0; pet.fullness = pet.joy = pet.energy = pet.hygiene = 80; };
  fresh(); pet.fullness = 20; pet.dbgTick();
  ck(pet.fullness == 50, "plugged in and idle: a hungry one is fed up to 50");
  fresh(); pet.poops = 3; pet.hygiene = 30; pet.dbgTick();
  ck(pet.poops == 0 && pet.hygiene >= 50, "dirty: cleaned up to 50 and the piles gone");
  fresh(); pet.fullness = 20; pet.energy = 20; pet.dbgTick();
  ck(pet.fullness == 50 && pet.energy < 40, "one thing a minute: food first, energy waits");
  pet.dbgTick();
  ck(pet.energy == 50, "and the next minute energy");
  fresh(); pet.joy = 20; pet.dbgTick();
  ck(pet.joy == 50, "sad: cheered up to 50");
  fresh(); pet.fullness = 20; pet.setPlugged(false); pet.dbgTick();
  ck(pet.fullness < 40, "unplugged: nothing is done for it");
  fresh(); pet.fullness = 20; pet.sleeping = true; pet.dbgTick();
  ck(pet.fullness < 40, "asleep: nothing is done for it either");
  fresh(); pet.fullness = 20; uint8_t bondBefore = pet.bond; pet.dbgTick();
  ck(pet.bond == bondBefore, "it is not the player's care: the bond does not move");
  pet.setPlugged(false);
}

extern bool tourOpen, mapOpen; extern uint8_t mapPage;
int mapGridHit(int16_t x, int16_t y);
void mapMenuTap(int16_t x, int16_t y);

static void stadiumTests() {
  readyPet();
  pet.energy = 80; pet.joy = 50; pet.sleeping = false; pet.sickness = SICK_NONE;
  pet.setClock(D0);
  extern bool screenOff; screenOff = false;
  mapOpen = true; mapPage = 2;
  // the Stadium is a tile on the third page of the map: the second one there
  int seen[4] = { -9, -9, -9, -9 }, nseen = 0, hx[4] = { 0 }, hy[4] = { 0 };
  for (int y = 90; y < 400 && nseen < 4; y += 6)
    for (int x = 60; x < 410 && nseen < 4; x += 6) {
      int h = mapGridHit((int16_t)x, (int16_t)y);
      if (h < 0) continue;
      bool known = false;
      for (int k = 0; k < nseen; k++) if (seen[k] == h) known = true;
      if (!known) { seen[nseen] = h; hx[nseen] = x; hy[nseen] = y; nseen++; }
    }
  ck(nseen == 3, "the third page of the map holds three places: the trip, the Stadium and the Mine");
  mapMenuTap((int16_t)hx[1], (int16_t)hy[1]);
  ck(tourOpen && !mapOpen, "tapping the second opens the Stadium");
  pumpMs(120);
  uint16_t medals0 = pet.tourGold + pet.tourSilver + pet.tourBronze; uint32_t coins0 = pet.coins;
  click(233, 132);                                          // the first event: the 100 m
  pumpMs(5000);
  ck(pet.tourGold + pet.tourSilver + pet.tourBronze == medals0, "an event is still playing after five seconds (about ten in all)");
  pumpMs(11000);                                            // it plays by itself (3-2-1-GO, the run), the podium follows
  uint16_t medals1 = pet.tourGold + pet.tourSilver + pet.tourBronze;
  ck(medals1 == medals0 + 1, "an event ends with exactly one medal");
  ck(pet.coins >= coins0 + TOUR_COINS_BRONZE && pet.energy == 80 - TOUR_ENERGY_COST, "it paid its coins and cost 15 energy");
  ck(pet.tourBest[EV_SPRINT] > 0, "and the 100 m has a record now");
  click(233, 330);                                          // back to the list (after the podium has been seen)
  pumpMs(120);
  click(233, 132 + 76 * 0);                                 // the podium needs a tap first: this one leaves it
  pumpMs(120);
  // a jump event (page 2 holds the long jump and the high jump): one at a time
  tourOpen = true;
  { extern void tourSwipe(int); tourSwipe(-1); }
  pet.energy = 80;
  click(233, 132);
  pumpMs(13000);                                            // 3-2-1-GO, three turns, the podium
  uint16_t medals2 = pet.tourGold + pet.tourSilver + pet.tourBronze;
  ck(medals2 >= medals1 + 1, "a jump event also ends with a medal");
  click(233, 330); pumpMs(120);
  // too tired: nothing starts
  pet.energy = TOUR_MIN_ENERGY - 5;
  uint16_t medals3 = pet.tourGold + pet.tourSilver + pet.tourBronze;
  click(233, 132); pumpMs(1500);
  ck(pet.tourGold + pet.tourSilver + pet.tourBronze == medals3 && pet.energy == TOUR_MIN_ENERGY - 5, "too tired: no event starts and nothing is spent");
  // the X leaves
  click(233, 22); pumpMs(120);
  ck(!tourOpen, "the X leaves the Stadium");
  pet.energy = 80;
}

static void carePaceTests() {
  // awake needs: 20 minutes of ticks from a full belly
  auto after20 = [&](uint8_t pace, bool sleep) {
    readyPet();
    pet.carePace = pace;
    pet.fullness = 100; pet.energy = 100; pet.hygiene = 100; pet.joy = 100;
    pet.sleeping = sleep; pet.lightOn = false;
    for (int i = 0; i < 20; i++) pet.dbgTick();
    return (int)pet.fullness;
  };
  ck(after20(0, false) == 60, "normal pace: food falls 2 a minute, as always (100 -> 60 in 20 min)");
  ck(after20(1, false) == 80, "relaxed pace: half as fast (100 -> 80)");
  ck(after20(2, false) == 90, "slow pace: a quarter as fast (100 -> 90)");
  int s0 = after20(0, true), s2 = after20(2, true);
  ck(s0 == s2, "asleep, the pace changes nothing");

  // away for an hour: the same pace applies to the catch-up
  auto away = [&](uint8_t pace) {
    readyPet();
    pet.carePace = pace;
    pet.fullness = 100; pet.energy = 100; pet.hygiene = 100; pet.joy = 100;
    pet.sleeping = false; pet.poops = 0;
    pet.setClock(D0);
    pet.syncClock(D0 + 3600);
    return (int)pet.fullness;
  };
  int f0 = away(0), f1 = away(1), f2 = away(2);
  printf("      away fullness: normal %d relaxed %d slow %d\n", f0, f1, f2);
  ck(f0 < f1 && f1 < f2, "after an hour away, a slower pace comes back less hungry");

  // the pace and its page
  readyPet();
  pet.carePace = 0;
  clockOpen = true; settingsPage = 6;                // SET_CARE
  pumpMs(150);
  uint8_t seen[4]; seen[0] = pet.carePace;
  for (int i = 1; i < 4; i++) { clockTap(233, 156); seen[i] = pet.carePace; }
  ck(seen[1] == 1 && seen[2] == 2 && seen[3] == 0, "the Care page row cycles Normal, Relaxed, Slow and back");
  { Preferences pr; pr.begin("tamanimals", true); ck(pr.getUChar("cpc", 9) == 0, "and the pace is saved under cpc"); pr.end(); }
  extern uint8_t gScreenOffSaver;
  gScreenOffSaver = 1;
  clockTap(233, 216);
  ck(gScreenOffSaver == 0, "the screen-off saver row switches it off");
  { Preferences pr; pr.begin("tamanimals", true); ck(pr.getUChar("sos", 9) == 0, "and it is saved under sos"); pr.end(); }
  clockTap(233, 216);
  ck(gScreenOffSaver == 1, "and on again");
  clockOpen = false;

  // the saver: slower chip only with the screen off and the switch on
  screenOff = false; pumpMs(150);
  ck(emuCpuMhz() == 240, "screen on: full speed");
  screenOff = true; pumpMs(150);
  ck(emuCpuMhz() == 80, "screen off with the saver: 80 MHz");
  gScreenOffSaver = 0; pumpMs(150);
  ck(emuCpuMhz() == 240, "screen off with the saver switched off: full speed");
  gScreenOffSaver = 1; pumpMs(150);
  ck(emuCpuMhz() == 80, "and slow again");
  screenOff = false; pumpMs(150);
  ck(emuCpuMhz() == 240, "waking the screen restores full speed");

  // light sleep: only screen off + saver + on battery, with the sensor storing the steps itself
  extern bool g_emuUsb; bool imuFifoActive();
  g_emuUsb = false; gScreenOffSaver = 1; screenOff = true; pumpMs(150);
  int sl0 = emuSleepCalls(); pumpMs(150);
  ck(emuSleepCalls() > sl0 && imuFifoActive(), "screen off on battery with the saver: the chip sleeps and the sensor's FIFO is on");
  g_emuUsb = true; pumpMs(90); sl0 = emuSleepCalls(); pumpMs(150);
  ck(emuSleepCalls() == sl0 && !imuFifoActive(), "plugged in: it never sleeps (the serial port stays) and the FIFO is off");
  g_emuUsb = false; gScreenOffSaver = 0; pumpMs(90); sl0 = emuSleepCalls(); pumpMs(150);
  ck(emuSleepCalls() == sl0 && !imuFifoActive(), "saver switched off: no sleep");
  gScreenOffSaver = 1; screenOff = false; pumpMs(90); sl0 = emuSleepCalls(); pumpMs(150);
  ck(emuSleepCalls() == sl0 && !imuFifoActive(), "screen on: no sleep, the poll task counts the steps");
  g_emuUsb = false;
}

static void saverModeTests() {
  extern int g_emuBatPct; extern bool g_emuUsb; extern uint8_t gPowerLevel, gSaverMode, dimStage;
  extern uint32_t lastInteract;
  pet.sleeping = false; screenOff = false; gStandbyOn = false;
  g_emuUsb = false; gSaverMode = 0; g_emuBatPct = 80; lastInteract = millis(); pumpMs(6000);
  ck(gPowerLevel == 0, "80 % on battery: normal");
  g_emuBatPct = 18; pumpMs(6000);
  ck(gPowerLevel == 1, "18 %: the saver is on");
  lastInteract = millis(); pumpMs(50000);
  ck(dimStage >= 1, "in the saver it dims after about 45 s (normally 90 s)");
  g_emuBatPct = 8; pumpMs(6000);
  ck(gPowerLevel == 2, "8 %: critical");
  g_emuUsb = true; pumpMs(6000);
  ck(gPowerLevel == 0, "plugged in: back to normal");
  g_emuUsb = false; g_emuBatPct = 80; gSaverMode = 2; pumpMs(6000);
  ck(gPowerLevel == 1, "'always' saves at any charge");
  gSaverMode = 1; g_emuBatPct = 5; pumpMs(6000);
  ck(gPowerLevel == 0, "'off' never saves");
  gSaverMode = 0; g_emuBatPct = 87; pumpMs(6000);
  ck(gPowerLevel == 0, "back to a full charge: normal");
  lastInteract = millis(); pumpMs(200);
}

static void mineSceneTests() {
  extern uint8_t placeScene, mapPage, mineHits; extern bool mapOpen;
  pet.dbgHatchAs(18, false);
  while (pet.hasLearnOffer()) pet.declineLearn();
  pet.sleeping = false; pet.ceremony = CER_NONE; pet.energy = 80; pet.mineDay = 0; pet.mineToday = 0;
  uint8_t before = pet.mineDigsLeft();
  mapOpen = true; mapPage = 2; pumpMs(100);
  click(150, 212);
  ck(placeScene == 20, "the map's third page has the Mine tile, and it opens");
  int need = pet.mineTaps();
  ck(need >= 4 && need <= 8, "a digger needs 4 to 8 taps");
  for (int k = 0; k < need - 1; k++) click(233, 250);
  ck(pet.mineDigsLeft() == before && pet.energy == 80 && mineHits == need - 1, "taps crack the rock but the dig is not spent yet");
  click(233, 250);
  ck(pet.mineDigsLeft() == before - 1 && pet.energy <= 70 && mineHits == 0, "the last tap breaks it: one dig, ten energy");
  pumpMs(900);
  click(233, 400 + 0);   // BACK
  ck(placeScene == 0, "BACK leaves the mine");
  mapOpen = false;
  pet.energy = 100;
}

static void fruitBagTests() {
  extern bool bagOpen; extern int8_t bagCat; extern uint8_t bagPage; extern uint32_t fpToastUntil;
  pet.dbgHatchAs(25, false);
  while (pet.hasLearnOffer()) pet.declineLearn();
  pet.sleeping = false; pet.ceremony = CER_NONE;
  pet.seedCount = 1; pet.fruitCount[0] = 2; pet.fruitCount[1] = 0; pet.fruitCount[2] = 1; pet.trAtk = 10; pet.trSpe = 0;
  bagOpen = true; bagCat = 5; bagPage = 0; pumpMs(100);
  click(233, 224);   // the second row: the first fruit that is in the bag (Power)
  ck(pet.fruitCount[0] == 1 && pet.trAtk == 14, "tapping a Power fruit in the garden aisle eats it: +4 Attack");
  ck(fpToastUntil != 0, "and shows a message");
  click(233, 224);
  ck(pet.fruitCount[0] == 0 && pet.trAtk == 18, "the last one too");
  pumpMs(100);
  ck(bagOpen && bagCat == 5, "the bag stays open");
  click(233, 224);   // now the second row is the Swift fruit
  ck(pet.fruitCount[2] == 0 && pet.trSpe == 4, "the list moves up: the next row is the Swift fruit");
  bagOpen = false; bagCat = -1;
}

int main() {
  setup();
  pumpMs(120);
  click(233, 108 + 30);
  pumpMs(60);
  click(233, 145);
  if (pet.awaitingStarter()) { printf("FAIL: no starter\n"); return 1; }
  pumpMs(120);
  moonRepro();
  idleWrapTest();
  standbyTests();
  stadiumTests();
  carePaceTests();
  saverModeTests();
  mineSceneTests();
  fruitBagTests();
  printf("updates_test: %s\n", bad ? "FAILURES" : "all passed");
  return bad ? 1 : 0;
}
