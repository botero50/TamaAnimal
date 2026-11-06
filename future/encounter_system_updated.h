/**
 * Tamanimals - Encounter System (Updated for Token + Respect)
 * Wild animal encounters with token-based catching
 */

#pragma once
#include "dex.h"
#include "token_system.h"
#include "respect_system.h"
#include <stdint.h>

// ============================================================================
// ENCOUNTER STATE
// ============================================================================

typedef struct {
  uint8_t animal_id;           // 1-988 (from DEX_TBL)
  uint8_t rarity;              // Animal's rarity tier
  uint8_t habitat;             // Animal's habitat type
  uint16_t encounter_time;     // When encountered (for timeout)
  bool active;                 // Is encounter currently active
} WildEncounter;

typedef struct {
  uint32_t last_check_time;    // Last time we checked for encounter
  uint32_t encounter_interval; // How often to check (900 = 15 min)
  WildEncounter current;       // Current wild animal (if any)
} EncounterState;

// ============================================================================
// ENCOUNTER FUNCTIONS
// ============================================================================

/**
 * Initialize encounter system
 */
static inline void encounter_init(EncounterState *state) {
  state->last_check_time = 0;
  state->encounter_interval = 900;  // 15 minutes
  state->current.active = false;
}

/**
 * Check if it's time for a new encounter (call frequently, e.g., every frame)
 * Returns true if new encounter spawned
 *
 * player_level: Higher levels unlock rarer animals
 * current_time: In seconds (use millis()/1000)
 */
static inline bool encounter_check(EncounterState *state, uint32_t current_time, uint8_t player_level) {
  if (!state->current.active) {
    // Check if interval has passed
    if (current_time - state->last_check_time >= state->encounter_interval) {
      state->last_check_time = current_time;

      // 50% chance of encounter (adjust as needed for balance)
      if (random(100) < 50) {
        // Spawn new encounter
        state->current.animal_id = select_wild_animal(player_level);
        state->current.rarity = DEX_TBL[state->current.animal_id].rarity;
        state->current.habitat = DEX_TBL[state->current.animal_id].habitat;
        state->current.active = true;
        state->current.encounter_time = current_time;
        return true;  // New encounter!
      }
    }
  }
  return false;
}

/**
 * Select a wild animal based on player level
 * Higher level = access to rarer animals
 *
 * Level progression:
 * 1-10:    Common animals
 * 11-30:   Uncommon animals
 * 31-70:   Rare animals
 * 71-100:  Legendary animals
 * 100+:    Mythical & Extinct animals
 */
static inline uint8_t select_wild_animal(uint8_t player_level) {
  uint8_t max_rarity = RARITY_COMMON;

  if (player_level >= 100) max_rarity = RARITY_MYTHICAL;
  else if (player_level >= 70) max_rarity = RARITY_LEGENDARY;
  else if (player_level >= 30) max_rarity = RARITY_RARE;
  else if (player_level >= 10) max_rarity = RARITY_UNCOMMON;

  // Random animal from accessible rarity
  uint16_t attempts = 0;
  uint8_t animal_id;

  while (attempts < 100) {
    animal_id = 1 + random(DEX_COUNT);
    if (DEX_TBL[animal_id].rarity <= max_rarity) {
      return animal_id;
    }
    attempts++;
  }

  return 1;  // Fallback
}

/**
 * Dismiss current encounter (player ran away)
 */
static inline void encounter_dismiss(EncounterState *state) {
  state->current.active = false;
}

/**
 * Get current encounter (if any)
 */
static inline WildEncounter *encounter_get_current(EncounterState *state) {
  if (state->current.active) {
    return &state->current;
  }
  return NULL;
}

// ============================================================================
// CATCH ATTEMPT CALCULATION
// ============================================================================

