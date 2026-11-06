#pragma once

// ============================================================================
// MASTER ACHIEVEMENTS SYSTEM - Content Expansion
// ============================================================================
// 150+ achievements across 10 categories
// Provides goals for 6+ months of gameplay
// Guides player exploration without forcing playstyle

#include <stdint.h>
#include <time.h>

#define ACHIEVEMENTS_MASTER_COUNT 150

enum AchievementCategory {
  CAT_DISCOVERY = 0,      // 15: Find animals from regions
  CAT_COLLECTION = 1,     // 20: Collection milestones
  CAT_RESPECT = 2,        // 15: Respect level achievements
  CAT_EVOLUTION = 3,      // 12: Pet stage achievements
  CAT_MINIGAME = 4,       // 15: Game skill challenges
  CAT_WALKING = 5,        // 12: Distance challenges
  CAT_HABITAT = 6,        // 15: Habitat mastery
  CAT_CHALLENGE = 7,      // 20: Difficult achievements
  CAT_TIME = 8,           // 12: Time-based milestones
  CAT_SPECIAL = 9,        // 14: Hidden/secret achievements
};

enum AchievementRarity {
  RARITY_COMMON = 0,      // 80% unlock
  RARITY_UNCOMMON = 1,    // 50% unlock
  RARITY_RARE = 2,        // 20% unlock
  RARITY_LEGENDARY = 3,   // 5% unlock
};

struct AchievementMaster {
  uint8_t id;
  const char *name;
  const char *description;
  uint8_t category;
  uint8_t rarity;
  uint32_t requirement_value;  // For automatic checking
  uint8_t requirement_type;    // 0=count, 1=level, 2=percentage, 3=special
};

// ============================================================================
// DISCOVERY ACHIEVEMENTS (15) - Find animals from each region
// ============================================================================

static const AchievementMaster DISCOVERIES[] = {
  {0, "African Explorer", "Found your first African animal", CAT_DISCOVERY, RARITY_COMMON, 1, 0},
  {1, "Asian Scout", "Found your first Asian animal", CAT_DISCOVERY, RARITY_COMMON, 1, 0},
  {2, "European Traveler", "Found your first European animal", CAT_DISCOVERY, RARITY_COMMON, 1, 0},
  {3, "South American Guide", "Found your first South American animal", CAT_DISCOVERY, RARITY_UNCOMMON, 1, 0},
  {4, "North American Pioneer", "Found your first North American animal", CAT_DISCOVERY, RARITY_UNCOMMON, 1, 0},
  {5, "Australian Adventurer", "Found your first Australian animal", CAT_DISCOVERY, RARITY_UNCOMMON, 1, 0},
  {6, "Arctic Explorer", "Found your first Arctic animal", CAT_DISCOVERY, RARITY_UNCOMMON, 1, 0},
  {7, "Ocean Discoverer", "Found your first sea creature", CAT_DISCOVERY, RARITY_UNCOMMON, 1, 0},
  {8, "Rainforest Trekker", "Found your first rainforest animal", CAT_DISCOVERY, RARITY_RARE, 1, 0},
  {9, "Extinct Paleontologist", "Found your first extinct animal", CAT_DISCOVERY, RARITY_RARE, 1, 0},
  {10, "Magic Finder", "Found your first mythical animal", CAT_DISCOVERY, RARITY_RARE, 1, 0},
  {11, "World Traveler", "Found animals from all 11 habitats", CAT_DISCOVERY, RARITY_LEGENDARY, 11, 0},
  {12, "Savanna Master", "Found 15 African savanna animals", CAT_DISCOVERY, RARITY_UNCOMMON, 15, 0},
  {13, "Jungle Expert", "Found 15 rainforest animals", CAT_DISCOVERY, RARITY_UNCOMMON, 15, 0},
  {14, "Ocean Conqueror", "Found 15 ocean animals", CAT_DISCOVERY, RARITY_UNCOMMON, 15, 0},
};

