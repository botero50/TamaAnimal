#pragma once

// ============================================================================
// ACHIEVEMENTS SYSTEM - Phase 1 Engagement Improvement
// ============================================================================
// Track and display player achievements to guide exploration
// Research shows achievements increase engagement by setting explicit goals
// Only ~20% should be obtainable in first month (scarce = valuable)

#include <stdint.h>
#include <string.h>

// Achievement IDs (first 5 in Phase 1, expandable to 50+ in Phase 2)
#define ACHIEVEMENT_COUNT 5

enum AchievementID {
  ACH_FIRST_PET = 0,        // Caught starter animal
  ACH_EXPLORER = 1,         // Caught 5 animals
  ACH_LEARNER = 2,          // Earned 100 respect
  ACH_TOKEN_MASTER = 3,     // Purchased from shop
  ACH_WALKER = 4,           // Walked 5,000 steps
};

// Achievement rarity (visual indicator)
enum AchievementRarity {
  RARITY_COMMON = 0,      // 80% of players unlock
  RARITY_UNCOMMON = 1,    // 50% of players
  RARITY_RARE = 2,        // 20% of players
  RARITY_LEGENDARY = 3,   // 5% of players
};

struct Achievement {
  uint8_t id;
  const char *name;
  const char *description;
  uint8_t rarity;
  bool unlocked;
  uint32_t unlocked_at;  // Unix timestamp when earned
};

// Achievement definitions table
static const Achievement ACHIEVEMENT_DEFS[ACHIEVEMENT_COUNT] = {
  {
    ACH_FIRST_PET,
    "First Pet",
    "Caught your starter animal",
    RARITY_COMMON,
    false,
    0,
  },
  {
    ACH_EXPLORER,
    "Explorer",
    "Caught 5 different animals",
    RARITY_COMMON,
    false,
    0,
  },
  {
    ACH_LEARNER,
    "Learner",
    "Earned 100 respect points",
    RARITY_UNCOMMON,
    false,
    0,
  },
  {
    ACH_TOKEN_MASTER,
    "Token Master",
    "Purchased a token from the shop",
    RARITY_UNCOMMON,
    false,
    0,
  },
  {
    ACH_WALKER,
    "Walker",
    "Walked 5,000 steps",
    RARITY_UNCOMMON,
    false,
    0,
  },
};

struct AchievementState {
  bool unlocked[ACHIEVEMENT_COUNT];
  uint32_t unlocked_at[ACHIEVEMENT_COUNT];
};

// ============================================================================
// Initialize achievement system
// ============================================================================
static inline void achievements_init(AchievementState *state) {
  for (int i = 0; i < ACHIEVEMENT_COUNT; i++) {
    state->unlocked[i] = false;
    state->unlocked_at[i] = 0;
  }
}

// ============================================================================
// Check if achievement is unlocked
// ============================================================================
static inline bool achievements_is_unlocked(AchievementState *state, uint8_t achievement_id) {
  if (achievement_id >= ACHIEVEMENT_COUNT) return false;
  return state->unlocked[achievement_id];
}

// ============================================================================
// Unlock an achievement
// ============================================================================
static inline bool achievements_unlock(AchievementState *state, uint8_t achievement_id) {
  if (achievement_id >= ACHIEVEMENT_COUNT) return false;
  if (state->unlocked[achievement_id]) return false;  // Already unlocked

  state->unlocked[achievement_id] = true;
  state->unlocked_at[achievement_id] = time(NULL);
  return true;  // Newly unlocked
}

// ============================================================================
// Get achievement metadata by ID
// ============================================================================
static inline const Achievement *achievements_get_def(uint8_t achievement_id) {
  if (achievement_id >= ACHIEVEMENT_COUNT) return NULL;
  return &ACHIEVEMENT_DEFS[achievement_id];
}

// ============================================================================
// Get rarity color for display (16-bit RGB565)
// ============================================================================
static inline uint16_t achievements_get_rarity_color(uint8_t rarity) {
  switch (rarity) {
    case RARITY_COMMON:
      return 0xFFFF;  // White
    case RARITY_UNCOMMON:
      return 0x0FFF;  // Cyan
    case RARITY_RARE:
      return 0xF01F;  // Magenta
    case RARITY_LEGENDARY:
      return 0xFFC0;  // Yellow
    default:
      return 0x8410;  // Gray
  }
}

// ============================================================================
// Get unlock percentage (for display)
// Simulated based on rarity (in real game, track per-player stats)
// ============================================================================
static inline uint8_t achievements_get_unlock_percent(uint8_t achievement_id) {
  if (achievement_id >= ACHIEVEMENT_COUNT) return 0;
  const Achievement *def = achievements_get_def(achievement_id);
  if (!def) return 0;

  // Return percentage based on rarity
  switch (def->rarity) {
    case RARITY_COMMON:
      return 80;
    case RARITY_UNCOMMON:
      return 50;
    case RARITY_RARE:
      return 20;
    case RARITY_LEGENDARY:
      return 5;
    default:
      return 0;
  }
}

// ============================================================================
// Get total unlocked count
// ============================================================================
static inline uint8_t achievements_get_unlocked_count(AchievementState *state) {
  uint8_t count = 0;
  for (int i = 0; i < ACHIEVEMENT_COUNT; i++) {
    if (state->unlocked[i]) count++;
  }
  return count;
}

// ============================================================================
// Serialize for NVS storage (7 bytes per achievement)
// ============================================================================
static inline void achievements_serialize(AchievementState *state, uint8_t *buffer) {
  for (int i = 0; i < ACHIEVEMENT_COUNT; i++) {
    buffer[i * 5] = state->unlocked[i] ? 1 : 0;
    uint32_t *timestamp = (uint32_t *)(buffer + i * 5 + 1);
    *timestamp = state->unlocked_at[i];
  }
}

// ============================================================================
// Deserialize from NVS storage
// ============================================================================
static inline void achievements_deserialize(AchievementState *state, const uint8_t *buffer) {
  for (int i = 0; i < ACHIEVEMENT_COUNT; i++) {
    state->unlocked[i] = buffer[i * 5] != 0;
    const uint32_t *timestamp = (const uint32_t *)(buffer + i * 5 + 1);
    state->unlocked_at[i] = *timestamp;
  }
}

// ============================================================================
// Check and unlock specific achievements based on game state
// (Called from various game systems)
// ============================================================================

// Called when a new animal is caught
static inline void achievements_check_catch(AchievementState *state, uint16_t total_caught) {
  // ACH_FIRST_PET: Caught starter (total_caught >= 1)
  if (total_caught >= 1) {
    achievements_unlock(state, ACH_FIRST_PET);
  }
  // ACH_EXPLORER: Caught 5 animals
  if (total_caught >= 5) {
    achievements_unlock(state, ACH_EXPLORER);
  }
}

// Called when respect increases
static inline void achievements_check_respect(AchievementState *state, uint16_t total_respect) {
  // ACH_LEARNER: Earned 100 respect
  if (total_respect >= 100) {
    achievements_unlock(state, ACH_LEARNER);
  }
}

// Called when player makes a shop purchase
static inline void achievements_check_shop(AchievementState *state) {
  achievements_unlock(state, ACH_TOKEN_MASTER);
}

// Called when steps reach milestone (from step counter)
static inline void achievements_check_steps(AchievementState *state, uint32_t total_steps) {
  // ACH_WALKER: Walked 5,000 steps
  if (total_steps >= 5000) {
    achievements_unlock(state, ACH_WALKER);
  }
}
