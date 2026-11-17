// Which sprite file actually gets opened.
//
// SprtMon::load() took a uint8_t while the dex reached 386, so every species
// from 256 up wrapped into Africa: a ANIMAL (258) opened p002.bin and stood
// there as an ANIMAL. Reported from a board, because nothing here looked.
//
// It is the third time this exact trap has been sprung -- DexEntry::evolvesTo
// and TrainerMon::dex were both uint8_t when the expansion landed -- so this
// checks EVERY species can be asked for and comes back as itself, rather than
// spot-checking the one that was reported.
#include "Arduino.h"
#include "Arduino_GFX_Library.h"
#include "Preferences.h"
#include "pet.h"
#include "party.h"
#include "sdmon.h"
#include <cstdio>
#include <cstring>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#define MKDIR(d) _mkdir(d)
#else
#define MKDIR(d) mkdir(d, 0755)
#endif
#include <string>
uint32_t g_seed=7; FakeSerial Serial; FakeESP ESP; FakeWire Wire;
volatile int g_touchX=0,g_touchY=0; volatile bool g_touchDown=false;
uint32_t millis(){return 0;}
void FakeESP::restart(){exit(0);}
int FakeSerial::available(){return 0;} String FakeSerial::readStringUntil(char){return String("");}
// sfxPlay comes from host_impl.cpp here -- this test links the host, not the sketch
void emuSetSpriteDir(const char *);
static int bad=0;
static void ck(bool ok,const char*w){printf("%s  %s\n",ok?"PASS":"FAIL",w); if(!ok)bad++;}