// ============================================================================
// COLLECTION ACHIEVEMENTS (20) - Milestone-based
// ============================================================================

static const AchievementMaster COLLECTIONS[] = {
  {15, "First Steps", "Caught 1 animal", CAT_COLLECTION, RARITY_COMMON, 1, 0},
  {16, "Rising Collector", "Caught 10 animals", CAT_COLLECTION, RARITY_COMMON, 10, 0},
  {17, "Dedicated Collector", "Caught 25 animals", CAT_COLLECTION, RARITY_UNCOMMON, 25, 0},
  {18, "Serious Collector", "Caught 50 animals", CAT_COLLECTION, RARITY_UNCOMMON, 50, 0},
  {19, "Obsessed Collector", "Caught 100 animals", CAT_COLLECTION, RARITY_UNCOMMON, 100, 0},
  {20, "Lifelong Passion", "Caught 250 animals", CAT_COLLECTION, RARITY_RARE, 250, 0},
  {21, "Living Library", "Caught 500 animals", CAT_COLLECTION, RARITY_RARE, 500, 0},
  {22, "Collection Complete", "Caught all 988 animals", CAT_COLLECTION, RARITY_LEGENDARY, 988, 0},
  {23, "Quarter Way", "Caught 25% of animals (247)", CAT_COLLECTION, RARITY_UNCOMMON, 25, 2},
  {24, "Halfway There", "Caught 50% of animals (494)", CAT_COLLECTION, RARITY_UNCOMMON, 50, 2},
  {25, "Three Quarters", "Caught 75% of animals (741)", CAT_COLLECTION, RARITY_RARE, 75, 2},
  {26, "Almost There", "Caught 90% of animals (889)", CAT_COLLECTION, RARITY_RARE, 90, 2},
  {27, "First Duplicate", "Caught the same species twice", CAT_COLLECTION, RARITY_UNCOMMON, 1, 3},
  {28, "Duplicate Collector", "Caught 100 duplicates", CAT_COLLECTION, RARITY_RARE, 100, 0},
  {29, "Rapid Catcher", "Caught 5 animals in one day", CAT_COLLECTION, RARITY_UNCOMMON, 5, 3},
  {30, "Prolific Catcher", "Caught 10 animals in one day", CAT_COLLECTION, RARITY_RARE, 10, 3},
  {31, "Catching Spree", "Caught 20 animals in one day", CAT_COLLECTION, RARITY_LEGENDARY, 20, 3},
  {32, "Rare Hunter", "Caught a rare-rarity animal", CAT_COLLECTION, RARITY_UNCOMMON, 1, 3},
  {33, "Legendary Hunter", "Caught a legendary-rarity animal", CAT_COLLECTION, RARITY_LEGENDARY, 1, 3},
  {34, "Lucky Catch", "Caught an animal with <5% catch rate", CAT_COLLECTION, RARITY_RARE, 1, 3},
};

// ============================================================================
// RESPECT ACHIEVEMENTS (15) - Progression milestones
// ============================================================================

