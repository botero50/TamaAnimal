/**
 * Tamanimals Integration Layer
 * Bridges old Pet system with new Tamanimals systems
 * Enables gradual integration without breaking existing code
 */

#pragma once
#include "game_systems.h"
#include "pet_tamanimals.h"
#include "minigames_impl.h"
#include "dex.h"

// ============================================================================
// INTEGRATION STATE
// ============================================================================

// Global pet in Tamanimals format
Pet tamanimals_pet;

// Progression tracking
struct {
  uint16_t total_caught;
  uint8_t current_tier;
  uint8_t tier_progress;
} tamanimals_progression;

// Last minigame result
MinigameResult last_minigame_result;

// ============================================================================
// INITIALIZATION
// ============================================================================

/**
 * Initialize Tamanimals systems
 * Call this in setup() after loading saved pet
 */
static inline void tamanimals_init() {
  // Create starter pet if new game
  tamanimals_pet = pet_new(1, "Lion", false);  // Start with Lion

  // Initialize progression
  tamanimals_progression.total_caught = 1;  // Have starter
  tamanimals_progression.current_tier = TIER_NOVICE;
  tamanimals_progression.tier_progress = 0;

  // Initialize minigame result
  last_minigame_result = {0, false, 1, 0};
}

// ============================================================================
// PET LIFECYCLE - Call these in game loop
// ============================================================================

/**
 * Update pet statistics (natural decay)
 * Call in game loop: tamanimals_tick(deltaMs / 1000)
 */
static inline void tamanimals_tick(uint32_t seconds_elapsed) {
  pet_tick(&tamanimals_pet, seconds_elapsed);
}

/**
 * Feed the pet
 * Call when player presses FEED button
 */
static inline void tamanimals_feed() {
  pet_feed(&tamanimals_pet);
}

/**
 * Clean the pet
 * Call when player presses CLEAN button
 */
static inline void tamanimals_clean() {
  pet_clean(&tamanimals_pet);
}

/**
 * Let pet sleep
 * Call when player presses SLEEP button
 */
static inline void tamanimals_sleep() {
  pet_sleep(&tamanimals_pet);
}

/**
 * Play minigame
 * Call when player presses PLAY button
 * Returns success status
 */
static inline bool tamanimals_play_minigame() {
  // Run appropriate minigame for this animal
  last_minigame_result = run_minigame_for_animal(tamanimals_pet.species_id);

  // Apply results to pet
  pet_play_minigame(&tamanimals_pet, last_minigame_result.score, last_minigame_result.success);

  // ========================================================================
  // PHASE 7: Reward minigame tokens and respect (Phase 7c Step 5)
  // ========================================================================
  // This will be called from the main game loop where catching system is available
  // For now, just store the result - actual rewards are applied in Tamanimals.ino

  // Calculate and apply reward
  if (last_minigame_result.success) {
    uint8_t reward = calculate_minigame_reward(last_minigame_result.score,
                                               last_minigame_result.difficulty);
    tamanimals_pet.exp += reward;

    // Check for level up
    uint16_t exp_needed = tamanimals_pet.level * 100;
    if (tamanimals_pet.exp >= exp_needed) {
      tamanimals_pet.level++;
      tamanimals_pet.exp -= exp_needed;
    }
  }

  return last_minigame_result.success;
}

// ============================================================================
// CATCHING SYSTEM
// ============================================================================

/**
 * Attempt to catch a wild animal
 * Called when player tries to catch
 */
static inline bool tamanimals_attempt_catch(uint8_t animal_id,
                                           uint8_t catch_ball_quality) {
  // Get minigame bonus from last result
  uint8_t minigame_bonus = last_minigame_result.success ? 50 : 0;

  // Attempt catch with system
  if (attempt_catch(animal_id, catch_ball_quality, minigame_bonus)) {
    // Success! Create new pet and add to collection
    Pet caught = pet_new(animal_id, "", true);

    // Add to progression
    tamanimals_progression.total_caught++;

    // Update tier if needed
    uint8_t new_tier = get_progression_tier(tamanimals_progression.total_caught);
    if (new_tier > tamanimals_progression.current_tier) {
      tamanimals_progression.current_tier = new_tier;
      // Tier up! Show notification
      // show_message("Advanced to %s!", get_tier_name(new_tier));
    }

    tamanimals_progression.tier_progress =
      get_tier_progress(tamanimals_progression.total_caught);

    return true;
  }

  return false;
}

