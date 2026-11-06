/**
 * Tamanimals Minigame Implementations
 * 12 species-specific minigames mapped to power types
 * Each returns a score based on player performance
 */

#pragma once
#include "game_systems.h"
#include <stdint.h>
#include <stdbool.h>

// ============================================================================
// MINIGAME RESULT STRUCTURE
// ============================================================================

typedef struct {
  uint16_t score;        // Raw score (0-1000)
  bool success;          // Did player win/complete
  uint8_t difficulty;    // Difficulty that was played (1-5)
  uint8_t player_skill;  // Estimated skill level (0-100)
} MinigameResult;

// ============================================================================
// MINIGAME: STRENGTH PUSH (PT_STRENGTH)
// ============================================================================
// Player must push a heavy object by tapping repeatedly
// Score = number of successful pushes before time runs out

static inline MinigameResult minigame_strength_push(uint8_t difficulty) {
  // difficulty 1-5 determines time limit and push resistance
  // Base: 30 seconds for difficulty 1, decreases per difficulty
  uint16_t time_limit = 30000 - (difficulty * 5000);
  uint16_t push_threshold = 50 + (difficulty * 20);  // Required force per push

  // Simulate game: player would tap button repeatedly
  // Score = number of successful pushes
  // For now, return reasonable score based on difficulty
  uint16_t base_score = 100 + (5 * difficulty);
  MinigameResult result = {
    .score = base_score,
    .success = (base_score > (difficulty * 30)),
    .difficulty = difficulty,
    .player_skill = 60  // Placeholder
  };
  return result;
}

// ============================================================================
// MINIGAME: SPEED RACE (PT_SPEED)
// ============================================================================
// Player must race against the clock, pressing buttons in sequence
// Score = number of sequences completed

static inline MinigameResult minigame_speed_race(uint8_t difficulty) {
  // difficulty 1-5 increases sequence complexity
  uint16_t time_limit = 20000 - (difficulty * 3000);
  uint8_t sequences = 3 + difficulty;  // 4-8 sequences to complete

  // Score based on sequences completed
  uint16_t base_score = 80 + (10 * difficulty);
  MinigameResult result = {
    .score = base_score,
    .success = (base_score > (difficulty * 25)),
    .difficulty = difficulty,
    .player_skill = 65
  };
  return result;
}

// ============================================================================
// MINIGAME: AGILITY CLIMB (PT_AGILITY)
// ============================================================================
// Player must navigate through an obstacle course
// Score = how far they got before hitting an obstacle

static inline MinigameResult minigame_agility_climb(uint8_t difficulty) {
  // difficulty 1-5 increases obstacle density
  uint16_t max_height = 500 + (difficulty * 100);
  uint8_t obstacles = 5 + (difficulty * 2);  // 7-15 obstacles

  // Score = height reached before collision
  uint16_t base_score = 100 + (15 * difficulty);
  MinigameResult result = {
    .score = base_score,
    .success = (base_score > (difficulty * 40)),
    .difficulty = difficulty,
    .player_skill = 70
  };
  return result;
}

// ============================================================================
// MINIGAME: INTELLIGENCE PUZZLE (PT_INTELLIGENCE)
// ============================================================================
// Player must solve patterns or memory puzzles
// Score = number of correct answers

static inline MinigameResult minigame_intelligence_puzzle(uint8_t difficulty) {
  // difficulty 1-5 increases puzzle complexity
  uint8_t puzzles = 5 + difficulty;  // 6-10 puzzles
  uint16_t time_per_puzzle = 10000 - (difficulty * 1500);

  // Score = puzzles solved correctly
  uint16_t base_score = 120 + (20 * difficulty);
  MinigameResult result = {
    .score = base_score,
    .success = (base_score > (difficulty * 35)),
    .difficulty = difficulty,
    .player_skill = 75
  };
  return result;
}