static const AchievementMaster RESPECT_ACHS[] = {
  {35, "Growing Bond", "Reached Respect Level 2", CAT_RESPECT, RARITY_COMMON, 2, 1},
  {36, "Strong Connection", "Reached Respect Level 3", CAT_RESPECT, RARITY_COMMON, 3, 1},
  {37, "Deep Partnership", "Reached Respect Level 4", CAT_RESPECT, RARITY_UNCOMMON, 4, 1},
  {38, "Trusted Guardian", "Reached Respect Level 5", CAT_RESPECT, RARITY_UNCOMMON, 5, 1},
  {39, "Devoted Keeper", "Reached Respect Level 6", CAT_RESPECT, RARITY_UNCOMMON, 6, 1},
  {40, "Ultimate Guardian", "Reached Respect Level 7", CAT_RESPECT, RARITY_RARE, 7, 1},
  {41, "Legendary Bond", "Reached Respect Level 8 (MAX)", CAT_RESPECT, RARITY_LEGENDARY, 8, 1},
  {42, "100 Respect", "Earned 100 total respect points", CAT_RESPECT, RARITY_COMMON, 100, 0},
  {43, "500 Respect", "Earned 500 total respect points", CAT_RESPECT, RARITY_UNCOMMON, 500, 0},
  {44, "1000 Respect", "Earned 1,000 total respect points", CAT_RESPECT, RARITY_UNCOMMON, 1000, 0},
  {45, "5000 Respect", "Earned 5,000 total respect points", CAT_RESPECT, RARITY_RARE, 5000, 0},
  {46, "Perfect Care", "7 consecutive days of perfect pet care", CAT_RESPECT, RARITY_RARE, 7, 3},
  {47, "Devoted Caregiver", "30 days of consistent pet care", CAT_RESPECT, RARITY_RARE, 30, 3},
  {48, "Caring Master", "100 total care actions", CAT_RESPECT, RARITY_UNCOMMON, 100, 0},
  {49, "Care Legend", "1,000 total care actions", CAT_RESPECT, RARITY_LEGENDARY, 1000, 0},
};

// ============================================================================
// EVOLUTION ACHIEVEMENTS (12) - Pet stage progression
// ============================================================================

static const AchievementMaster EVOLUTION_ACHS[] = {
  {50, "Hatching", "Pet reached Hatchling stage", CAT_EVOLUTION, RARITY_COMMON, 1, 1},
  {51, "Growing Up", "Pet reached Young stage", CAT_EVOLUTION, RARITY_UNCOMMON, 2, 1},
  {52, "Coming of Age", "Pet reached Adult stage", CAT_EVOLUTION, RARITY_UNCOMMON, 3, 1},
  {53, "Wisdom Attained", "Pet reached Elder stage", CAT_EVOLUTION, RARITY_RARE, 4, 1},
  {54, "Immortal Bond", "Pet reached Legend stage", CAT_EVOLUTION, RARITY_LEGENDARY, 5, 1},
  {55, "Speed Evolver", "Pet reached Adult in under 10 days", CAT_EVOLUTION, RARITY_RARE, 10, 3},
  {56, "Rapid Growth", "Pet reached Elder in under 20 days", CAT_EVOLUTION, RARITY_RARE, 20, 3},
  {57, "Legendary Path", "Pet reached Legend stage in record time", CAT_EVOLUTION, RARITY_LEGENDARY, 30, 3},
  {58, "First Evolution", "Pet evolved to next stage for first time", CAT_EVOLUTION, RARITY_COMMON, 1, 3},
  {59, "Evolution Enthusiast", "Pet evolved 5 times total", CAT_EVOLUTION, RARITY_UNCOMMON, 5, 3},
  {60, "Evolution Master", "Own 5 Legend-stage pets", CAT_EVOLUTION, RARITY_LEGENDARY, 5, 3},
  {61, "Multi-Stage", "Own pets at all 6 evolution stages", CAT_EVOLUTION, RARITY_RARE, 6, 3},
};

// ============================================================================
// MINIGAME ACHIEVEMENTS (15) - Gaming skill
// ============================================================================

