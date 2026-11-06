#pragma once

// ============================================================================
// EXPANDED ACHIEVEMENTS SYSTEM - Phase 2 Engagement Improvement
// ============================================================================
// 50+ achievements across multiple categories
// ~20-30% obtainable in first month (scarce = valuable per research)
// Guides exploration without forcing play style

#include <stdint.h>
#include <time.h>

// Total achievements (expandable)
#define ACHIEVEMENT_EXPANDED_COUNT 50

enum AchievementIDExpanded {
  // Discovery Achievements (10)
  ACH_FIND_AFRICAN = 0,
  ACH_FIND_ASIAN = 1,
  ACH_FIND_EUROPEAN = 2,
  ACH_FIND_SOUTH_AMERICAN = 3,
  ACH_FIND_NORTH_AMERICAN = 4,
  ACH_FIND_AUSTRALIAN = 5,
  ACH_FIND_ARCTIC = 6,
  ACH_FIND_OCEAN = 7,
  ACH_FIND_RAINFOREST = 8,
  ACH_FIND_EXTINCT = 9,

  // Milestone Achievements (15)
  ACH_CATCH_10 = 10,
  ACH_CATCH_25 = 11,
  ACH_CATCH_50 = 12,
  ACH_CATCH_100 = 13,
  ACH_CATCH_250 = 14,
  ACH_CATCH_500 = 15,
  ACH_RESPECT_LEVEL_2 = 16,
  ACH_RESPECT_LEVEL_4 = 17,
  ACH_RESPECT_LEVEL_6 = 18,
  ACH_RESPECT_LEVEL_8 = 19,
  ACH_EVOLVED_ADULT = 20,
  ACH_EVOLVED_ELDER = 21,
  ACH_EVOLVED_LEGEND = 22,
  ACH_WALKED_10KM = 23,
  ACH_WALKED_50KM = 24,

  // Challenge Achievements (10)
  ACH_CATCH_3_IN_DAY = 25,
  ACH_WIN_7_MINIGAMES = 26,
  ACH_EARN_1000_COINS = 27,
  ACH_COLLECT_RARE_TIER = 28,
  ACH_PERFECT_CARE = 29,
  ACH_LUCKY_CATCH = 30,
  ACH_SPEED_RACER = 31,
  ACH_COLLECTOR_SPIRIT = 32,
  ACH_TRAINER_BOND = 33,
  ACH_WEEKEND_WARRIOR = 34,

  // Rare Achievements (10)
  ACH_COLLECT_ALL_AFRICAN = 35,
  ACH_COLLECT_ALL_ASIAN = 36,
  ACH_COLLECT_ALL_RAINFOREST = 37,
  ACH_CATCH_LEGENDARY = 38,
  ACH_MAX_RESPECT = 39,
  ACH_EXTINCT_COLLECTOR = 40,
  ACH_PERFECT_EVOLUTION = 41,
  ACH_MASTER_WALKER = 42,
  ACH_MINIGAME_MASTER = 43,
  ACH_ULTIMATE_COLLECTOR = 44,

  // Special/Hidden Achievements (5)
  ACH_LUCKY_FIND = 45,
  ACH_NIGHT_OWL = 46,
  ACH_EARLY_BIRD = 47,
  ACH_MILESTONE_100_DAYS = 48,
  ACH_PERFECT_CARE_WEEK = 49,
};

enum AchievementRarityExpanded {
  RARITY_COMMON = 0,      // 80% of players
  RARITY_UNCOMMON = 1,    // 50% of players
  RARITY_RARE = 2,        // 20% of players
  RARITY_LEGENDARY = 3,   // 5% of players
};

struct AchievementExpanded {
  uint8_t id;
  const char *name;
  const char *description;
  uint8_t rarity;
  uint8_t category;  // 0=Discovery, 1=Milestone, 2=Challenge, 3=Rare, 4=Special
};