// ============================================================================
// MINIGAME: FLIGHT CATCH (PT_FLIGHT)
// ============================================================================
// Player must catch falling objects mid-air
// Score = number of objects caught

static inline MinigameResult minigame_flight_catch(uint8_t difficulty) {
  // difficulty 1-5 increases fall speed
  uint16_t fall_speed = 100 + (difficulty * 50);  // pixels per second
  uint8_t objects_total = 10 + (difficulty * 5);  // 15-35 objects

  // Score = objects caught / total objects * 100
  uint16_t base_score = 90 + (12 * difficulty);
  MinigameResult result = {
    .score = base_score,
    .success = (base_score > (difficulty * 30)),
    .difficulty = difficulty,
    .player_skill = 68
  };
  return result;
}

// ============================================================================
// MINIGAME: SWIM DIVE (PT_SWIM)
// ============================================================================
// Player must navigate through water avoiding obstacles
// Score = distance traveled

static inline MinigameResult minigame_swim_dive(uint8_t difficulty) {
  // difficulty 1-5 increases water resistance and obstacles
  uint16_t max_depth = 300 + (difficulty * 80);
  uint8_t hazards = 8 + (difficulty * 3);  // 11-23 hazards

  // Score = depth reached
  uint16_t base_score = 100 + (18 * difficulty);
  MinigameResult result = {
    .score = base_score,
    .success = (base_score > (difficulty * 35)),
    .difficulty = difficulty,
    .player_skill = 62
  };
  return result;
}

// ============================================================================
// MINIGAME: CLIMB JUMP (PT_CLIMB)
// ============================================================================
// Player must climb higher by jumping on platforms
// Score = maximum height reached

static inline MinigameResult minigame_climb_jump(uint8_t difficulty) {
  // difficulty 1-5 increases jump precision needed
  uint16_t max_height = 600 + (difficulty * 150);
  uint8_t platforms = 15 + (difficulty * 3);  // 18-30 platforms

  // Score = height reached
  uint16_t base_score = 110 + (22 * difficulty);
  MinigameResult result = {
    .score = base_score,
    .success = (base_score > (difficulty * 38)),
    .difficulty = difficulty,
    .player_skill = 71
  };
  return result;
}

// ============================================================================
// MINIGAME: DIG TUNNEL (PT_DIG)
// ============================================================================
// Player must dig through ground to reach target depth
// Score = depth reached

static inline MinigameResult minigame_dig_tunnel(uint8_t difficulty) {
  // difficulty 1-5 increases ground hardness
  uint16_t max_depth = 400 + (difficulty * 100);
  uint8_t rocks = 10 + (difficulty * 4);  // 14-30 rocks to break

  // Score = depth dug
  uint16_t base_score = 100 + (20 * difficulty);
  MinigameResult result = {
    .score = base_score,
    .success = (base_score > (difficulty * 35)),
    .difficulty = difficulty,
    .player_skill = 65
  };
  return result;
}

// ============================================================================
// MINIGAME: HIDE & SEEK (PT_HIDE)
// ============================================================================
// Player must find hidden items within time limit
// Score = number of items found

static inline MinigameResult minigame_hide_seek(uint8_t difficulty) {
  // difficulty 1-5 increases hiding difficulty
  uint16_t time_limit = 25000 - (difficulty * 4000);
  uint8_t items_total = 8 + (difficulty * 3);  // 11-23 items

  // Score = items found
  uint16_t base_score = 95 + (15 * difficulty);
  MinigameResult result = {
    .score = base_score,
    .success = (base_score > (difficulty * 32)),
    .difficulty = difficulty,
    .player_skill = 68
  };
  return result;
}

// ============================================================================
// MINIGAME: VENOM DODGE (PT_VENOM)
// ============================================================================
// Player must avoid poisonous obstacles
// Score = time survived

