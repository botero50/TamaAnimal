#pragma once

// ============================================================================
// COLLECTION TIER SYSTEM - Phase 3 Community Improvement
// ============================================================================
// Unlock progression benefits at collection milestones
// Progression path separate from respect level (multiple ways to succeed)
// Creates multiple goal pathways for different player types

#include <stdint.h>
#include <string.h>

// Tier definitions
enum CollectionTier {
  TIER_EXPLORER = 0,      // 0-100 animals
  TIER_NATURALIST = 1,    // 100-300 animals
  TIER_BIOLOGIST = 2,     // 300-600 animals
  TIER_KEEPER = 3,        // 600-988 animals (full collection)
};

struct CollectionTierInfo {
  uint8_t tier;
  const char *name;
  const char *description;
  uint16_t min_animals;
  uint16_t max_animals;
  uint8_t unlock_reward_type;  // 0=coins, 1=tokens, 2=cosmetic, 3=catch_bonus
  uint16_t reward_amount;
};

struct CollectionTierState {
  uint8_t current_tier;
  uint16_t animals_caught;
  uint8_t tiers_unlocked;        // Bitmask of unlocked tiers
  bool tier_bonus_active[4];     // Bonus status for each tier
  uint32_t unlocked_at[4];       // Timestamp when unlocked
};

// Tier information table
static const CollectionTierInfo TIER_INFO[] = {
  {
    TIER_EXPLORER,
    "Explorer",
    "Caught 100 animals - Unlock daily coins bonus",
    0, 100,
    0,  // Coins reward
    100, // 100 coins/day
  },
  {
    TIER_NATURALIST,
    "Naturalist",
    "Caught 300 animals - Unlock habitat documentaries",
    100, 300,
    2,  // Cosmetic
    1,  // Unlock cosmetic pet skin
  },
  {
    TIER_BIOLOGIST,
    "Biologist",
    "Caught 600 animals - Unlock catch rate bonus",
    300, 600,
    3,  // Catch bonus
    10, // +10% catch rate
  },
  {
    TIER_KEEPER,
    "Keeper",
    "Caught all 988 animals - Unlock exclusive rewards",
    600, 988,
    1,  // Tokens
    5,  // 5 special tokens
  },
};

// ============================================================================
// Initialize collection tier system
// ============================================================================
static inline void collection_tiers_init(CollectionTierState *state) {
  state->current_tier = TIER_EXPLORER;
  state->animals_caught = 0;
  state->tiers_unlocked = 0;

  for (int i = 0; i < 4; i++) {
    state->tier_bonus_active[i] = false;
    state->unlocked_at[i] = 0;
  }
}

// ============================================================================
// Get tier from animal count
// ============================================================================
static inline uint8_t collection_tiers_get_tier(uint16_t animals_caught) {
  if (animals_caught >= 600) return TIER_KEEPER;
  if (animals_caught >= 300) return TIER_BIOLOGIST;
  if (animals_caught >= 100) return TIER_NATURALIST;
  return TIER_EXPLORER;
}

// ============================================================================
// Update state based on collection count
// Returns: tier level that was newly unlocked (or 0xFF if none)
// ============================================================================
static inline uint8_t collection_tiers_update(CollectionTierState *state, uint16_t animals_caught) {
  uint8_t new_tier = collection_tiers_get_tier(animals_caught);
  uint8_t old_tier = state->current_tier;

  state->animals_caught = animals_caught;
  state->current_tier = new_tier;

  // Check for newly unlocked tiers
  uint8_t newly_unlocked = 0xFF;

  if (new_tier > old_tier) {
    // Tier advancement
    for (int i = old_tier + 1; i <= new_tier; i++) {
      if (!(state->tiers_unlocked & (1 << i))) {
        state->tiers_unlocked |= (1 << i);
        state->tier_bonus_active[i] = true;
        state->unlocked_at[i] = time(NULL);
        if (newly_unlocked == 0xFF) newly_unlocked = i;
      }
    }
  }

  return newly_unlocked;
}

// ============================================================================
// Check if tier is unlocked
// ============================================================================
static inline bool collection_tiers_is_unlocked(const CollectionTierState *state, uint8_t tier) {
  if (tier >= 4) return false;
  return (state->tiers_unlocked & (1 << tier)) != 0;
}

// ============================================================================
// Get tier name
// ============================================================================
static inline const char *collection_tiers_get_name(uint8_t tier) {
  if (tier >= 4) return "Unknown";
  return TIER_INFO[tier].name;
}

// ============================================================================
// Get tier description
// ============================================================================
static inline const char *collection_tiers_get_description(uint8_t tier) {
  if (tier >= 4) return "";
  return TIER_INFO[tier].description;
}

// ============================================================================
// Get tier requirement (minimum animals)
// ============================================================================
static inline uint16_t collection_tiers_get_requirement(uint8_t tier) {
  if (tier >= 4) return 988;
  return TIER_INFO[tier].min_animals;
}

