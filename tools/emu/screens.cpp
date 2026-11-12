// Drives the real firmware through its main screens and writes each frame as raw RGB565
// (466x466), for the README. Built and run by tools/gen/make_docs.py; not part of the tests.
#include "Arduino.h"
#include "Arduino_GFX_Library.h"
#include "Preferences.h"
#include "pet.h"
#include "audio.h"
#include "party.h"
#include "save.h"
#include "i18n.h"
#include "pin_config.h"
#include <cstdio>
#include <cstring>

uint32_t g_seed = 0x7A3A;
FakeSerial Serial;
FakeESP ESP;
FakeWire Wire;
volatile int g_touchX = 0, g_touchY = 0;
volatile bool g_touchDown = false;
void FakeESP::restart() { exit(0); }
int FakeSerial::available() { return 0; }
String FakeSerial::readStringUntil(char) { return String(""); }

void setup(); void render(); void ensureMon(); void emuSetMillis(uint32_t); void drawSplash(); void onTap(int16_t, int16_t);
void startTrainerBattle(uint8_t, bool); void startBattle(int16_t, uint8_t); void startFunPackGame(uint8_t);
extern Pet pet; extern Party party; extern TpCanvas *gfx;
extern bool battleOpen, mapOpen, galleryOpen, galleryPick, gymOpen, gymPick, partyOpen, menuOpen, shopOpen, cardOpen, fpOpen;
extern uint8_t rpickPage, cardPage, choiceKind, galleryRegion;
extern uint32_t choiceUntil;
extern bool galleryDirty;

static const char *gOut = ".";
static void shot(const char *name) {
  char p[512]; snprintf(p, sizeof p, "%s/%s.raw", gOut, name);
  FILE *f = fopen(p, "wb");
  if (!f) { printf("cannot write %s\n", p); return; }
  fwrite(gfx->buffer(), 2, LCD_WIDTH * LCD_HEIGHT, f);
  fclose(f);
  printf("shot %s\n", name);
}
static void closeAll() {
  battleOpen = mapOpen = galleryOpen = galleryPick = gymOpen = gymPick = partyOpen = menuOpen = shopOpen = cardOpen = fpOpen = false;
  choiceKind = 0;
}
static void pump(int n) {
  // ensureMon() is what loop() runs each pass to load the pet sprite for its species and stage
  for (int i = 0; i < n; i++) { while (pet.hasLearnOffer()) pet.declineLearn(); ensureMon(); delay(60); render(); }
}
// the emulator clock follows real time and delay() does nothing, so jump it past an animation
static void skip(uint32_t ms) { emuSetMillis(millis() + ms); }
static void careFor(int level) {
  pet.ageMinutes = (uint32_t)(level - 1) * MINUTES_PER_LEVEL;
  pet.fullness = pet.joy = pet.energy = pet.hygiene = 70;
  pet.careMistakes = 0; pet.poops = 0; pet.sleeping = false;
  pet.medals = 0xFFFF;                    // nothing left to celebrate, so no banner over the pet
}

int main(int argc, char **argv) {
  if (argc > 1) gOut = argv[1];
  setup();
  drawSplash(); shot("splash");
  pump(3); shot("region");
  onTap(233, 108 + 30);                   // Africa
  pump(3); shot("starter");
  pet.chooseStarter(1);
  pump(2);

  pet.dbgHatchAs(1, false);               // a lion cub
  careFor(10);
  pet.coins = 480;
  pet.wildBalls[0] = 3; pet.wildBalls[1] = 2; pet.wildBalls[2] = 1;
  pump(80); shot("main_cub");
  cardOpen = true; cardPage = 3; pump(2); shot("card_growth");
  closeAll();
  careFor(16); pump(3); shot("grow_button");
  choiceKind = 1; choiceUntil = millis() + 12000; pump(2); shot("grow_dialog");
  closeAll();
  pet.evolve(); pump(2); skip(EVOLVE_ANIM_MS + 500); pump(4); shot("main_juvenile");
  careFor(36); pet.evolve(); pump(2); skip(EVOLVE_ANIM_MS + 500); pump(4); shot("main_adult");
  cardOpen = true; cardPage = 0; pump(2); shot("card_profile");
  cardPage = 1; pump(2); shot("card_stats");
  cardPage = 2; pump(2); shot("card_moves");
  closeAll();

  memset(pet.dexReg, 0x6D, sizeof(pet.dexReg));   // a well-filled field guide
  galleryOpen = true; galleryPick = true; rpickPage = 0; pump(2); shot("guide_regions");
  galleryPick = false; galleryRegion = 0; galleryDirty = true; pump(4); shot("guide_africa");
  closeAll();
  gymOpen = true; gymPick = true; rpickPage = 0; pump(2); shot("ranger_regions");
  gymPick = false; pump(2); shot("ranger_ladder");
  closeAll();
  startTrainerBattle(0, false); pump(5); shot("battle_ranger");
  closeAll();
  startBattle(146, 30); pump(5); shot("battle_wild");
  closeAll();
  mapOpen = true; pump(2); shot("map");
  closeAll();
  shopOpen = true; pump(2); shot("shop");
  closeAll();
  menuOpen = true; pump(2); shot("menu");
  closeAll();
  static const char *GAME[4] = { "game_lookway", "game_highlow", "game_whois", "game_berrydrop" };
  for (uint8_t g = 0; g < 4; g++) { closeAll(); startFunPackGame(g); pump(6); shot(GAME[g]); }
  closeAll(); fpOpen = false;
  pump(4);
  setLang(LANG_ZH); cardOpen = true; cardPage = 3; pump(2); shot("card_growth_zh");
  setLang(LANG_KO); pump(2); shot("card_growth_ko");
  setLang(LANG_EN);
  return 0;
}
