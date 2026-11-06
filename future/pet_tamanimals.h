/**
 * Tamanimals Pet Structure
 * Simplified from animal virtual pet
 * Focus on care stats and bonding rather than complex battle mechanics
 */

#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

// Sprite animation system
#include "sprite_animation_states.h"
#include "sprite_state_transitions.h"

// ============================================================================
// PET CARE STATISTICS
// ============================================================================

typedef struct {
  uint8_t hunger;        // 0-255, 0 = full, 255 = starving
  uint8_t hygiene;       // 0-255, 0 = perfect, 255 = filthy
  uint8_t joy;           // 0-255, 255 = maximum happy
  uint8_t energy;        // 0-255, 255 = fully rested
  uint8_t bond;          // 0-255, bonding level with player
  uint8_t health;        // 0-255, 255 = perfect health
  uint8_t sickness;      // 0-20, number of days sick (0 = healthy)
} PetStats;

// ============================================================================
// MAIN PET STRUCTURE
// ============================================================================

typedef struct {
  // Identity
  uint8_t species_id;         // Animal species (1-988 from dex)
  char nickname[32];          // Custom name, empty = use species name
  bool shiny;                 // Special coloration variant

  // Basics
  uint8_t level;              // 1-100 progression
  uint32_t exp;               // Experience points
  uint16_t age_days;          // Days since birth
  uint32_t timestamp_birth;   // Birth timestamp (for age calculation)

  // Care
  PetStats stats;

  // Bonding & Love
  uint8_t bond_level;         // 0-10 bonding tiers
  uint8_t affection;          // 0-255, player affection
  int8_t mood;                // -10 to +10, current mood modifier

  // Caught Status
  bool is_caught;             // Is this a caught animal or starter
  uint8_t catch_rate;         // Base catch rate when found
  uint32_t timestamp_caught;  // When caught

  // Location Bonuses
  uint8_t current_habitat;    // Current biome (0-10)
  uint8_t habitat_bonus;      // Bonus from being in correct habitat (0-20%)

  // Power Type Specialization
  uint8_t favorite_power_type; // Which power type animal excels at
  uint8_t power_affinity;      // 0-100%, how much animal benefits from power type

  // Play History
  uint16_t minigames_played;   // Total minigames played
  uint16_t minigames_won;      // Minigames won
  uint32_t last_play_time;     // Last minigame timestamp

  // Breeding (optional)
  bool is_bred;                // Was this animal bred or caught
  uint8_t parent1_species;     // First parent species (for breeding)
  uint8_t parent2_species;     // Second parent species (for breeding)
  uint8_t generation;          // Breeding generation (0 = wild/starter)

  // Sprite Animation System (Phase 4)
  AnimalStateContext state_ctx;      // Animation state context
  uint32_t last_state_update_ms;     // Last animation state update

} Pet;

// ============================================================================
// PET LIFECYCLE FUNCTIONS
// ============================================================================

/**
 * Create new pet (starter or caught)
 */
static inline Pet pet_new(uint8_t species_id, const char *nickname,
                         bool is_caught) {
  Pet p = {};
  p.species_id = species_id;
  p.level = 1;
  p.exp = 0;
  p.age_days = 0;
  p.timestamp_birth = time(NULL);
  p.is_caught = is_caught;
  p.timestamp_caught = is_caught ? time(NULL) : 0;
  p.bond_level = 1;
  p.affection = 50;
  p.mood = 0;
  p.minigames_played = 0;
  p.minigames_won = 0;

  // Initialize stats to reasonable levels
  p.stats.hunger = 100;      // Slightly hungry
  p.stats.hygiene = 150;     // Could use cleaning
  p.stats.joy = 180;         // Generally happy
  p.stats.energy = 200;      // Somewhat rested
  p.stats.bond = 100;        // Neutral bond
  p.stats.health = 250;      // Very healthy
  p.stats.sickness = 0;      // Healthy

  // Initialize sprite animation state (Phase 4)
  memset(&p.state_ctx, 0, sizeof(AnimalStateContext));
  p.state_ctx.current_state = STATE_NORMAL;
  p.state_ctx.state_start_time_ms = millis();
  p.state_ctx.happiness_level = 100;
  p.state_ctx.hunger_level = 0;
  p.state_ctx.health_level = 100;
  p.state_ctx.is_sick = false;
  p.state_ctx.is_poisoned = false;
  p.last_state_update_ms = millis();

  if (nickname && strlen(nickname) > 0) {
    strncpy(p.nickname, nickname, sizeof(p.nickname) - 1);
  } else {
    p.nickname[0] = '\0';  // Use species name
  }

  return p;
}

/**
 * Get age in days
 */
static inline uint16_t pet_get_age_days(const Pet *pet) {
  if (!pet) return 0;
  time_t now = time(NULL);
  uint32_t seconds = now - pet->timestamp_birth;
  return seconds / (24 * 3600);  // Convert to days
}

/**
 * Play minigame: handle effects on pet
 */