static const AchievementMaster MINIGAME_ACHS[] = {
  {62, "Gamer", "Won 1 minigame", CAT_MINIGAME, RARITY_COMMON, 1, 0},
  {63, "Game Enthusiast", "Won 10 minigames", CAT_MINIGAME, RARITY_COMMON, 10, 0},
  {64, "Game Master", "Won 50 minigames", CAT_MINIGAME, RARITY_UNCOMMON, 50, 0},
  {65, "Arcade Champion", "Won 100 minigames", CAT_MINIGAME, RARITY_UNCOMMON, 100, 0},
  {66, "Legendary Gamer", "Won 500 minigames", CAT_MINIGAME, RARITY_LEGENDARY, 500, 0},
  {67, "Perfect Win", "Won a minigame with perfect score", CAT_MINIGAME, RARITY_RARE, 1, 3},
  {68, "Speed Racer", "Won 5 minigames in under 10 seconds each", CAT_MINIGAME, RARITY_RARE, 5, 3},
  {69, "Win Streak", "Won 5 minigames in a row", CAT_MINIGAME, RARITY_UNCOMMON, 5, 3},
  {70, "Epic Streak", "Won 10 minigames in a row", CAT_MINIGAME, RARITY_RARE, 10, 3},
  {71, "Legendary Streak", "Won 20 minigames in a row", CAT_MINIGAME, RARITY_LEGENDARY, 20, 3},
  {72, "All Games Mastered", "Won all 12 minigames at least once", CAT_MINIGAME, RARITY_UNCOMMON, 12, 3},
  {73, "Flawless Set", "Won 5 consecutive games with perfect scores", CAT_MINIGAME, RARITY_LEGENDARY, 5, 3},
  {74, "High Roller", "Earned 1,000 coins from minigames", CAT_MINIGAME, RARITY_UNCOMMON, 1000, 0},
  {75, "Rich Gamer", "Earned 5,000 coins from minigames", CAT_MINIGAME, RARITY_RARE, 5000, 0},
  {76, "Vault Master", "Earned 10,000 coins from minigames", CAT_MINIGAME, RARITY_LEGENDARY, 10000, 0},
};

// ============================================================================
// WALKING ACHIEVEMENTS (12) - Distance challenges
// ============================================================================

static const AchievementMaster WALKING_ACHS[] = {
  {77, "First Steps", "Walked 1,000 steps", CAT_WALKING, RARITY_COMMON, 1, 2},
  {78, "Jogger", "Walked 10,000 steps", CAT_WALKING, RARITY_COMMON, 10, 2},
  {79, "Power Walker", "Walked 50,000 steps", CAT_WALKING, RARITY_UNCOMMON, 50, 2},
  {80, "Marathon Runner", "Walked 100,000 steps", CAT_WALKING, RARITY_UNCOMMON, 100, 2},
  {81, "Ultra Runner", "Walked 500,000 steps", CAT_WALKING, RARITY_RARE, 500, 2},
  {82, "Legend Walker", "Walked 1,000,000 steps", CAT_WALKING, RARITY_LEGENDARY, 1000, 2},
  {83, "Daily Walker", "Walked 20,000 steps in one day", CAT_WALKING, RARITY_UNCOMMON, 20, 2},
  {84, "Marathon Day", "Walked 50,000 steps in one day", CAT_WALKING, RARITY_RARE, 50, 2},
  {85, "Walking Habit", "7 consecutive days of 10,000+ steps", CAT_WALKING, RARITY_RARE, 7, 3},
  {86, "Monthly Jogger", "30 consecutive days of 10,000+ steps", CAT_WALKING, RARITY_LEGENDARY, 30, 3},
  {87, "Active Lifestyle", "Average 15,000 steps per day", CAT_WALKING, RARITY_RARE, 15, 3},
  {88, "Fitness Champion", "Average 25,000 steps per day", CAT_WALKING, RARITY_LEGENDARY, 25, 3},
};

// ============================================================================
// HABITAT ACHIEVEMENTS (15) - Habitat mastery
// ============================================================================