// ============================================================================
// Get progress to next tier (0-100%)
// ============================================================================
static inline uint8_t collection_tiers_get_next_tier_progress(const CollectionTierState *state) {
  uint8_t next_tier = state->current_tier + 1;
  if (next_tier >= 4) return 100;  // Already at max

  uint16_t current_req = TIER_INFO[state->current_tier].min_animals;
  uint16_t next_req = TIER_INFO[next_tier].min_animals;
  uint16_t range = next_req - current_req;

  if (range == 0) return 100;

  uint16_t progress = state->animals_caught - current_req;
  uint8_t percent = (progress * 100) / range;

  if (percent > 100) percent = 100;
  return percent;
}

// ============================================================================
// Get animals needed for next tier
// ============================================================================
static inline uint16_t collection_tiers_get_animals_needed(const CollectionTierState *state) {
  uint8_t next_tier = state->current_tier + 1;
  if (next_tier >= 4) return 0;  // Already at max

  uint16_t next_req = TIER_INFO[next_tier].min_animals;
  if (state->animals_caught >= next_req) return 0;

  return next_req - state->animals_caught;
}

// ============================================================================
// Get catch rate bonus from tiers
// ============================================================================
static inline uint8_t collection_tiers_get_catch_bonus(const CollectionTierState *state) {
  // Biologist tier and above give +10% catch rate
  if (state->tier_bonus_active[TIER_BIOLOGIST]) {
    return 10;
  }
  return 0;
}

// ============================================================================
// Get daily coin bonus from tiers
// ============================================================================
static inline uint16_t collection_tiers_get_daily_coin_bonus(const CollectionTierState *state) {
  // Explorer tier and above give +100 coins/day
  if (state->tier_bonus_active[TIER_EXPLORER]) {
    return 100;
  }
  return 0;
}

// ============================================================================
// Get cosmetic unlock from tiers
// ============================================================================
static inline bool collection_tiers_has_cosmetic_unlock(const CollectionTierState *state) {
  // Naturalist tier unlocks pet skins
  return state->tier_bonus_active[TIER_NATURALIST];
}

// ============================================================================
// Get special token reward from tiers
// ============================================================================
static inline uint8_t collection_tiers_get_special_tokens(const CollectionTierState *state) {
  // Keeper tier gives special tokens
  if (state->tier_bonus_active[TIER_KEEPER]) {
    return 5;
  }
  return 0;
}

// ============================================================================
// Get all active bonuses as text
// ============================================================================
static inline void collection_tiers_get_active_bonuses_text(const CollectionTierState *state,
                                                             char *buffer) {
  buffer[0] = '\0';

  // Explorer bonuses
  if (state->tier_bonus_active[TIER_EXPLORER]) {
    strcat(buffer, "+100 coins/day | ");
  }

  // Naturalist bonuses
  if (state->tier_bonus_active[TIER_NATURALIST]) {
    strcat(buffer, "Pet skins unlocked | ");
  }

  // Biologist bonuses
  if (state->tier_bonus_active[TIER_BIOLOGIST]) {
    strcat(buffer, "+10% catch rate | ");
  }

  // Keeper bonuses
  if (state->tier_bonus_active[TIER_KEEPER]) {
    strcat(buffer, "Legendary rewards unlocked");
  }

  // Remove trailing " | " if present
  if (strlen(buffer) > 3 && buffer[strlen(buffer) - 3] == '|') {
    buffer[strlen(buffer) - 3] = '\0';
  }
}

// ============================================================================
// Get next tier milestone info
// ============================================================================
static inline void collection_tiers_get_next_milestone(const CollectionTierState *state,
                                                        char *name, char *reward) {
  uint8_t next_tier = state->current_tier + 1;
  if (next_tier >= 4) {
    strcpy(name, "Collection Complete!");
    strcpy(reward, "You've caught them all!");
    return;
  }

  strcpy(name, collection_tiers_get_name(next_tier));
  strcpy(reward, collection_tiers_get_description(next_tier));
}

// ============================================================================
// Serialize collection tier state
// ============================================================================
static inline void collection_tiers_serialize(const CollectionTierState *state, uint8_t *buffer) {
  buffer[0] = state->current_tier;
  buffer[1] = state->tiers_unlocked;
  uint16_t *p = (uint16_t *)(buffer + 2);
  p[0] = state->animals_caught;
}

// ============================================================================
// Deserialize collection tier state
// ============================================================================
static inline void collection_tiers_deserialize(CollectionTierState *state, const uint8_t *buffer) {
  state->current_tier = buffer[0];
  state->tiers_unlocked = buffer[1];
  const uint16_t *p = (const uint16_t *)(buffer + 2);
  state->animals_caught = p[0];
}

// ============================================================================
// Get time when tier was unlocked
// ============================================================================
static inline uint32_t collection_tiers_get_unlock_time(const CollectionTierState *state,
                                                         uint8_t tier) {
  if (tier >= 4) return 0;
  return state->unlocked_at[tier];
}

// ============================================================================
// Check if collection is complete
// ============================================================================
static inline bool collection_tiers_is_complete(const CollectionTierState *state) {
  return state->animals_caught >= 988;
}

// ============================================================================
// Get completion percentage
// ============================================================================
static inline uint8_t collection_tiers_get_completion_percent(const CollectionTierState *state) {
  uint8_t percent = (state->animals_caught * 100) / 988;
  if (percent > 100) percent = 100;
  return percent;
}