static inline void pet_play_minigame(Pet *pet, uint16_t score, bool won) {
  if (!pet) return;

  pet->minigames_played++;
  if (won) {
    pet->minigames_won++;
    pet->stats.joy = pet->stats.joy > 200 ? 255 : pet->stats.joy + 30;
    pet->stats.energy = pet->stats.energy > 100 ? 0 : pet->stats.energy - 50;
    pet->affection = pet->affection > 240 ? 255 : pet->affection + 10;
    pet->exp += (score / 10);  // Exp from score
  } else {
    pet->stats.joy = pet->stats.joy > 50 ? pet->stats.joy - 30 : 0;
    pet->stats.energy = pet->stats.energy > 50 ? pet->stats.energy - 50 : 0;
  }

  pet->last_play_time = time(NULL);
}

/**
 * Feed the pet
 */
static inline void pet_feed(Pet *pet) {
  if (!pet) return;
  pet->stats.hunger = pet->stats.hunger > 50 ? pet->stats.hunger - 50 : 0;
  pet->stats.energy = pet->stats.energy > 100 ? 0 : pet->stats.energy - 100;
  pet->affection = pet->affection > 240 ? 255 : pet->affection + 5;
}

/**
 * Clean the pet
 */
static inline void pet_clean(Pet *pet) {
  if (!pet) return;
  pet->stats.hygiene = 0;  // Perfect cleanliness
  pet->stats.energy = pet->stats.energy > 50 ? pet->stats.energy - 50 : 0;
  pet->stats.health = pet->stats.health > 240 ? 255 : pet->stats.health + 10;
  pet->affection = pet->affection > 245 ? 255 : pet->affection + 5;
}

/**
 * Let pet sleep
 */
static inline void pet_sleep(Pet *pet) {
  if (!pet) return;
  pet->stats.energy = 255;  // Fully rested
  pet->stats.hunger = pet->stats.hunger > 100 ? pet->stats.hunger - 100 : 0;
  pet->stats.health = pet->stats.health > 240 ? 255 : pet->stats.health + 10;
}

/**
 * Pet naturally decays over time
 */
static inline void pet_tick(Pet *pet, uint32_t seconds_elapsed) {
  if (!pet) return;

  // Hunger increases over time
  uint8_t hunger_increase = (seconds_elapsed / 60);  // 1 point per minute
  pet->stats.hunger = (pet->stats.hunger + hunger_increase) > 255 ?
                      255 : pet->stats.hunger + hunger_increase;

  // Energy decreases over time
  uint8_t energy_decrease = (seconds_elapsed / 120);  // 1 point per 2 minutes
  pet->stats.energy = pet->stats.energy > energy_decrease ?
                      pet->stats.energy - energy_decrease : 0;

  // Hygiene worsens over time
  uint8_t hygiene_increase = (seconds_elapsed / 300);  // 1 point per 5 minutes
  pet->stats.hygiene = (pet->stats.hygiene + hygiene_increase) > 255 ?
                       255 : pet->stats.hygiene + hygiene_increase;

  // Health damage from prolonged hunger (Phase 4 - Critical)
  if (pet->stats.hunger > 200) {
    // Very hungry: health decreases rapidly
    if (pet->stats.health > 0) {
      pet->stats.health--;
    }
    // Sickness increases when starving
    if (pet->stats.sickness < 20) {
      pet->stats.sickness++;
    }
  } else if (pet->stats.hunger > 150) {
    // Moderately hungry: slower health loss
    if (seconds_elapsed % 30 == 0 && pet->stats.health > 0) {  // Every 30 seconds
      pet->stats.health--;
    }
  }

  // Sickness damage if health is low
  if (pet->stats.health < 50 && pet->stats.sickness < 20) {
    pet->stats.sickness++;
  }

  // Mood affects bonding
  if (pet->stats.joy > 200) {
    pet->bond = pet->bond > 240 ? 255 : pet->bond + 2;
  } else if (pet->stats.joy < 50) {
    pet->bond = pet->bond > 10 ? pet->bond - 5 : 0;
  }

  // If very unhappy/unhealthy, affection decreases
  if (pet->stats.joy < 50 || pet->stats.health < 50) {
    pet->affection = pet->affection > 5 ? pet->affection - 5 : 0;
  }
}

/**
 * Get visual representation of stat
 */
static inline const char* pet_stat_visual(uint8_t value) {
  if (value > 200) return "▓▓▓▓▓▓▓▓";
  if (value > 150) return "▓▓▓▓▓▓░░";
  if (value > 100) return "▓▓▓▓░░░░";
  if (value > 50) return "▓▓░░░░░░";
  return "░░░░░░░░";
}

/**
 * Get win rate percentage
 */
static inline uint8_t pet_get_winrate(const Pet *pet) {
  if (!pet || pet->minigames_played == 0) return 0;
  return (uint8_t)((pet->minigames_won * 100) / pet->minigames_played);
}

/**
 * Get bond status text
 */
static inline const char* pet_get_bond_text(uint8_t bond_level) {
  switch (bond_level) {
    case 0: return "Stranger";
    case 1:
    case 2: return "Acquaintance";
    case 3:
    case 4: return "Friend";
    case 5:
    case 6: return "Close Friend";
    case 7:
    case 8: return "Best Friend";
    case 9:
    case 10: return "Soulmate";
    default: return "Unknown";
  }
}