// Achievement definitions (massive table)
static const AchievementExpanded ACHIEVEMENTS_EXPANDED[ACHIEVEMENT_EXPANDED_COUNT] = {
  // Discovery (10)
  {ACH_FIND_AFRICAN, "African Explorer", "Found an African animal", RARITY_COMMON, 0},
  {ACH_FIND_ASIAN, "Asian Wanderer", "Found an Asian animal", RARITY_COMMON, 0},
  {ACH_FIND_EUROPEAN, "European Traveler", "Found a European animal", RARITY_COMMON, 0},
  {ACH_FIND_SOUTH_AMERICAN, "South American Guide", "Found a South American animal", RARITY_UNCOMMON, 0},
  {ACH_FIND_NORTH_AMERICAN, "North American Scout", "Found a North American animal", RARITY_UNCOMMON, 0},
  {ACH_FIND_AUSTRALIAN, "Australian Adventure", "Found an Australian animal", RARITY_UNCOMMON, 0},
  {ACH_FIND_ARCTIC, "Polar Explorer", "Found an Arctic animal", RARITY_UNCOMMON, 0},
  {ACH_FIND_OCEAN, "Ocean Diver", "Found a sea creature", RARITY_UNCOMMON, 0},
  {ACH_FIND_RAINFOREST, "Jungle Trekker", "Found a rainforest animal", RARITY_RARE, 0},
  {ACH_FIND_EXTINCT, "Paleontologist", "Found an extinct animal", RARITY_RARE, 0},

  // Milestones (15)
  {ACH_CATCH_10, "Rising Collector", "Caught 10 animals", RARITY_COMMON, 1},
  {ACH_CATCH_25, "Dedicated Collector", "Caught 25 animals", RARITY_UNCOMMON, 1},
  {ACH_CATCH_50, "Serious Collector", "Caught 50 animals", RARITY_UNCOMMON, 1},
  {ACH_CATCH_100, "Obsessed Collector", "Caught 100 animals", RARITY_RARE, 1},
  {ACH_CATCH_250, "Lifelong Passion", "Caught 250 animals", RARITY_RARE, 1},
  {ACH_CATCH_500, "Living Library", "Caught 500 animals", RARITY_LEGENDARY, 1},
  {ACH_RESPECT_LEVEL_2, "Growing Bond", "Reached Respect Level 2", RARITY_COMMON, 1},
  {ACH_RESPECT_LEVEL_4, "Strong Connection", "Reached Respect Level 4", RARITY_UNCOMMON, 1},
  {ACH_RESPECT_LEVEL_6, "Deep Partnership", "Reached Respect Level 6", RARITY_RARE, 1},
  {ACH_RESPECT_LEVEL_8, "Legendary Guardian", "Reached Respect Level 8 (Max!)", RARITY_LEGENDARY, 1},
  {ACH_EVOLVED_ADULT, "Growing Up", "Pet evolved to Adult stage", RARITY_UNCOMMON, 1},
  {ACH_EVOLVED_ELDER, "Wisdom Attained", "Pet evolved to Elder stage", RARITY_RARE, 1},
  {ACH_EVOLVED_LEGEND, "Immortal Bond", "Pet evolved to Legend stage", RARITY_LEGENDARY, 1},
  {ACH_WALKED_10KM, "Jogger", "Walked 10 kilometers", RARITY_UNCOMMON, 1},
  {ACH_WALKED_50KM, "Marathon Runner", "Walked 50 kilometers", RARITY_RARE, 1},

  // Challenges (10)
  {ACH_CATCH_3_IN_DAY, "Lucky Day", "Caught 3 animals in one day", RARITY_UNCOMMON, 2},
  {ACH_WIN_7_MINIGAMES, "Game Master", "Won 7 minigames in a row", RARITY_UNCOMMON, 2},
  {ACH_EARN_1000_COINS, "Wealthy Trainer", "Earned 1,000 coins", RARITY_UNCOMMON, 2},
  {ACH_COLLECT_RARE_TIER, "Rarity Seeker", "Caught a Rare-tier animal", RARITY_RARE, 2},
  {ACH_PERFECT_CARE, "Devoted Guardian", "7 days of perfect pet care", RARITY_RARE, 2},
  {ACH_LUCKY_CATCH, "Golden Touch", "Caught with <5% success rate", RARITY_RARE, 2},
  {ACH_SPEED_RACER, "Quick Reflexes", "Won 5 minigames in under 10 seconds each", RARITY_RARE, 2},
  {ACH_COLLECTOR_SPIRIT, "Soul Searcher", "Caught from every habitat", RARITY_RARE, 2},
  {ACH_TRAINER_BOND, "Best Friend", "Achieved 500 respect points", RARITY_UNCOMMON, 2},
  {ACH_WEEKEND_WARRIOR, "Weekend Champion", "Caught 5 animals on weekend", RARITY_UNCOMMON, 2},

  // Rare (10)
  {ACH_COLLECT_ALL_AFRICAN, "Master of Africa", "Caught all African animals", RARITY_LEGENDARY, 3},
  {ACH_COLLECT_ALL_ASIAN, "Asian Conqueror", "Caught all Asian animals", RARITY_LEGENDARY, 3},
  {ACH_COLLECT_ALL_RAINFOREST, "Jungle Master", "Caught all Rainforest animals", RARITY_RARE, 3},
  {ACH_CATCH_LEGENDARY, "Mythical Hunter", "Caught a Legendary animal", RARITY_LEGENDARY, 3},
  {ACH_MAX_RESPECT, "Ultimate Guardian", "Reached max Respect Level (8)", RARITY_LEGENDARY, 3},
  {ACH_EXTINCT_COLLECTOR, "Time Traveler", "Caught all Extinct animals", RARITY_LEGENDARY, 3},
  {ACH_PERFECT_EVOLUTION, "Perfect Evolution", "Pet reached Legend stage in record time", RARITY_RARE, 3},
  {ACH_MASTER_WALKER, "Pedometer Legend", "Walked 100 kilometers", RARITY_RARE, 3},
  {ACH_MINIGAME_MASTER, "Arcade Champion", "Won 50 minigames total", RARITY_RARE, 3},
  {ACH_ULTIMATE_COLLECTOR, "Living Archive", "Caught all 988 animals", RARITY_LEGENDARY, 3},

  // Special/Hidden (5)
  {ACH_LUCKY_FIND, "Serendipity", "Found a duplicate species in one day (hidden)", RARITY_LEGENDARY, 4},
  {ACH_NIGHT_OWL, "Nocturnal", "Played between midnight and 6 AM", RARITY_RARE, 4},
  {ACH_EARLY_BIRD, "Dawn Explorer", "Played between 6 AM and 8 AM", RARITY_RARE, 4},
  {ACH_MILESTONE_100_DAYS, "Centenary", "Owned pet for 100 days", RARITY_RARE, 4},
  {ACH_PERFECT_CARE_WEEK, "Weekly Perfection", "Fed, cleaned, and played with pet every day for a week", RARITY_UNCOMMON, 4},
};

