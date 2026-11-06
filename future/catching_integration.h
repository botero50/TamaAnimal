/**
 * Tamanimals - Catching System Integration
 * Complete wrapper layer for token + respect + collection systems
 *
 * This file integrates token_system.h, respect_system.h, and encounter_system_updated.h
 * into a single cohesive catching system.
 */

#pragma once
#include "token_system.h"
#include "respect_system.h"
#include "encounter_system_updated.h"

// ============================================================================
// GLOBAL CATCHING STATE
// ============================================================================

typedef struct {
  TokenInventory tokens;        // Token inventory
  RespectSystem respect;        // Respect tracking
  EncounterState encounter;     // Passive encounter system
  HuntingSession hunting;       // Active hunting session
  Collection collection;        // Animals caught/seen
} CatchingSystem;

// ============================================================================
// INITIALIZATION
// ============================================================================

/**
 * Initialize entire catching system
 */
static inline void catching_system_init(CatchingSystem *sys) {
  tokens_init(&sys->tokens);
  respect_init(&sys->respect);
  encounter_init(&sys->encounter);
  collection_init(&sys->collection);

  // Initialize hunting as inactive
  sys->hunting.active = false;
  sys->hunting.animals_found = 0;
}

/**
 * Initialize for new game with starter animal
 */
static inline void catching_system_new_game(CatchingSystem *sys, uint8_t starter_animal_id) {
  catching_system_init(sys);

  // Mark starter as caught
  collection_catch(&sys->collection, starter_animal_id);

  // Start with some respect
  respect_add(&sys->respect, 50);

  // Start with basic tokens
  tokens_add(&sys->tokens, TOKEN_BONE, 5);   // Savanna
  tokens_add(&sys->tokens, TOKEN_WOOD, 3);   // Forest
  tokens_add(&sys->tokens, TOKEN_SEED, 3);   // Grassland
}

// ============================================================================
// MAIN GAME LOOP FUNCTIONS
// ============================================================================

/**
 * Check for passive wild encounters
 * Call this every game loop frame
 * Returns true if new encounter triggered
 */
static inline bool catching_check_encounter(CatchingSystem *sys, uint32_t current_time, uint8_t player_level) {
  return encounter_check(&sys->encounter, current_time, player_level);
}

/**
 * Get current active encounter (if any)
 */
static inline WildEncounter *catching_get_current_encounter(CatchingSystem *sys) {
  return encounter_get_current(&sys->encounter);
}

/**
 * Start active hunting in a habitat
 */
static inline void catching_start_hunting(CatchingSystem *sys, uint8_t habitat) {
  hunting_start(&sys->hunting, habitat);
}

/**
 * Check if hunting session has more animals to find
 */
static inline bool catching_hunting_has_more(CatchingSystem *sys) {
  return hunting_has_more_animals(&sys->hunting);
}

/**
 * Get next animal in current hunting session
 */
static inline uint8_t catching_hunting_get_animal(CatchingSystem *sys) {
  uint8_t animal_id = hunting_get_animal(&sys->hunting);
  sys->hunting.animals_found++;
  return animal_id;
}

/**
 * End hunting session
 */
static inline void catching_end_hunting(CatchingSystem *sys) {
  hunting_end(&sys->hunting);
}

// ============================================================================
// CATCH ATTEMPT HANDLING
// ============================================================================

/**
 * Attempt to catch the currently encountered animal
 * Returns: true if caught, false if escaped
 *
 * selected_token: Which token to use (0-11, or TOKEN_GOLDEN for 100%)
 * minigame_bonus: 0-50 points if player won minigame, 0 otherwise
 * player_level: Pet's current level
 */
static inline bool catching_attempt_catch(
  CatchingSystem *sys,
  uint8_t selected_token,
  uint8_t minigame_bonus,
  uint8_t player_level
) {
  WildEncounter *enc = catching_get_current_encounter(sys);
  if (!enc) return false;  // No active encounter

  // Use the token
  if (!tokens_use(&sys->tokens, selected_token)) {
    return false;  // Token not available
  }

  // Calculate catch rate
  uint8_t catch_rate = calculate_catch_rate(
    enc->rarity,
    enc->habitat,
    selected_token,
    player_level,
    sys->respect.respect_level,
    minigame_bonus
  );

  // Attempt catch
  if (attempt_catch(catch_rate)) {
    // SUCCESS!
    return true;  // Return true for UI to handle success animation
  } else {
    // FAILED - still give some respect for trying
    respect_add(&sys->respect, 5);
    // Keep encounter active for retry
    return false;
  }
}

/**
 * Handle successful catch
 * Call this AFTER catching_attempt_catch returns true
 */
static inline void catching_on_success(
  CatchingSystem *sys,
  uint8_t animal_id,
  uint16_t coin_reward
) {
  // Add to collection
  collection_catch(&sys->collection, animal_id);

  // Award coins and respect
  tokens_add_coins(&sys->tokens, coin_reward);
  respect_add(&sys->respect, 10);

  // Dismiss encounter
  encounter_dismiss(&sys->encounter);
}

/**
 * Handle failed catch
 * Call this AFTER catching_attempt_catch returns false
 */
static inline void catching_on_failure(CatchingSystem *sys) {
  // Encounter remains active for retry
  // Small respect gain already applied in catching_attempt_catch
  // Player can try again or run away
}

// ============================================================================
// MINIGAME REWARDS
// ============================================================================

/**
 * Award coins and tokens after minigame
 * score: Final minigame score
 * won: true if minigame was won
 */