int main(){
  emuSetSpriteDir(SPRITE_DIR);

  // the exact creature that was reported, and the exact one it turned into
  {
    SprtMon a, b;
    bool okA = a.load(258, false);      // ANIMAL
    bool okB = b.load(2, false);        // ANIMAL
    ck(okA, "a the Americas sprite loads at all");
    ck(okA && a.dex == 258, "and it is ANIMAL, not whatever 258 truncates to");
    if (okA && okB) {
      // every animal shares one palette, so tell them apart by their pixels
      const SprtAct &ia = a.acts[0], &ib = b.acts[0];
      bool same = ia.w == ib.w && ia.h == ib.h && !memcmp(ia.data, ib.data, (size_t)ia.w * ia.h);
      ck(!same, "ANIMAL's pixels are not ANIMAL's");
    }
    a.unload(); b.unload();
  }

  // EVERY species: asked for by number, comes back as itself
  {
    int loaded = 0, wrong = 0, missing = 0;
    for (int16_t d = 1; d <= DEX_COUNT; d++) {
      SprtMon m;
      if (!m.load(d, false)) { missing++; continue; }
      loaded++;
      if (m.dex != d) { wrong++; if (wrong < 4) printf("      asked %d, got %d\n", d, m.dex); }
      m.unload();
    }
    printf("      %d sprites loaded, %d missing from the pack\n", loaded, missing);
    ck(loaded > 300, "most of the dex has a sprite to check");
    ck(wrong == 0, "every species that loads is the species that was asked for");
  }

  // THE assertion that actually bites. m.dex only records what was ASKED for,
  // so a truncation inside the filename alone would slip past it -- proven by
  // putting the bug back and watching that check still pass. This compares the
  // PIXELS of every species past 255 against the Africa species it would wrap
  // onto, which is the thing that was really going wrong on the panel.
  {
    int checked = 0, collided = 0;
    for (int16_t d = 256; d <= DEX_COUNT; d++) {
      SprtMon hi, lo;
      if (!hi.load(d, false)) continue;
      if (!lo.load((int16_t)(d - 256), false)) { hi.unload(); continue; }
      checked++;
      if (hi.acts[0].w == lo.acts[0].w && hi.acts[0].h == lo.acts[0].h &&
          !memcmp(hi.acts[0].data, lo.acts[0].data, (size_t)hi.acts[0].w * hi.acts[0].h)) {
        if (collided < 3) printf("      dex %d draws the same pixels as %d\n", d, d - 256);
        collided++;
      }
      hi.unload(); lo.unload();
    }
    printf("      compared %d species past 255 against their wrap-around twin\n", checked);
    ck(checked > 50, "the whole back half of the dex is reachable");
    ck(collided == 0, "and none of them draws the Africa species it would truncate to");
  }

  // and nonsense is refused rather than opening some other creature's file
  {
    SprtMon m;
    ck(!m.load(0, false), "dex 0 loads nothing");
    ck(!m.load(-1, false), "nor a negative dex");
    ck(!m.load(9999, false), "nor one past the table");
  }

  // growth stages: a cub draws its own art, and a card holding only the adult files
  // (the packs from before growth stages) still shows the animal rather than nothing
  {
    auto frame = [](const SprtMon &m) { return std::string((const char *)m.acts[0].data, (size_t)m.acts[0].w * m.acts[0].h); };
    SprtMon adult, cub, juv;
    bool okA = adult.load(1, false, 2), okC = cub.load(1, false, 0), okJ = juv.load(1, false, 1);
    ck(okA && okC && okJ, "an animal loads at every growth stage");
    ck(okA && okC && frame(adult) != frame(cub), "and a cub is not drawn as the adult");
    ck(okC && okJ && frame(cub) != frame(juv), "nor a juvenile as the cub");
    std::string adultPixels = okA ? frame(adult) : "";
    adult.unload(); cub.unload(); juv.unload();

    // copy just p001.bin into a folder of its own (in the working directory)
    const char *only = "adult_only_sprites";
    MKDIR(only);
    {
      FILE *in = fopen(SPRITE_DIR "/p001.bin", "rb"), *out = fopen("adult_only_sprites/p001.bin", "wb");
      char buf[4096]; size_t n;
      while (in && out && (n = fread(buf, 1, sizeof(buf), in)) > 0) fwrite(buf, 1, n, out);
      if (in) fclose(in);
      if (out) fclose(out);
    }
    emuSetSpriteDir(only);
    SprtMon fb;
    ck(fb.load(1, false, 0) && frame(fb) == adultPixels, "with only adult files, a cub falls back to the adult art");
    fb.unload();
    remove("adult_only_sprites/p001.bin");
    emuSetSpriteDir(SPRITE_DIR);
  }

  // the moods (1.2.0): every animal at every stage has its sad, sick, tired, hungry, dirty and
  // happy faces, and each one is drawn, not a copy of the idle frame
  {
    static const uint8_t MOODS[] = { SPRT_SAD, SPRT_SICK, SPRT_TIRED, SPRT_HUNGRY, SPRT_DIRTY, SPRT_HAPPY };
    int lacking = 0, copies = 0, checked = 0;
    for (int16_t d = 1; d <= DEX_COUNT; d++)
      for (uint8_t g = 0; g < 3; g++) {
        SprtMon m;
        if (!m.load(d, false, g)) continue;
        checked++;
        for (uint8_t a : MOODS) {
          if (!m.has(a)) { if (lacking++ < 3) printf("      dex %d stage %u lacks action %u\n", d, g, a); continue; }
          const SprtAct &x = m.acts[a], &i = m.acts[SPRT_IDLE];
          if (x.w == i.w && x.h == i.h && !memcmp(x.data, i.data, (size_t)x.w * x.h)) copies++;
        }
        m.unload();
      }
    printf("      %d sprite files checked for the six moods\n", checked);
    ck(checked > 900, "every animal at every stage was checked");
    ck(lacking == 0, "and each has all six mood faces");
    ck(copies == 0, "and none of them is just the idle frame again");
  }

  // a card from before 1.2.0 (twelve actions) still loads, simply without the moods; and a card
  // from a later version, with an action this firmware does not know, loads too
  {
    FILE *in = fopen(SPRITE_DIR "/p001.bin", "rb");
    std::string blob;
    char buf[4096]; size_t n;
    while (in && (n = fread(buf, 1, sizeof(buf), in)) > 0) blob.append(buf, n);
    if (in) fclose(in);
    // walk the actions to find where the twelfth ends
    size_t pal = (uint8_t)blob[5] | ((uint8_t)blob[6] << 8), pos = 7 + pal * 2, end12 = 0;
    for (int i = 0; i < (uint8_t)blob[4] && pos + 4 <= blob.size(); i++) {
      size_t w = (uint8_t)blob[pos + 1], h = (uint8_t)blob[pos + 2], nf = (uint8_t)blob[pos + 3];
      pos += 4 + nf * 2 + w * h * nf;
      if (i == 11) end12 = pos;
    }
    ck(end12 > 0 && end12 < blob.size(), "the test found the end of the twelfth action");
    const char *dir = "card_versions";
    MKDIR(dir);
    emuSetSpriteDir(dir);
    std::string old = blob.substr(0, end12);
    old[4] = 12;
    FILE *o = fopen("card_versions/p001.bin", "wb"); fwrite(old.data(), 1, old.size(), o); fclose(o);
    SprtMon a;
    bool okOld = a.load(1, false, 2);
    ck(okOld && a.has(SPRT_IDLE) && a.has(SPRT_SIT), "a twelve-action card from before 1.2.0 still loads");
    ck(okOld && !a.has(SPRT_SAD) && !a.has(SPRT_HAPPY), "and simply has no mood faces (the game falls back)");
    a.unload();
    std::string newer = blob;
    newer[4] = (char)((uint8_t)newer[4] + 1);
    const char extra[] = { 40, 2, 2, 1, 100, 0, 0, 1, 2, 3 };   // id 40, 2x2, one frame of 100 ms
    newer.append(extra, sizeof(extra));
    o = fopen("card_versions/p001.bin", "wb"); fwrite(newer.data(), 1, newer.size(), o); fclose(o);
    SprtMon b;
    ck(b.load(1, false, 2) && b.has(SPRT_HAPPY), "a card with an action from a later version loads, the unknown one skipped");
    b.unload();
    remove("card_versions/p001.bin");
    emuSetSpriteDir(SPRITE_DIR);
  }

  printf("%s\n", bad?"FAILURES":"all good");
  return bad?1:0;
}
