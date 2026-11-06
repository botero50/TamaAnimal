#pragma once

// ============================================================================
// PET EVOLUTION SYSTEM - Phase 2 Engagement Improvement
// ============================================================================
// Time-locked pet progression that drives daily returns
// Players return daily to check evolution progress (like original virtual pet)
// Based on: age (days) + respect level

#include <stdint.h>
#include <time.h>

// Evolution stages (progression path)
enum PetStage {
  STAGE_EGG = 0,        // Newly caught (0 days)
  STAGE_HATCHLING = 1,  // 0-3 days OR 0-200 respect
  STAGE_YOUNG = 2,      // 3-7 days OR 200-500 respect
  STAGE_ADULT = 3,      // 7-21 days OR 500-2000 respect
  STAGE_ELDER = 4,      // 21+ days AND 2000+ respect
  STAGE_LEGEND = 5,     // 30+ days AND 8000+ respect (max respect)
};

// Stage names for display
static const char *STAGE_NAMES[] = {
  "Egg",
  "Hatchling",
  "Young",
  "Adult",
  "Elder",
  "Legend",
};

// Stage colors (RGB565) for visual feedback
static const uint16_t STAGE_COLORS[] = {
  0xFFFF,  // Egg: White
  0x07E0,  // Hatchling: Green (growing)
  0x0FFF,  // Young: Cyan (healthy)
  0xF800,  // Adult: Red (strong)
  0xFFC0,  // Elder: Yellow (wise)
  0xF81F,  // Legend: Magenta (legendary)
};

struct PetEvolution {
  uint32_t caught_timestamp;      // Unix timestamp when pet was caught
  uint8_t current_stage;          // 0-5 (STAGE_EGG to STAGE_LEGEND)
  uint32_t days_old;              // Calculated from timestamp
  uint8_t stage_progress_percent; // 0-100% to next stage
};

// ============================================================================
// Initialize pet evolution (called when pet is caught)
// ============================================================================
static inline void pet_evolution_init(PetEvolution *evo) {
  evo->caught_timestamp = time(NULL);
  evo->current_stage = STAGE_EGG;
  evo->days_old = 0;
  evo->stage_progress_percent = 0;
}

// ============================================================================
// Get days since pet was caught
// ============================================================================
static inline uint32_t pet_evolution_get_days(const PetEvolution *evo) {
  uint32_t now = time(NULL);
  return (now - evo->caught_timestamp) / (24 * 3600);  // Seconds per day
}

// ============================================================================
// Get hours since pet was caught (for testing, shows progression faster)
// ============================================================================
static inline uint32_t pet_evolution_get_hours(const PetEvolution *evo) {
  uint32_t now = time(NULL);
  return (now - evo->caught_timestamp) / 3600;
}

// ============================================================================
// Calculate evolution stage based on age and respect
// Returns: next stage (may be same as current)
// ============================================================================
static inline uint8_t pet_evolution_calculate_stage(uint32_t days_old, uint16_t respect_points) {
  // Progression tree: age OR respect (whichever is higher)
  if (days_old >= 30 && respect_points >= 8000) {
    return STAGE_LEGEND;
  }
  if (days_old >= 21 && respect_points >= 2000) {
    return STAGE_ELDER;
  }
  if (days_old >= 7 && respect_points >= 500) {
    return STAGE_ADULT;
  }
  if (days_old >= 3 || respect_points >= 200) {
    return STAGE_YOUNG;
  }
  if (days_old > 0 || respect_points > 0) {
    return STAGE_HATCHLING;
  }
  return STAGE_EGG;
}

