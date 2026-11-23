// The Tournament's rules (tourney.h): ratings, type bonuses, marks, places, rewards' numbers and a
// fairness simulation. The screens are not drawn here; the loop-level taps are in games46_test.
#include "Arduino.h"
#include "Preferences.h"
#include "pet.h"
#include "tourney.h"
#include <cstdio>
#include <cstdlib>
uint32_t g_seed = 21; FakeSerial Serial; FakeESP ESP; FakeWire Wire;
volatile int g_touchX = 0, g_touchY = 0; volatile bool g_touchDown = false; bool wasPressed = false;
static uint32_t g_ms = 0; uint32_t millis() { return g_ms; }
void FakeESP::restart() { exit(0); }
int FakeSerial::available() { return 0; } String FakeSerial::readStringUntil(char) { return String(""); }
void sfxPlay(uint8_t) {}
static int bad = 0;
static void ck(bool ok, const char *w) { printf("%s  %s\n", ok ? "PASS" : "FAIL", w); if (!ok) bad++; }
static float urand() { return (rand() % 10000) / 10000.0f; }

static_assert(EV_COUNT == TOUR_EVENTS, "the pet keeps one record per event");
static const uint32_t DAY = 86400, D0 = 20000 * DAY + 12 * 3600;

int main() {
  srand(7);
  // growth stage: a cub is a little slower, an adult a little faster
  Athlete cub = athleteOf(1, 10), juv = cub, adult = cub;
  cub.growth = 0; juv.growth = 1; adult.growth = 2;
  ck(athleteRating(cub, EV_SPRINT) < athleteRating(juv, EV_SPRINT) && athleteRating(juv, EV_SPRINT) < athleteRating(adult, EV_SPRINT), "cub < juvenile < adult");
  ck(rivalAthlete(5, 10, 0, 0, 0, 0, 0).growth == 0, "a rival is at the stage asked for");
  // talent bonuses
  ck(typeBonus(EV_SWIM, PT_SWIM, PT_NONE) > 1.2f && typeBonus(EV_SWIM, PT_DIG, PT_NONE) < 0.9f, "swimming: swimmers up, diggers down");
  ck(typeBonus(EV_SPRINT, PT_SPEED, PT_NONE) > 1.1f, "the 100 m favours speed");
  ck(typeBonus(EV_LIFT, PT_STRENGTH, PT_NONE) > 1.1f, "the weights favour strength");
  ck(typeBonus(EV_TORCH, PT_PACK, PT_NONE) > 1.1f, "the torch run favours a pack");
  ck(typeBonus(EV_HIGHJUMP, PT_FLIGHT, PT_NONE) > 1.2f, "the high jump favours flight");
  ck(typeBonus(EV_SWIM, PT_SWIM, PT_SPEED) < typeBonus(EV_SWIM, PT_SWIM, PT_NONE) && typeBonus(EV_SWIM, PT_SWIM, PT_SPEED) > 1.0f,
     "two talents take the average of their modifiers");
  ck(typeBonus(EV_SPRINT, PT_VENOM, PT_NONE) == 1.0f, "a talent with no entry changes nothing");
  bool everyTypeHasAnEvent = true;
  for (uint8_t t = 0; t <= PT_SPIRIT; t++) {
    bool up = false;
    for (uint8_t ev = 0; ev < EV_COUNT; ev++) if (typeBonus(ev, t, PT_NONE) > 1.0f) up = true;
    if (!up) { everyTypeHasAnEvent = false; printf("      no event favours type %u\n", t); }
  }
  ck(everyTypeHasAnEvent, "every talent has at least one event where it does better");
  bool tableOk = true;
  for (unsigned i = 0; i < sizeof(TYPE_MODS) / sizeof(TYPE_MODS[0]); i++)
    if (TYPE_MODS[i].ev >= EV_COUNT || TYPE_MODS[i].type > PT_SPIRIT || TYPE_MODS[i].mul < 0.8f || TYPE_MODS[i].mul > 1.3f) tableOk = false;
  ck(tableOk, "the talent table only holds real events and talents, with modest numbers");
  bool wOk = true;
  for (int e = 0; e < EV_COUNT; e++) { float s = EV_W[e][0] + EV_W[e][1] + EV_W[e][2] + EV_W[e][3]; if (s < 0.999f || s > 1.001f) wOk = false; }
  ck(wOk, "every event's stat weights add up to 1");

  // ratings
  Athlete cheetah = athleteOf(4, 10), sloth = athleteOf(205, 10);
  ck(athleteRating(cheetah, EV_SPRINT) > athleteRating(sloth, EV_SPRINT), "a cheetah out-sprints a sloth");
  Athlete otter = athleteOf(216, 10), lion = athleteOf(1, 10);
  ck(athleteRating(otter, EV_SWIM) / athleteRating(lion, EV_SWIM) > athleteRating(otter, EV_SPRINT) / athleteRating(lion, EV_SPRINT),
     "a giant otter gains on a lion in the pool");
  Athlete a1 = athleteOf(25, 10), a2 = a1; a2.trSpe = 100; a2.trAtk = 100; a2.trDef = 100;
  float gain = athleteRating(a2, EV_SPRINT) / athleteRating(a1, EV_SPRINT);
  ck(gain > 1.05f && gain < 1.13f, "full training is worth about +12 %");
  Athlete l1 = athleteOf(25, 1), l2 = athleteOf(25, 100);
  float lg = athleteRating(l2, EV_SPRINT) / athleteRating(l1, EV_SPRINT);
  ck(lg > 1.15f && lg < 1.22f, "level 100 is worth about +20 % over level 1");
  ck(performance(100, 0) < 86 && performance(100, 0) > 84 && performance(100, 1) > 114 && performance(100, 1) < 116, "luck is +-15 %");

  // marks
  ck(eventMark(EV_SPRINT, 100) < eventMark(EV_SPRINT, 60), "a better performance runs the 100 m faster");
  ck(eventMark(EV_SPRINT, 1000) >= 7.0f && eventMark(EV_SPRINT, 1) <= 25.0f, "marks stay inside their limits");
  ck(eventMark(EV_LONGJUMP, 120) > eventMark(EV_LONGJUMP, 60), "a better performance jumps further");
  ck(eventMark(EV_HIGHJUMP, 100) > 1.5f && eventMark(EV_HIGHJUMP, 100) < 2.0f, "the high jump is in a human range");
  float lift = eventMark(EV_LIFT, 90); ck(((int)lift) % 5 == 0, "weights go up in 5 kg steps");
  bool lw = eventLowerWins(EV_SPRINT) && eventLowerWins(EV_SWIM) && eventLowerWins(EV_CLIMB) && eventLowerWins(EV_TRAIL) &&
            !eventLowerWins(EV_LONGJUMP) && !eventLowerWins(EV_HIGHJUMP) && !eventLowerWins(EV_LIFT) && !eventLowerWins(EV_DIVE);
  ck(lw, "races want less, jumps weights and dives want more");
  ck(eventUnit(EV_LONGJUMP) == UNIT_METERS && eventUnit(EV_LIFT) == UNIT_KG && eventUnit(EV_DIVE) == UNIT_POINTS && eventUnit(EV_TORCH) == UNIT_MINSEC && eventUnit(EV_SPRINT) == UNIT_SECS, "each event counts in its unit");

  // places
  ck(playerPlace(10, 11, 12, true) == 1 && playerPlace(10, 9, 12, true) == 2 && playerPlace(10, 9, 8, true) == 3, "races: places by time");
  ck(playerPlace(5, 4, 3, false) == 1 && playerPlace(5, 6, 3, false) == 2 && playerPlace(5, 6, 7, false) == 3, "jumps: places by distance");
  ck(playerPlace(10, 10, 10, true) == 1 && playerPlace(5, 5, 5, false) == 1, "a tie goes to the player");

  // rewards and records
  ck(tourCoins(1) == 25 && tourCoins(2) == 12 && tourCoins(3) == 5, "gold 25, silver 12, bronze 5 coins");
  ck(eventRecord(EV_SPRINT, 0, 1200) && eventRecord(EV_SPRINT, 1300, 1200) && !eventRecord(EV_SPRINT, 1100, 1200), "a record is a lower time in a race, and the first mark is one");
  ck(eventRecord(EV_LONGJUMP, 500, 520) && !eventRecord(EV_LONGJUMP, 520, 500), "and a longer jump");
  ck(eventMarkStore(EV_SPRINT, 11.37f) == 1137 && eventMarkStore(EV_TORCH, 1800.0f) == 18000 && eventMarkStore(EV_LIFT, 300.0f) == 30000, "marks are stored as hundredths (tenths of a second for the long races)");

  // fairness: a random fight against two random animals
  auto goldRate = [&](Athlete me, int rounds) {
    int gold = 0;
    for (int i = 0; i < rounds; i++) {
      uint8_t ev = (uint8_t)(rand() % EV_COUNT);
      int16_t r1 = (int16_t)(1 + rand() % DEX_COUNT), r2 = (int16_t)(1 + rand() % DEX_COUNT);
      Athlete A = rivalAthlete(r1, me.level, urand(), urand(), urand(), urand());
      Athlete B = rivalAthlete(r2, me.level, urand(), urand(), urand(), urand());
      float mine = eventMark(ev, performance(athleteRating(me, ev), urand()));
      float ma = eventMark(ev, performance(athleteRating(A, ev), urand()));
      float mb = eventMark(ev, performance(athleteRating(B, ev), urand()));
      if (playerPlace(mine, ma, mb, eventLowerWins(ev)) == 1) gold++;
    }
    return gold / (float)rounds;
  };
  Athlete champ = athleteOf(4, 60); champ.geneAtk = champ.geneDef = champ.geneSpe = 108; champ.trAtk = champ.trDef = champ.trSpe = 100;
  Athlete baby = athleteOf(1, 1); baby.growth = 0;
  Athlete avg = athleteOf(24, 20); avg.trAtk = avg.trDef = avg.trSpe = 20;
  float gc = goldRate(champ, 6000), gb = goldRate(baby, 6000), ga = goldRate(avg, 6000);
  printf("      gold rate: champion %.2f, average %.2f, hatchling %.2f\n", gc, ga, gb);
  ck(gc > 0.55f, "a trained high-level cheetah wins gold more than half the time");
  ck(gb < 0.25f, "a level-1 cub rarely wins gold");
  ck(ga > 0.15f && ga < 0.60f, "an average companion wins gold now and then, not always");
  ck(gc > ga && ga > gb, "strength shows: champion > average > hatchling");

  // ---- the pet: who may compete, what an event pays and costs, records, the save
  {
    Pet p; p.begin(); p.dbgHatchAs(1, false);
    p.sleeping = false; p.ceremony = CER_NONE; p.sickness = SICK_NONE; p.energy = 80; p.joy = 50;
    p.setClock(D0);
    ck(p.canCompete(), "a healthy, rested companion at home may compete");
    p.energy = TOUR_MIN_ENERGY - 1; ck(!p.canCompete(), "too tired (under 20 energy): no");
    p.energy = TOUR_MIN_ENERGY; ck(p.canCompete(), "exactly 20 energy is enough");
    p.energy = 80; p.sleeping = true; ck(!p.canCompete(), "not while asleep");
    p.sleeping = false; p.sickness = SICK_COLD; ck(!p.canCompete(), "not while sick");
    p.sickness = SICK_NONE;
    Pet egg; egg.begin(); ck(!egg.canCompete() || !egg.isEgg(), "an egg cannot compete");

    uint32_t c0 = p.coins; p.energy = 80; p.joy = 50;
    Pet::TourPay a = p.tourFinish(EV_SPRINT, 1, 1137);
    ck(a.coins == TOUR_COINS_GOLD + TOUR_DAILY_BONUS && a.bonus && a.record, "the first gold of the day: 25 coins + 20 bonus, and a first record");
    ck(p.coins - c0 == (uint32_t)a.coins && p.energy == 65 && p.joy == 60 && p.tourGold == 1, "it paid, cost 15 energy, gave 10 joy and counted the gold");
    Pet::TourPay b = p.tourFinish(EV_SWIM, 1, 2000);
    ck(b.coins == TOUR_COINS_GOLD && !b.bonus, "a second gold the same day: no daily bonus");
    Pet::TourPay c = p.tourFinish(EV_SPRINT, 2, 1200);
    ck(c.coins == TOUR_COINS_SILVER && !c.record && p.tourBest[EV_SPRINT] == 1137 && p.tourSilver == 1, "silver: 12 coins; a slower time is no record");
    Pet::TourPay d = p.tourFinish(EV_SPRINT, 3, 1100);
    ck(d.coins == TOUR_COINS_BRONZE && d.record && p.tourBest[EV_SPRINT] == 1100 && p.tourBronze == 1, "bronze: 5 coins; a faster time is a record whatever the place");
    uint8_t joyBefore = p.joy; p.tourFinish(EV_LIFT, 2, 8000);
    ck(p.joy == joyBefore, "only a gold cheers it up");
    p.energy = 10; p.tourFinish(EV_HIGHJUMP, 3, 150); ck(p.energy == 0, "energy never goes below 0");
    ck(p.tourFinish(EV_COUNT, 1, 1).coins == 0 && p.tourFinish(EV_SPRINT, 0, 1).coins == 0 && p.tourFinish(EV_SPRINT, 4, 1).coins == 0, "a bad event or place pays nothing");
    p.setClock(D0 + DAY);
    ck(p.tourFinish(EV_DIVE, 1, 7000).bonus, "the next day the first gold pays the bonus again");
    ck(p.hasTrophy(TRO_STADIUM) == false, "the trophy is not given at once");
    for (int i = 0; i < 5; i++) p.tourFinish(EV_TRAIL, 1, 20000 - i);
    p.checkTrophies();
    ck(p.hasTrophy(TRO_STADIUM), "five golds earn the Tournament trophy");

    p.saveNow();
    Pet q; q.begin();
    ck(q.tourGold == p.tourGold && q.tourSilver == p.tourSilver && q.tourBronze == p.tourBronze && q.tourGoldDay == p.tourGoldDay, "medals and the bonus day survive a save");
    ck(q.tourBest[EV_SPRINT] == 1100 && q.tourBest[EV_DIVE] == 7000 && q.tourBest[EV_LIFT] == 8000, "and so do the records");
  }

  if (bad) { printf("FAILURES: %d\n", bad); return 1; }
  printf("tourney test passed\n");
  return 0;
}