static inline void catching_reward_minigame(
  CatchingSystem *sys,
  uint16_t score,
  bool won
) {
  if (won) {
    // Award coins based on score
    uint16_t coins = score / 10;  // Convert score to coins
    tokens_add_coins(&sys->tokens, coins);

    // Award tokens (one per 100 coins)
    uint8_t token_count = coins / 100;
    if (token_count > 0) {
      uint8_t token_type = random(11);  // Random habitat token (0-10)
      tokens_add(&sys->tokens, token_type, token_count);
    }

    // Build respect from win
    respect_earn_minigame_win(&sys->respect);
  } else {
    // Still get some coins for trying
    tokens_add_coins(&sys->tokens, score / 20);
  }
}

/**
 * Award respect and tokens for pet care
 * Call after feed, clean, or sleep actions
 */
static inline void catching_reward_care(CatchingSystem *sys) {
  respect_earn_proper_care(&sys->respect);

  // Small token reward for consistent care
  tokens_add(&sys->tokens, TOKEN_SEED, 1);  // Grassland/nature care token
}

/**
 * Award respect and tokens for tier-up
 * Call when player advances a tier
 */
static inline void catching_reward_tier_up(CatchingSystem *sys) {
  // Significant respect bonus for achievement
  respect_earn_tier_up(&sys->respect);

  // Token rewards for tier-up
  tokens_add(&sys->tokens, TOKEN_BONE, 50);   // 50 common tokens
  tokens_add(&sys->tokens, TOKEN_PEARL, 20);  // 20 uncommon tokens

  // Chance for special token
  if (random(100) < 10) {  // 10% chance
    tokens_add(&sys->tokens, TOKEN_GOLDEN, 1);
  }
}

// ============================================================================
// DAILY BONUSES
// ============================================================================

/**
 * Call once per day to award daily login bonuses
 */
static inline void catching_daily_bonus(CatchingSystem *sys) {
  // Respect bonus
  respect_earn_daily_bonus(&sys->respect);

  // Token gifts
  tokens_add(&sys->tokens, TOKEN_SEED, 3);    // 3 Grassland tokens
  tokens_add(&sys->tokens, TOKEN_WOOD, 2);    // 2 Forest tokens
  tokens_add(&sys->tokens, TOKEN_BONE, 2);    // 2 Savanna tokens
}

// ============================================================================
// COLLECTION & PROGRESS
// ============================================================================

/**
 * Get total animals caught
 */
static inline uint16_t catching_get_caught_count(CatchingSystem *sys) {
  return sys->collection.total_caught;
}

/**
 * Check if animal has been caught
 */
static inline bool catching_is_caught(CatchingSystem *sys, uint8_t animal_id) {
  return collection_is_caught(&sys->collection, animal_id);
}

/**
 * Get collection progress percentage
 */
static inline uint8_t catching_get_collection_percent(CatchingSystem *sys) {
  return (sys->collection.total_caught * 100) / 988;
}

/**
 * Get respect level (1-8)
 */
static inline uint8_t catching_get_respect_level(CatchingSystem *sys) {
  return sys->respect.respect_level;
}

/**
 * Get respect level name
 */
static inline const char *catching_get_respect_name(CatchingSystem *sys) {
  return respect_get_level_name(sys->respect.respect_level);
}

/**
 * Get progress to next respect level (0-100%)
 */
static inline uint8_t catching_get_respect_progress(CatchingSystem *sys) {
  return respect_get_progress_to_next(&sys->respect);
}

/**
 * Get total coins available
 */
static inline uint32_t catching_get_coins(CatchingSystem *sys) {
  return sys->tokens.coins;
}

/**
 * Get count of specific token type
 */
static inline uint16_t catching_get_token_count(CatchingSystem *sys, uint8_t token_type) {
  if (token_type < TOKEN_MAX) {
    return sys->tokens.tokens[token_type];
  }
  return 0;
}

/**
 * Spend coins (for token shop purchases)
 */
static inline void catching_spend_coins(CatchingSystem *sys, uint16_t amount) {
  if (sys->tokens.coins >= amount) {
    sys->tokens.coins -= amount;
  }
}

// ============================================================================
// SHOP / BUYING TOKENS
// ============================================================================

/**
 * Buy tokens with coins
 * Returns true if purchase successful
 */
static inline bool catching_buy_tokens(
  CatchingSystem *sys,
  uint8_t token_type,
  uint16_t quantity
) {
  return tokens_buy(&sys->tokens, token_type, quantity);
}

/**
 * Get cost of a single token
 */
static inline uint16_t catching_get_token_cost(uint8_t token_type) {
  return tokens_get_cost(token_type);
}

/**
 * Get token name
 */
static inline const char *catching_get_token_name(uint8_t token_type) {
  return tokens_get_name(token_type);
}

/**
 * Get token habitat ID
 */
static inline uint8_t catching_get_token_habitat(uint8_t token_type) {
  return tokens_get_habitat(token_type);
}

// ============================================================================
// UI HELPERS
// ============================================================================

/**
 * Get description of current respect level
 */
static inline const char *catching_get_respect_description(CatchingSystem *sys) {
  return respect_get_description(sys->respect.respect_level);
}

/**
 * Check if there are any tokens available
 */
static inline bool catching_has_tokens(CatchingSystem *sys) {
  return tokens_get_total(&sys->tokens) > 0;
}

/**
 * Check if player can afford a token
 */
static inline bool catching_can_afford_token(CatchingSystem *sys, uint8_t token_type) {
  uint16_t cost = tokens_get_cost(token_type);
  return sys->tokens.coins >= cost;
}