static const AchievementMaster HABITAT_ACHS[] = {
  {89, "African Expert", "Master all African animals", CAT_HABITAT, RARITY_RARE, 25, 3},
  {90, "Asian Scholar", "Master all Asian animals", CAT_HABITAT, RARITY_RARE, 25, 3},
  {91, "European Naturalist", "Master all European animals", CAT_HABITAT, RARITY_RARE, 25, 3},
  {92, "Rainforest Sage", "Master all Rainforest animals", CAT_HABITAT, RARITY_RARE, 25, 3},
  {93, "Arctic Master", "Master all Arctic animals", CAT_HABITAT, RARITY_RARE, 25, 3},
  {94, "Ocean Master", "Master all Ocean animals", CAT_HABITAT, RARITY_RARE, 25, 3},
  {95, "Habitat Collector", "Master 3 habitats", CAT_HABITAT, RARITY_UNCOMMON, 3, 3},
  {96, "Habitat Expert", "Master 6 habitats", CAT_HABITAT, RARITY_RARE, 6, 3},
  {97, "Habitat Master", "Master all 11 habitats", CAT_HABITAT, RARITY_LEGENDARY, 11, 3},
  {98, "Extinct Historian", "Learn facts about all extinct animals", CAT_HABITAT, RARITY_LEGENDARY, 25, 3},
  {99, "Educational", "Learn 100 animal facts", CAT_HABITAT, RARITY_UNCOMMON, 100, 0},
  {100, "Living Encyclopedia", "Learn 500 animal facts", CAT_HABITAT, RARITY_RARE, 500, 0},
  {101, "Knowledge Master", "Learn 1,000+ animal facts", CAT_HABITAT, RARITY_LEGENDARY, 1000, 0},
  {102, "First Habitat Expert", "Reach habitat mastery in any habitat", CAT_HABITAT, RARITY_UNCOMMON, 1, 3},
  {103, "Diverse Learner", "Learn facts about animals from all 11 habitats", CAT_HABITAT, RARITY_RARE, 11, 3},
};

// ============================================================================
// CHALLENGE ACHIEVEMENTS (20) - Difficult accomplishments
// ============================================================================

static const AchievementMaster CHALLENGE_ACHS[] = {
  {104, "Lucky Day", "Caught 3 animals in one day", CAT_CHALLENGE, RARITY_UNCOMMON, 3, 3},
  {105, "Fortunate Week", "Caught 15 animals in one week", CAT_CHALLENGE, RARITY_UNCOMMON, 15, 3},
  {106, "Unstoppable", "Caught 30 animals in one week", CAT_CHALLENGE, RARITY_RARE, 30, 3},
  {107, "Golden Touch", "Caught 5 animals with <10% success rate", CAT_CHALLENGE, RARITY_LEGENDARY, 5, 3},
  {108, "Perfect Week", "Completed all 7 weekly challenges", CAT_CHALLENGE, RARITY_UNCOMMON, 7, 3},
  {109, "Perfect Month", "Completed weekly challenges 4 weeks in a row", CAT_CHALLENGE, RARITY_RARE, 4, 3},
  {110, "Challenge Master", "Completed 50 weeks of challenges", CAT_CHALLENGE, RARITY_LEGENDARY, 50, 3},
  {111, "No Sweat", "Won 3 minigames with no misses", CAT_CHALLENGE, RARITY_RARE, 3, 3},
  {112, "Wealthy Trainer", "Earned 1,000 coins total", CAT_CHALLENGE, RARITY_COMMON, 1000, 0},
  {113, "Rich Trainer", "Earned 5,000 coins total", CAT_CHALLENGE, RARITY_UNCOMMON, 5000, 0},
  {114, "Millionaire", "Earned 50,000 coins total", CAT_CHALLENGE, RARITY_RARE, 50000, 0},
  {115, "Token Enthusiast", "Acquired 100 tokens", CAT_CHALLENGE, RARITY_UNCOMMON, 100, 0},
  {116, "Token Master", "Acquired 500 tokens", CAT_CHALLENGE, RARITY_RARE, 500, 0},
  {117, "Token Legend", "Acquired 2,000 tokens", CAT_CHALLENGE, RARITY_LEGENDARY, 2000, 0},
  {118, "Guild Founder", "Created a guild", CAT_CHALLENGE, RARITY_UNCOMMON, 1, 3},
  {119, "Guild Leader", "Lead a guild with 20+ members", CAT_CHALLENGE, RARITY_UNCOMMON, 20, 3},
  {120, "Guild Master", "Reach guild level 5", CAT_CHALLENGE, RARITY_RARE, 5, 3},
  {121, "Top Trainer", "Reach top 10 in your leaderboard segment", CAT_CHALLENGE, RARITY_RARE, 10, 3},
  {122, "Legendary Trainer", "Reach #1 in your leaderboard segment", CAT_CHALLENGE, RARITY_LEGENDARY, 1, 3},
  {123, "Collection Conqueror", "Reach each collection tier (Explorer → Keeper)", CAT_CHALLENGE, RARITY_LEGENDARY, 4, 3},
};