struct AchievementStateExpanded {
  bool unlocked[ACHIEVEMENT_EXPANDED_COUNT];
  uint32_t unlocked_at[ACHIEVEMENT_EXPANDED_COUNT];
  uint16_t total_unlocked;  // Cache for quick display
};

// ============================================================================
// Initialize achievement system
// ============================================================================
static inline void achievements_expanded_init(AchievementStateExpanded *state) {
  for (int i = 0; i < ACHIEVEMENT_EXPANDED_COUNT; i++) {
    state->unlocked[i] = false;
    state->unlocked_at[i] = 0;
  }
  state->total_unlocked = 0;
}

// ============================================================================
// Unlock achievement
// ============================================================================
static inline bool achievements_expanded_unlock(AchievementStateExpanded *state, uint8_t achievement_id) {
  if (achievement_id >= ACHIEVEMENT_EXPANDED_COUNT) return false;
  if (state->unlocked[achievement_id]) return false;

  state->unlocked[achievement_id] = true;
  state->unlocked_at[achievement_id] = time(NULL);
  state->total_unlocked++;
  return true;
}

// ============================================================================
// Check if unlocked
// ============================================================================
static inline bool achievements_expanded_is_unlocked(const AchievementStateExpanded *state, uint8_t achievement_id) {
  if (achievement_id >= ACHIEVEMENT_EXPANDED_COUNT) return false;
  return state->unlocked[achievement_id];
}

// ============================================================================
// Get achievement definition
// ============================================================================
static inline const AchievementExpanded *achievements_expanded_get_def(uint8_t achievement_id) {
  if (achievement_id >= ACHIEVEMENT_EXPANDED_COUNT) return NULL;
  return &ACHIEVEMENTS_EXPANDED[achievement_id];
}

