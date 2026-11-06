/**
 * Tamanimals Game Systems
 * Power type minigames, rarity/catching system, extinction mechanics
 */

#pragma once
#include "dex.h"
#include <stdint.h>
#include <stdbool.h>

// ============================================================================
// POWER TYPE MINIGAME SYSTEM
// ============================================================================

typedef enum {
  MINIGAME_STRENGTH_PUSH,      // Push game - PT_STRENGTH
  MINIGAME_SPEED_RACE,          // Racing game - PT_SPEED
  MINIGAME_AGILITY_CLIMB,       // Climbing game - PT_AGILITY
  MINIGAME_INTELLIGENCE_PUZZLE, // Puzzle game - PT_INTELLIGENCE
  MINIGAME_FLIGHT_CATCH,        // Aerial catch - PT_FLIGHT
  MINIGAME_SWIM_DIVE,           // Swimming/diving - PT_SWIM
  MINIGAME_CLIMB_JUMP,          // Jumping/climbing - PT_CLIMB
  MINIGAME_DIG_TUNNEL,          // Digging game - PT_DIG
  MINIGAME_HIDE_SEEK,           // Hide & seek - PT_HIDE
  MINIGAME_VENOM_DODGE,         // Dodge game - PT_VENOM
  MINIGAME_ARMOR_DEFENSE,       // Defense game - PT_ARMOR
  MINIGAME_SIZE_PUSH,           // Weight-based push - PT_SIZE
} MinigameType;

typedef struct {
  uint8_t animal_id;
  uint8_t minigame_type;        // Which minigame to play
  uint16_t score;               // Final score
  uint8_t difficulty;           // 1-5 difficulty level
  bool success;                 // Did player win
  uint8_t reward_amount;        // Exp/bonding reward
} MinigameResult;

/**
 * Get recommended minigame for an animal based on power types
 */
static inline uint8_t get_primary_minigame(uint8_t animal_id) {
  if (animal_id > DEX_COUNT) return MINIGAME_STRENGTH_PUSH;

  const DexEntry &entry = DEX_TBL[animal_id];
  uint8_t power1 = entry.power1;

  // Map power type to minigame
  switch (power1) {
    case PT_STRENGTH: return MINIGAME_STRENGTH_PUSH;
    case PT_SPEED: return MINIGAME_SPEED_RACE;
    case PT_AGILITY: return MINIGAME_AGILITY_CLIMB;
    case PT_INTELLIGENCE: return MINIGAME_INTELLIGENCE_PUZZLE;
    case PT_FLIGHT: return MINIGAME_FLIGHT_CATCH;
    case PT_SWIM: return MINIGAME_SWIM_DIVE;
    case PT_CLIMB: return MINIGAME_CLIMB_JUMP;
    case PT_DIG: return MINIGAME_DIG_TUNNEL;
    case PT_HIDE: return MINIGAME_HIDE_SEEK;
    case PT_VENOM: return MINIGAME_VENOM_DODGE;
    case PT_ARMOR: return MINIGAME_ARMOR_DEFENSE;
    case PT_SIZE: return MINIGAME_SIZE_PUSH;
    default: return MINIGAME_STRENGTH_PUSH;
  }
}

/**
 * Calculate minigame difficulty based on rarity
 */
static inline uint8_t get_minigame_difficulty(uint8_t rarity) {
  switch (rarity) {
    case R_COMMON: return 1;
    case R_UNCOMMON: return 2;
    case R_RARE: return 3;
    case R_LEGENDARY: return 4;
    case R_MYTHICAL: return 4;
    case R_EXTINCT: return 5;
    default: return 1;
  }
}

/**
 * Calculate bonding/exp reward from minigame score
 */
static inline uint8_t calculate_minigame_reward(uint16_t score, uint8_t difficulty) {
  // Base reward: 10 + difficulty*5
  uint8_t base = 10 + (difficulty * 5);
  // Bonus for high scores
  uint8_t bonus = (score / 100);  // 1 bonus per 100 score points
  return (uint8_t)((base + bonus) > 255 ? 255 : (base + bonus));
}

// ============================================================================
// CATCHING / RARITY SYSTEM
// ============================================================================

typedef struct {
  uint8_t animal_id;
  uint8_t rarity;
  uint8_t base_catch_rate;  // 0-255 probability
  uint8_t level_min;        // Min level to encounter
  uint8_t level_max;        // Max level encountered
  uint16_t exp_yield;       // Exp given when caught
  bool is_starter;          // Starter animal
} CatchSpecification;

/**
 * Get catch rate for a rarity level
 */
static inline uint8_t get_catch_rate(uint8_t rarity) {
  switch (rarity) {
    case R_COMMON: return 255;     // 100% catch rate
    case R_UNCOMMON: return 200;   // ~78%
    case R_RARE: return 100;       // ~39%
    case R_LEGENDARY: return 50;   // ~20%
    case R_MYTHICAL: return 30;    // ~12%
    case R_EXTINCT: return 10;     // ~4%
    default: return 255;
  }
}

/**
 * Calculate catch success probability
 * Can be modified by current animal's power types and minigame performance
 */