// ============================================================================
// TIME-BASED ACHIEVEMENTS (12) - Milestones over time
// ============================================================================

static const AchievementMaster TIME_ACHS[] = {
  {124, "First Day", "Played for 1 day", CAT_TIME, RARITY_COMMON, 1, 3},
  {125, "First Week", "Played for 7 days", CAT_TIME, RARITY_COMMON, 7, 3},
  {126, "One Month Champion", "Played for 30 days", CAT_TIME, RARITY_UNCOMMON, 30, 3},
  {127, "One Hundred Days", "Played for 100 days", CAT_TIME, RARITY_UNCOMMON, 100, 3},
  {128, "Six Months", "Played for 180 days", CAT_TIME, RARITY_RARE, 180, 3},
  {129, "One Year Master", "Played for 365 days", CAT_TIME, RARITY_LEGENDARY, 365, 3},
  {130, "Daily Enthusiast", "Played 30 consecutive days", CAT_TIME, RARITY_UNCOMMON, 30, 3},
  {131, "Devoted Player", "Played 100 consecutive days", CAT_TIME, RARITY_RARE, 100, 3},
  {132, "Unbreakable Habit", "Played 365 consecutive days", CAT_TIME, RARITY_LEGENDARY, 365, 3},
  {133, "Early Bird", "Played between 6am-8am", CAT_TIME, RARITY_RARE, 1, 3},
  {134, "Night Owl", "Played between midnight-2am", CAT_TIME, RARITY_RARE, 1, 3},
  {135, "All Hours", "Played in all 24 hours", CAT_TIME, RARITY_LEGENDARY, 24, 3},
};

// ============================================================================
// SPECIAL/HIDDEN ACHIEVEMENTS (14) - Secret achievements
// ============================================================================

static const AchievementMaster SPECIAL_ACHS[] = {
  {136, "Serendipity", "Found 2 of same species in one day (hidden)", CAT_SPECIAL, RARITY_LEGENDARY, 1, 3},
  {137, "Lucky Streak", "Caught 3 animals in a row without missing (hidden)", CAT_SPECIAL, RARITY_RARE, 3, 3},
  {138, "Patience", "Wait 1 hour without clicking anything", CAT_SPECIAL, RARITY_LEGENDARY, 1, 3},
  {139, "Marathon Session", "Play for 5+ hours straight (hidden)", CAT_SPECIAL, RARITY_RARE, 5, 3},
  {140, "Balanced Life", "Play exactly 1 hour per day for a month (hidden)", CAT_SPECIAL, RARITY_RARE, 30, 3},
  {141, "Nocturnal", "Played 10+ times between midnight-6am (hidden)", CAT_SPECIAL, RARITY_RARE, 10, 3},
  {142, "Weekend Warrior", "Caught 25 animals on weekends only (hidden)", CAT_SPECIAL, RARITY_UNCOMMON, 25, 3},
  {143, "Perfect Symmetry", "Reach exactly 500 respect at exactly 500 animals (hidden)", CAT_SPECIAL, RARITY_LEGENDARY, 1, 3},
  {144, "Unlikely Winner", "Catch an animal with 1% success rate (hidden)", CAT_SPECIAL, RARITY_LEGENDARY, 1, 3},
  {145, "Speedrunner", "Complete first 100 animals in under 50 days (hidden)", CAT_SPECIAL, RARITY_RARE, 50, 3},
  {146, "Procrastinator", "Complete game achievements in the last day of month (hidden)", CAT_SPECIAL, RARITY_LEGENDARY, 1, 3},
  {147, "Secret Hunter", "Unlock 10 hidden achievements (hidden)", CAT_SPECIAL, RARITY_RARE, 10, 3},
  {148, "Achievement Master", "Unlock 100 achievements total", CAT_SPECIAL, RARITY_LEGENDARY, 100, 0},
  {149, "Perfectionist", "Unlock all 150 achievements", CAT_SPECIAL, RARITY_LEGENDARY, 150, 0},
};