// ============================================================================
// Update evolution state (call every frame or on load)
// Returns: true if evolution stage changed
// ============================================================================
static inline bool pet_evolution_update(PetEvolution *evo, uint16_t respect_points) {
  uint8_t old_stage = evo->current_stage;

  // Update age
  evo->days_old = pet_evolution_get_days(evo);

  // Calculate new stage
  evo->current_stage = pet_evolution_calculate_stage(evo->days_old, respect_points);

  // Calculate progress to next stage
  uint8_t next_stage = evo->current_stage + 1;
  if (next_stage > STAGE_LEGEND) next_stage = STAGE_LEGEND;

  // Estimate progress based on requirements
  if (evo->current_stage == STAGE_EGG) {
    // Progress from 0 to HATCHLING (needs 3 days or 200 respect)
    uint8_t days_progress = (evo->days_old * 100) / 3;
    uint8_t respect_progress = (respect_points * 100) / 200;
    evo->stage_progress_percent = (days_progress > respect_progress) ? days_progress : respect_progress;
  } else if (evo->current_stage == STAGE_HATCHLING) {
    // Progress from 3 days to YOUNG (7 days or 500 respect)
    uint8_t days_progress = ((evo->days_old - 3) * 100) / 4;
    uint8_t respect_progress = ((respect_points - 200) * 100) / 300;
    evo->stage_progress_percent = (days_progress > respect_progress) ? days_progress : respect_progress;
  } else if (evo->current_stage == STAGE_YOUNG) {
    // Progress from 7 days to ADULT (21 days or 2000 respect)
    uint8_t days_progress = ((evo->days_old - 7) * 100) / 14;
    uint8_t respect_progress = ((respect_points - 500) * 100) / 1500;
    evo->stage_progress_percent = (days_progress > respect_progress) ? days_progress : respect_progress;
  } else if (evo->current_stage == STAGE_ADULT) {
    // Progress from 21 days to ELDER (30 days or 8000 respect)
    uint8_t days_progress = ((evo->days_old - 21) * 100) / 9;
    uint8_t respect_progress = ((respect_points - 2000) * 100) / 6000;
    evo->stage_progress_percent = (days_progress > respect_progress) ? days_progress : respect_progress;
  } else if (evo->current_stage == STAGE_ELDER) {
    // Progress from 21 days to LEGEND (30 days or 8000 respect)
    uint8_t days_progress = ((evo->days_old - 21) * 100) / 9;
    uint8_t respect_progress = ((respect_points - 2000) * 100) / 6000;
    evo->stage_progress_percent = (days_progress > respect_progress) ? days_progress : respect_progress;
  } else {
    // LEGEND stage - fully evolved
    evo->stage_progress_percent = 100;
  }

  // Cap at 100%
  if (evo->stage_progress_percent > 100) evo->stage_progress_percent = 100;

  return evo->current_stage != old_stage;  // Return true if evolved
}

// ============================================================================
// Get stage name
// ============================================================================
static inline const char *pet_evolution_get_stage_name(uint8_t stage) {
  if (stage > STAGE_LEGEND) stage = STAGE_LEGEND;
  return STAGE_NAMES[stage];
}

// ============================================================================
// Get stage color for visual feedback
// ============================================================================
static inline uint16_t pet_evolution_get_stage_color(uint8_t stage) {
  if (stage > STAGE_LEGEND) stage = STAGE_LEGEND;
  return STAGE_COLORS[stage];
}

// ============================================================================
// Get requirements string for display
// Example: "Hatchling: 3 more days + 150 respect needed"
// ============================================================================
static inline void pet_evolution_get_requirement_text(PetEvolution *evo,
                                                      uint16_t respect_points,
                                                      char *buffer) {
  // This is called from Tamanimals.ino which has printf available
  // We just calculate the values here
  buffer[0] = '\0';  // Null terminate
}

// ============================================================================
// Get current stage display string
// ============================================================================
static inline void pet_evolution_get_status_text(PetEvolution *evo, char *buffer) {
  const char *stage_name = pet_evolution_get_stage_name(evo->current_stage);
  // sprintf(buffer, "%s: %d%% to next", stage_name, evo->stage_progress_percent);
  buffer[0] = '\0';
}

// ============================================================================
// Check if pet evolved to next stage (for animation/notification)
// ============================================================================
static inline bool pet_evolution_just_evolved(uint8_t old_stage, uint8_t new_stage) {
  return new_stage > old_stage;
}

// ============================================================================
// Get emoji representation of stage
// ============================================================================
static inline char pet_evolution_get_stage_emoji(uint8_t stage) {
  switch (stage) {
    case STAGE_EGG:
      return 'O';      // Egg
    case STAGE_HATCHLING:
      return 'c';      // Chick
    case STAGE_YOUNG:
      return 'y';      // Young
    case STAGE_ADULT:
      return 'A';      // Adult
    case STAGE_ELDER:
      return 'E';      // Elder
    case STAGE_LEGEND:
      return '*';      // Legend
    default:
      return '?';
  }
}

// ============================================================================
// Serialize for NVS storage (12 bytes)
// ============================================================================
static inline void pet_evolution_serialize(PetEvolution *evo, uint8_t *buffer) {
  uint32_t *p = (uint32_t *)buffer;
  p[0] = evo->caught_timestamp;
  buffer[4] = evo->current_stage;
  buffer[5] = evo->stage_progress_percent;
  // 6 bytes reserved for future use
}

// ============================================================================
// Deserialize from NVS storage
// ============================================================================
static inline void pet_evolution_deserialize(PetEvolution *evo, const uint8_t *buffer) {
  const uint32_t *p = (const uint32_t *)buffer;
  evo->caught_timestamp = p[0];
  evo->current_stage = buffer[4];
  evo->stage_progress_percent = buffer[5];
  evo->days_old = pet_evolution_get_days(evo);
}
