// Animal names follow the board's language: Spanish names when it is set to Spanish,
// the table's own names otherwise. The Memes keep their nicknames in every language.
#include "Arduino.h"
#include "Preferences.h"
#include "pet.h"
#include "i18n.h"
#include <cstdio>
#include <cstring>
#include <cctype>
uint32_t g_seed=31; FakeSerial Serial; FakeESP ESP; FakeWire Wire;
volatile int g_touchX=0,g_touchY=0; volatile bool g_touchDown=false; bool wasPressed=false;
uint32_t millis(){return 0;} void FakeESP::restart(){exit(0);}
int FakeSerial::available(){return 0;} String FakeSerial::readStringUntil(char){return String("");}
void sfxPlay(uint8_t){}
static int bad=0;
static void ck(bool ok,const char*w){printf("%s  %s\n",ok?"PASS":"FAIL",w); if(!ok)bad++;}

int main(){
  gLang = LANG_EN;
  ck(!strcmp(dexName(202), "MACAW"), "English: the macaw is MACAW");
  gLang = LANG_ES;
  ck(!strcmp(dexName(202), "GUACAMAYO"), "Spanish: the macaw is GUACAMAYO");
  ck(!strcmp(dexName(206), "CHIGUIRO"), "Spanish: the capybara by its Colombian name");
  ck(!strcmp(dexName(366), "PIE GRANDE"), "Spanish: Bigfoot is PIE GRANDE");
  ck(!strcmp(dexName(329), DEX_TBL[329].name), "a legend with a Spanish name keeps it");
  ck(!strcmp(dexName(DEX_COUNT), DEX_TBL[DEX_COUNT].name), "the Memes keep their nicknames");
  ck(!strcmp(dexName(DEX_TBL[202]), "GUACAMAYO"), "looked up from a table entry too");
  // every free animal (1-325) has a Spanish name the board's font can draw, and fits
  int missing = 0, unfit = 0;
  for (int d = 1; d <= 325; d++) {
    const char *n = dexName(d);
    if (!DEX_NAME_ES[d]) missing++;
    if (strlen(n) > 18) unfit++;
    for (const char *c = n; *c; c++) if (!(isupper((unsigned char)*c) || isdigit((unsigned char)*c) || *c == ' ')) unfit++;
  }
  ck(missing == 0, "every free animal has its Spanish name");
  ck(unfit == 0, "capitals, digits and spaces only, at most 18 letters");
  gLang = LANG_FR;
  ck(!strcmp(dexName(202), "MACAW"), "a language without its own names uses the table's");
  printf("names_test: %s\n", bad ? "FAILED" : "all passed");
  return bad ? 1 : 0;
}