/**
 * Calculate catch success probability
 *
 * Formula:
 * Base = rarity-based catch rate (60-90%)
 * + Token bonus (0% if wrong habitat, +50% if correct habitat)
 * + Respect bonus (respects_level * 10, max +80)
 * + Player level bonus (0-50%)
 * + Minigame bonus (0-50 if won)
 * = Final catch rate (0-100%)
 *
 * Special: Golden Token = guaranteed 100%
 */
static inline uint8_t calculate_catch_rate(
  uint8_t animal_rarity,         // From DEX_TBL
  uint8_t animal_habitat,        // From DEX_TBL
  uint8_t selected_token,        // Selected token type
  uint8_t player_level,          // Pet level
  uint8_t respect_level,         // Player respect (1-8)
  uint8_t minigame_bonus         // 0-50 if player won minigame
) {
  // Golden Token = automatic success
  if (selected_token == TOKEN_GOLDEN) {
    return 255;  // 100%
  }

  uint16_t catch_rate = 0;

  // Base rate by rarity
  switch (animal_rarity) {
    case RARITY_COMMON: catch_rate = 90; break;
    case RARITY_UNCOMMON: catch_rate = 70; break;
    case RARITY_RARE: catch_rate = 50; break;
    case RARITY_LEGENDARY: catch_rate = 30; break;
    case RARITY_MYTHICAL: catch_rate = 15; break;
    case RARITY_EXTINCT: catch_rate = 5; break;
    default: catch_rate = 50;
  }

  // Token habitat bonus (+50% if correct habitat token used)
  if (selected_token < 11) {  // 0-10 are habitat tokens
    if (selected_token == animal_habitat) {
      catch_rate = (catch_rate * 150) / 100;  // +50%
    }
  }

  // Respect bonus (+10 per level, max +80)
  if (respect_level > 8) respect_level = 8;
  catch_rate += (respect_level * 10);

  // Player level bonus (higher level = better catcher)
  uint8_t level_bonus = (player_level > 50) ? 50 : player_level;
  catch_rate += level_bonus;

  // Minigame bonus (0-50 if won)
  catch_rate += minigame_bonus;

  // Cap at 255 (100%)
  return (catch_rate > 255) ? 255 : (uint8_t)catch_rate;
}

/**
 * Attempt to catch the wild animal
 *
 * Returns: true if catch successful, false if animal escaped
 *
 * The actual catch is determined by:
 * 1. Calculate catch rate (see above)
 * 2. Random check: random(256) < catch_rate
 * 3. If true: catch successful
 */
static inline bool attempt_catch(uint8_t catch_rate) {
  return random(256) < catch_rate;
}

// ============================================================================
// HABITAT HUNTING (Active mode)
// ============================================================================

typedef struct {
  bool active;                // Is player in hunting mode
  uint8_t current_habitat;    // Which habitat selected
  uint8_t animals_found;      // How many found in this session (0-5)
  uint32_t session_start;     // When session started
} HuntingSession;

/**
 * Start hunting in a habitat
 * Guaranteed 3-5 encounters in the habitat
 */
static inline void hunting_start(HuntingSession *session, uint8_t habitat) {
  session->active = true;
  session->current_habitat = habitat;
  session->animals_found = 0;
  session->session_start = millis();  // Or appropriate time function
}

/**
 * Get next animal in habitat for active hunting
 */
static inline uint8_t hunting_get_animal(HuntingSession *session) {
  // Select random animal from this habitat
  uint8_t animals[100];
  uint16_t count = 0;

  // Find all animals in this habitat
  for (uint16_t i = 1; i <= DEX_COUNT && count < 100; i++) {
    if (DEX_TBL[i].habitat == session->current_habitat) {
      animals[count++] = i;
    }
  }

  if (count == 0) return 1;  // Fallback
  return animals[random(count)];
}

/**
 * End hunting session
 */
static inline void hunting_end(HuntingSession *session) {
  session->active = false;
}

/**
 * Check if hunting should offer more encounters
 * Returns true if more animals available (up to 5 per session)
 */
static inline bool hunting_has_more_animals(HuntingSession *session) {
  return session->animals_found < 5;  // Max 5 per hunting session
}