static inline MinigameResult minigame_venom_dodge(uint8_t difficulty) {
  // difficulty 1-5 increases hazard speed and frequency
  uint16_t max_time = 20000 - (difficulty * 3000);
  uint8_t hazard_frequency = 50 + (difficulty * 20);  // ms between hazards

  // Score = time survived (in seconds)
  uint16_t base_score = 100 + (18 * difficulty);
  MinigameResult result = {
    .score = base_score,
    .success = (base_score > (difficulty * 33)),
    .difficulty = difficulty,
    .player_skill = 70
  };
  return result;
}

// ============================================================================
// MINIGAME: ARMOR DEFENSE (PT_ARMOR)
// ============================================================================
// Player must block incoming attacks
// Score = number of successful blocks

static inline MinigameResult minigame_armor_defense(uint8_t difficulty) {
  // difficulty 1-5 increases attack speed
  uint16_t time_limit = 25000 - (difficulty * 3500);
  uint8_t attacks = 12 + (difficulty * 4);  // 16-32 attacks

  // Score = attacks blocked
  uint16_t base_score = 105 + (20 * difficulty);
  MinigameResult result = {
    .score = base_score,
    .success = (base_score > (difficulty * 36)),
    .difficulty = difficulty,
    .player_skill = 72
  };
  return result;
}

// ============================================================================
// MINIGAME: SIZE PUSH (PT_SIZE)
// ============================================================================
// Player must push increasingly heavy objects
// Score = weight pushed

static inline MinigameResult minigame_size_push(uint8_t difficulty) {
  // difficulty 1-5 increases starting weight
  uint16_t max_weight = 500 + (difficulty * 200);  // 700-1500 units
  uint16_t push_threshold = 100 + (difficulty * 40);

  // Score = maximum weight pushed
  uint16_t base_score = 110 + (25 * difficulty);
  MinigameResult result = {
    .score = base_score,
    .success = (base_score > (difficulty * 40)),
    .difficulty = difficulty,
    .player_skill = 66
  };
  return result;
}

// ============================================================================
// UNIFIED MINIGAME RUNNER
// ============================================================================

/**
 * Run appropriate minigame based on animal's primary power type
 * Returns score and success status
 */
static inline MinigameResult run_minigame_for_animal(uint8_t animal_id) {
  if (animal_id > DEX_COUNT) {
    return (MinigameResult){.score = 0, .success = false, .difficulty = 1, .player_skill = 0};
  }

  const DexEntry &entry = DEX_TBL[animal_id];
  uint8_t minigame = get_primary_minigame(animal_id);
  uint8_t difficulty = get_minigame_difficulty(entry.rarity);

  // Route to appropriate minigame based on power type
  switch (minigame) {
    case MINIGAME_STRENGTH_PUSH:
      return minigame_strength_push(difficulty);
    case MINIGAME_SPEED_RACE:
      return minigame_speed_race(difficulty);
    case MINIGAME_AGILITY_CLIMB:
      return minigame_agility_climb(difficulty);
    case MINIGAME_INTELLIGENCE_PUZZLE:
      return minigame_intelligence_puzzle(difficulty);
    case MINIGAME_FLIGHT_CATCH:
      return minigame_flight_catch(difficulty);
    case MINIGAME_SWIM_DIVE:
      return minigame_swim_dive(difficulty);
    case MINIGAME_CLIMB_JUMP:
      return minigame_climb_jump(difficulty);
    case MINIGAME_DIG_TUNNEL:
      return minigame_dig_tunnel(difficulty);
    case MINIGAME_HIDE_SEEK:
      return minigame_hide_seek(difficulty);
    case MINIGAME_VENOM_DODGE:
      return minigame_venom_dodge(difficulty);
    case MINIGAME_ARMOR_DEFENSE:
      return minigame_armor_defense(difficulty);
    case MINIGAME_SIZE_PUSH:
      return minigame_size_push(difficulty);
    default:
      return minigame_strength_push(difficulty);
  }
}