// ============================================================================
// DISPLAY FUNCTIONS - Use in rendering code
// ============================================================================

/**
 * Get pet name (nickname or species name)
 */
static inline const char* tamanimals_get_pet_name() {
  if (tamanimals_pet.nickname[0] != '\0') {
    return tamanimals_pet.nickname;
  }
  return DEX_TBL[tamanimals_pet.species_id].name;
}

/**
 * Get current tier name
 */
static inline const char* tamanimals_get_tier_name() {
  return get_tier_name(tamanimals_progression.current_tier);
}

/**
 * Get animals caught string
 */
static inline uint16_t tamanimals_get_caught_count() {
  return tamanimals_progression.total_caught;
}

/**
 * Get tier progress percentage
 */
static inline uint8_t tamanimals_get_tier_progress() {
  return tamanimals_progression.tier_progress;
}

/**
 * Get pet level
 */
static inline uint8_t tamanimals_get_level() {
  return tamanimals_pet.level;
}

/**
 * Get stat value (0-255)
 * stat_id: 0=hunger, 1=hygiene, 2=joy, 3=energy, 4=bond, 5=health
 */
static inline uint8_t tamanimals_get_stat(uint8_t stat_id) {
  switch (stat_id) {
    case 0: return tamanimals_pet.stats.hunger;
    case 1: return tamanimals_pet.stats.hygiene;
    case 2: return tamanimals_pet.stats.joy;
    case 3: return tamanimals_pet.stats.energy;
    case 4: return tamanimals_pet.stats.bond;
    case 5: return tamanimals_pet.stats.health;
    default: return 0;
  }
}

/**
 * Get stat visual bar (for drawing)
 */
static inline uint8_t tamanimals_get_stat_percent(uint8_t stat_id) {
  uint8_t stat = tamanimals_get_stat(stat_id);
  return (stat * 100) / 255;  // Convert 0-255 to 0-100
}

// ============================================================================
// SAVE/LOAD
// ============================================================================

/**
 * Prepare pet data for saving
 * Copy Tamanimals pet to save structure
 */
static inline void tamanimals_prepare_save() {
  // This would copy tamanimals_pet to whatever save structure you use
  // Format: serialize tamanimals_pet + tamanimals_progression to bytes
}

/**
 * Load saved pet data
 * Copy from save structure to Tamanimals pet
 */
static inline void tamanimals_load_save() {
  // This would deserialize saved pet data into tamanimals_pet
  // If no save exists, tamanimals_init() will create new pet
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

/**
 * Get animal name by ID
 */
static inline const char* tamanimals_get_animal_name(uint8_t animal_id) {
  if (animal_id > DEX_COUNT) return "???";
  return DEX_TBL[animal_id].name;
}

/**
 * Get power type name
 */
static inline const char* tamanimals_get_power_type_name(uint8_t power_type) {
  switch (power_type) {
    case PT_STRENGTH: return "Strength";
    case PT_SPEED: return "Speed";
    case PT_AGILITY: return "Agility";
    case PT_INTELLIGENCE: return "Intelligence";
    case PT_FLIGHT: return "Flight";
    case PT_SWIM: return "Swimming";
    case PT_CLIMB: return "Climbing";
    case PT_DIG: return "Digging";
    case PT_HIDE: return "Hiding";
    case PT_VENOM: return "Venom";
    case PT_ARMOR: return "Armor";
    case PT_SIZE: return "Size";
    case PT_SOUND: return "Sound";
    case PT_FREEZE: return "Freeze";
    case PT_PACK: return "Pack";
    case PT_HERD: return "Herd";
    case PT_NOCTURNAL: return "Nocturnal";
    case PT_DIURNAL: return "Diurnal";
    case PT_HERBIVORE: return "Herbivore";
    case PT_CARNIVORE: return "Carnivore";
    default: return "Unknown";
  }
}

/**
 * Check if animal is extinct
 */
static inline bool tamanimals_is_extinct(uint8_t animal_id) {
  return is_extinct_animal(animal_id);
}