// ============================================================================
// Combined achievement table
// ============================================================================

#define ACH_TOTAL 150

struct AchievementMasterState {
  bool unlocked[150];
  uint32_t unlocked_at[150];
  uint16_t total_unlocked;
  uint8_t category_progress[10];  // Unlocked per category
};

// ============================================================================
// Initialize master achievements
// ============================================================================
static inline void achievements_master_init(AchievementMasterState *state) {
  for (int i = 0; i < ACH_TOTAL; i++) {
    state->unlocked[i] = false;
    state->unlocked_at[i] = 0;
  }
  state->total_unlocked = 0;
  for (int i = 0; i < 10; i++) {
    state->category_progress[i] = 0;
  }
}

// ============================================================================
// Unlock achievement
// ============================================================================
static inline bool achievements_master_unlock(AchievementMasterState *state, uint8_t ach_id) {
  if (ach_id >= ACH_TOTAL) return false;
  if (state->unlocked[ach_id]) return false;

  state->unlocked[ach_id] = true;
  state->unlocked_at[ach_id] = time(NULL);
  state->total_unlocked++;
  return true;
}

// ============================================================================
// Get total unlocked
// ============================================================================
static inline uint16_t achievements_master_get_total(const AchievementMasterState *state) {
  return state->total_unlocked;
}

// ============================================================================
// Get percentage unlocked
// ============================================================================
static inline uint8_t achievements_master_get_percent(const AchievementMasterState *state) {
  if (ACH_TOTAL == 0) return 100;
  return (state->total_unlocked * 100) / ACH_TOTAL;
}

// ============================================================================
// Helper: Get rarity color
// ============================================================================
static inline uint16_t achievements_master_get_color(uint8_t rarity) {
  switch (rarity) {
    case RARITY_COMMON:
      return 0xFFFF;   // White
    case RARITY_UNCOMMON:
      return 0x0FFF;   // Cyan
    case RARITY_RARE:
      return 0xF01F;   // Magenta
    case RARITY_LEGENDARY:
      return 0xFFC0;   // Yellow
    default:
      return 0x8410;   // Gray
  }
}

// ============================================================================
// Helper: Get category name
// ============================================================================
static inline const char *achievements_master_get_category_name(uint8_t category) {
  switch (category) {
    case CAT_DISCOVERY:
      return "Discovery";
    case CAT_COLLECTION:
      return "Collection";
    case CAT_RESPECT:
      return "Respect";
    case CAT_EVOLUTION:
      return "Evolution";
    case CAT_MINIGAME:
      return "Minigames";
    case CAT_WALKING:
      return "Walking";
    case CAT_HABITAT:
      return "Habitats";
    case CAT_CHALLENGE:
      return "Challenges";
    case CAT_TIME:
      return "Time-based";
    case CAT_SPECIAL:
      return "Special";
    default:
      return "Unknown";
  }
}

// ============================================================================
// Get category count
// ============================================================================
static inline uint8_t achievements_master_get_category_count(const AchievementMasterState *state,
                                                              uint8_t category) {
  uint8_t count = 0;
  // This would iterate through master achievement table and count by category
  // Simplified version for this implementation
  return count;
}