static inline bool attempt_catch(uint8_t animal_id, uint8_t catch_ball_quality,
                                uint8_t minigame_bonus) {
  if (animal_id > DEX_COUNT) return false;

  const DexEntry &entry = DEX_TBL[animal_id];
  uint8_t base_rate = get_catch_rate(entry.rarity);

  // Increase catch rate with better ball
  base_rate = (uint8_t)((base_rate + (catch_ball_quality * 10)) > 255 ? 255 :
              (base_rate + (catch_ball_quality * 10)));

  // Add minigame bonus (up to 50 points)
  base_rate = (uint8_t)((base_rate + minigame_bonus) > 255 ? 255 :
              (base_rate + minigame_bonus));

  // Return success if random roll < adjusted rate
  return (random(256) < base_rate);
}

// ============================================================================
// EXTINCTION / GEOLOGICAL PERIOD SYSTEM
// ============================================================================

typedef enum {
  ERA_MODERN = 0,
  ERA_QUATERNARY = 1,        // 2.6M - 11,700 years ago
  ERA_PLIOCENE = 2,          // 5.3M - 2.6M years ago
  ERA_MIOCENE = 3,           // 23M - 5.3M years ago
  ERA_OLIGOCENE = 4,         // 34M - 23M years ago
  ERA_EOCENE = 5,            // 56M - 34M years ago
  ERA_PALEOCENE = 6,         // 66M - 56M years ago
} GeologicalEra;

typedef struct {
  uint8_t animal_id;
  uint8_t era;               // GeologicalEra
  const char *extinct_name;  // Species name (e.g. "Mammuthus primigenius")
  uint16_t years_ago_min;    // Million years ago minimum
  uint16_t years_ago_max;    // Million years ago maximum
} ExtinctionInfo;

/**
 * Get geological era name
 */
static inline const char* get_era_name(uint8_t era) {
  switch (era) {
    case ERA_MODERN: return "Modern";
    case ERA_QUATERNARY: return "Quaternary (2.6M - 11.7K years)";
    case ERA_PLIOCENE: return "Pliocene (5.3M - 2.6M years)";
    case ERA_MIOCENE: return "Miocene (23M - 5.3M years)";
    case ERA_OLIGOCENE: return "Oligocene (34M - 23M years)";
    case ERA_EOCENE: return "Eocene (56M - 34M years)";
    case ERA_PALEOCENE: return "Paleocene (66M - 56M years)";
    default: return "Unknown";
  }
}

/**
 * Check if animal is extinct
 */
static inline bool is_extinct_animal(uint8_t animal_id) {
  if (animal_id > DEX_COUNT) return false;
  return (DEX_TBL[animal_id].rarity == R_EXTINCT);
}

/**
 * Get extinction difficulty multiplier
 * Extinct animals are harder to find but more valuable
 */
static inline float get_extinction_multiplier(uint8_t animal_id) {
  if (is_extinct_animal(animal_id)) {
    return 2.5f;  // 2.5x more valuable, rarer
  }
  return 1.0f;
}

// ============================================================================
// PROGRESSION SYSTEM
// ============================================================================

typedef enum {
  TIER_NOVICE = 1,          // First 10 animals
  TIER_APPRENTICE = 2,      // 11-30 animals
  TIER_AMATEUR = 3,         // 31-100 animals
  TIER_INTERMEDIATE = 4,    // 101-200 animals
  TIER_ADVANCED = 5,        // 201-400 animals
  TIER_EXPERT = 6,          // 401-600 animals
  TIER_MASTER = 7,          // 601-800 animals
  TIER_LEGEND = 8,          // 801-950 animals
  TIER_CHAMPION = 9,        // All 988 animals
} ProgressionTier;

/**
 * Calculate progression tier based on animals caught
 */
static inline uint8_t get_progression_tier(uint16_t animals_caught) {
  if (animals_caught < 10) return TIER_NOVICE;
  if (animals_caught < 30) return TIER_APPRENTICE;
  if (animals_caught < 100) return TIER_AMATEUR;
  if (animals_caught < 200) return TIER_INTERMEDIATE;
  if (animals_caught < 400) return TIER_ADVANCED;
  if (animals_caught < 600) return TIER_EXPERT;
  if (animals_caught < 800) return TIER_MASTER;
  if (animals_caught < 950) return TIER_LEGEND;
  return TIER_CHAMPION;
}

/**
 * Get tier name
 */
static inline const char* get_tier_name(uint8_t tier) {
  switch (tier) {
    case TIER_NOVICE: return "Novice";
    case TIER_APPRENTICE: return "Apprentice";
    case TIER_AMATEUR: return "Amateur";
    case TIER_INTERMEDIATE: return "Intermediate";
    case TIER_ADVANCED: return "Advanced";
    case TIER_EXPERT: return "Expert";
    case TIER_MASTER: return "Master";
    case TIER_LEGEND: return "Legend";
    case TIER_CHAMPION: return "Champion";
    default: return "Unknown";
  }
}

/**
 * Get tier progression percentage
 */
static inline uint8_t get_tier_progress(uint16_t animals_caught) {
  uint8_t tier = get_progression_tier(animals_caught);
  const uint16_t tier_thresholds[] = {10, 30, 100, 200, 400, 600, 800, 950, 988};
  const uint16_t tier_sizes[] = {10, 20, 70, 100, 200, 200, 200, 150, 38};

  if (tier > TIER_CHAMPION) tier = TIER_CHAMPION;

  uint16_t tier_start = (tier > 1) ? tier_thresholds[tier - 2] : 0;
  uint16_t tier_size = tier_sizes[tier - 1];
  uint16_t progress = animals_caught - tier_start;

  return (uint8_t)((progress * 100) / tier_size);
}

