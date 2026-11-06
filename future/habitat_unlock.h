#pragma once

// ============================================================================
// HABITAT UNLOCK SYSTEM - Phase 4
// ============================================================================
// Unlock habitats by walking distance (links walking to progression)

#include <stdint.h>

#define HABITAT_COUNT 11

enum HabitatID {
  HABITAT_AFRICA = 0,
  HABITAT_ASIA = 1,
  HABITAT_EUROPE = 2,
  HABITAT_SOUTH_AMERICA = 3,
  HABITAT_NORTH_AMERICA = 4,
  HABITAT_AUSTRALIA = 5,
  HABITAT_ARCTIC = 6,
  HABITAT_OCEAN = 7,
  HABITAT_RAINFOREST = 8,
  HABITAT_EXTINCT = 9,
  HABITAT_MAGIC = 10,
};

struct HabitatUnlock {
  uint8_t habitat_id;
  const char *name;
  const char *description;
  uint32_t distance_km_required;
  uint16_t animal_count;
  uint16_t bonus_catch_rate;  // +% catch rate when mastered
};

static const HabitatUnlock HABITAT_UNLOCKS[HABITAT_COUNT] = {
  {HABITAT_AFRICA, "Africa", "Savanna and grasslands", 0, 25, 0},
  {HABITAT_ASIA, "Asia", "Mountains and forests", 25, 35, 0},
  {HABITAT_EUROPE, "Europe", "Temperate forests", 50, 20, 0},
  {HABITAT_SOUTH_AMERICA, "South America", "Rainforests and plains", 100, 35, 0},
  {HABITAT_NORTH_AMERICA, "North America", "Mixed habitats", 150, 25, 0},
  {HABITAT_AUSTRALIA, "Australia", "Unique island fauna", 200, 20, 0},
  {HABITAT_ARCTIC, "Arctic", "Polar regions", 250, 15, 0},
  {HABITAT_OCEAN, "Ocean", "Marine life", 150, 30, 0},
  {HABITAT_RAINFOREST, "Rainforest", "Tropical diversity", 180, 40, 0},
  {HABITAT_EXTINCT, "Extinct", "Prehistoric animals", 75, 25, 0},
  {HABITAT_MAGIC, "Magic", "Mythical creatures", 100, 28, 0},
};

struct HabitatUnlockState {
  bool unlocked[HABITAT_COUNT];
  uint32_t unlocked_at[HABITAT_COUNT];
  uint8_t total_unlocked;
};

// ============================================================================
// Initialize habitat unlocks
// ============================================================================
static inline void habitat_unlock_init(HabitatUnlockState *state) {
  for (int i = 0; i < HABITAT_COUNT; i++) {
    state->unlocked[i] = false;
    state->unlocked_at[i] = 0;
  }
  state->unlocked[HABITAT_AFRICA] = true;  // Start with Africa
  state->unlocked_at[HABITAT_AFRICA] = time(NULL);
  state->total_unlocked = 1;
}

// ============================================================================
// Check for habitat unlocks based on distance
// ============================================================================
static inline uint8_t habitat_unlock_check(HabitatUnlockState *state, uint32_t total_steps_km) {
  uint8_t newly_unlocked = 0;

  for (int i = 0; i < HABITAT_COUNT; i++) {
    if (!state->unlocked[i] && total_steps_km >= HABITAT_UNLOCKS[i].distance_km_required) {
      state->unlocked[i] = true;
      state->unlocked_at[i] = time(NULL);
      state->total_unlocked++;
      newly_unlocked = i;
    }
  }

  return newly_unlocked;
}

// ============================================================================
// Get habitat name
// ============================================================================
static inline const char *habitat_unlock_get_name(uint8_t habitat_id) {
  if (habitat_id >= HABITAT_COUNT) return "Unknown";
  return HABITAT_UNLOCKS[habitat_id].name;
}

// ============================================================================
// Get distance required
// ============================================================================
static inline uint32_t habitat_unlock_get_requirement(uint8_t habitat_id) {
  if (habitat_id >= HABITAT_COUNT) return 999999;
  return HABITAT_UNLOCKS[habitat_id].distance_km_required;
}

// ============================================================================
// Get KM remaining to unlock
// ============================================================================
static inline uint32_t habitat_unlock_get_remaining(const HabitatUnlockState *state,
                                                     uint8_t habitat_id, uint32_t current_km) {
  if (habitat_id >= HABITAT_COUNT) return 0;
  if (state->unlocked[habitat_id]) return 0;

  uint32_t required = HABITAT_UNLOCKS[habitat_id].distance_km_required;
  if (current_km >= required) return 0;
  return required - current_km;
}

// ============================================================================
// Check if habitat unlocked
// ============================================================================
static inline bool habitat_unlock_is_unlocked(const HabitatUnlockState *state, uint8_t habitat_id) {
  if (habitat_id >= HABITAT_COUNT) return false;
  return state->unlocked[habitat_id];
}

// ============================================================================
// Get total habitats unlocked
// ============================================================================
static inline uint8_t habitat_unlock_get_total(const HabitatUnlockState *state) {
  return state->total_unlocked;
}

// ============================================================================
// Serialize habitat state
// ============================================================================
static inline void habitat_unlock_serialize(const HabitatUnlockState *state, uint8_t *buffer) {
  for (int i = 0; i < HABITAT_COUNT; i++) {
    buffer[i] = state->unlocked[i] ? 1 : 0;
  }
}

// ============================================================================
// Deserialize habitat state
// ============================================================================
static inline void habitat_unlock_deserialize(HabitatUnlockState *state, const uint8_t *buffer) {
  for (int i = 0; i < HABITAT_COUNT; i++) {
    state->unlocked[i] = buffer[i] != 0;
    if (state->unlocked[i]) state->total_unlocked++;
  }
}