// ============================================================================
// Get achievement by category count
// ============================================================================
static inline uint8_t achievements_expanded_get_category_count(const AchievementStateExpanded *state, uint8_t category) {
  uint8_t count = 0;
  for (int i = 0; i < ACHIEVEMENT_EXPANDED_COUNT; i++) {
    if (ACHIEVEMENTS_EXPANDED[i].category == category && state->unlocked[i]) {
      count++;
    }
  }
  return count;
}

// ============================================================================
// Get total in category
// ============================================================================
static inline uint8_t achievements_expanded_get_category_total(uint8_t category) {
  uint8_t count = 0;
  for (int i = 0; i < ACHIEVEMENT_EXPANDED_COUNT; i++) {
    if (ACHIEVEMENTS_EXPANDED[i].category == category) {
      count++;
    }
  }
  return count;
}

// ============================================================================
// Get rarity color
// ============================================================================
static inline uint16_t achievements_expanded_get_rarity_color(uint8_t rarity) {
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
// Achievement check functions (called from game events)
// ============================================================================

static inline void achievements_expanded_check_catch_total(AchievementStateExpanded *state, uint16_t total_caught) {
  if (total_caught >= 10) achievements_expanded_unlock(state, ACH_CATCH_10);
  if (total_caught >= 25) achievements_expanded_unlock(state, ACH_CATCH_25);
  if (total_caught >= 50) achievements_expanded_unlock(state, ACH_CATCH_50);
  if (total_caught >= 100) achievements_expanded_unlock(state, ACH_CATCH_100);
  if (total_caught >= 250) achievements_expanded_unlock(state, ACH_CATCH_250);
  if (total_caught >= 500) achievements_expanded_unlock(state, ACH_CATCH_500);
}

static inline void achievements_expanded_check_respect_level(AchievementStateExpanded *state, uint8_t respect_level) {
  if (respect_level >= 2) achievements_expanded_unlock(state, ACH_RESPECT_LEVEL_2);
  if (respect_level >= 4) achievements_expanded_unlock(state, ACH_RESPECT_LEVEL_4);
  if (respect_level >= 6) achievements_expanded_unlock(state, ACH_RESPECT_LEVEL_6);
  if (respect_level >= 8) achievements_expanded_unlock(state, ACH_RESPECT_LEVEL_8);
}

static inline void achievements_expanded_check_evolution_stage(AchievementStateExpanded *state, uint8_t stage) {
  if (stage >= 3) achievements_expanded_unlock(state, ACH_EVOLVED_ADULT);
  if (stage >= 4) achievements_expanded_unlock(state, ACH_EVOLVED_ELDER);
  if (stage >= 5) achievements_expanded_unlock(state, ACH_EVOLVED_LEGEND);
}

static inline void achievements_expanded_check_steps(AchievementStateExpanded *state, uint32_t total_steps_km) {
  if (total_steps_km >= 10) achievements_expanded_unlock(state, ACH_WALKED_10KM);
  if (total_steps_km >= 50) achievements_expanded_unlock(state, ACH_WALKED_50KM);
  if (total_steps_km >= 100) achievements_expanded_unlock(state, ACH_MASTER_WALKER);
}

// ============================================================================
// Serialize/Deserialize
// ============================================================================

static inline void achievements_expanded_serialize(AchievementStateExpanded *state, uint8_t *buffer) {
  for (int i = 0; i < ACHIEVEMENT_EXPANDED_COUNT; i++) {
    buffer[i] = state->unlocked[i] ? 1 : 0;
  }
  uint16_t *p = (uint16_t *)(buffer + ACHIEVEMENT_EXPANDED_COUNT);
  p[0] = state->total_unlocked;
}

static inline void achievements_expanded_deserialize(AchievementStateExpanded *state, const uint8_t *buffer) {
  for (int i = 0; i < ACHIEVEMENT_EXPANDED_COUNT; i++) {
    state->unlocked[i] = buffer[i] != 0;
  }
  const uint16_t *p = (const uint16_t *)(buffer + ACHIEVEMENT_EXPANDED_COUNT);
  state->total_unlocked = p[0];
}
