/**
 * Tamanimals - Respect System
 * Player-wide animal respect reputation
 *
 * Respect grows from caring for animals and winning minigames.
 * Higher respect increases catch rates for all animals.
 * Respect level ranges from 1-8 (8 = maximum respect).
 */

#pragma once
#include <stdint.h>

// ============================================================================
// RESPECT SYSTEM
// ============================================================================

typedef struct {
  uint16_t respect_points;     // 0-9999 total points
  uint8_t respect_level;       // Calculated from points (1-8)
  uint32_t last_earned_time;   // Timestamp of last respect gain (optional)
} RespectSystem;

// ============================================================================
// RESPECT LEVEL THRESHOLDS
// ============================================================================

// Respect points needed for each level:
// Level 1: 0+ points
// Level 2: 500+ points
// Level 3: 1000+ points
// Level 4: 2000+ points
// Level 5: 3500+ points
// Level 6: 5000+ points
// Level 7: 6500+ points
// Level 8: 8000+ points (max)

// ============================================================================
// RESPECT FUNCTIONS
// ============================================================================

/**
 * Initialize respect system
 * Start with some baseline respect
 */
static inline void respect_init(RespectSystem *sys) {
  sys->respect_points = 100;   // Start with level 1
  sys->respect_level = 1;
  sys->last_earned_time = 0;
}

/**
 * Recalculate respect level from points
 * Should be called after any respect gain
 */
static inline void respect_recalculate_level(RespectSystem *sys) {
  if (sys->respect_points < 500) {
    sys->respect_level = 1;
  } else if (sys->respect_points < 1000) {
    sys->respect_level = 2;
  } else if (sys->respect_points < 2000) {
    sys->respect_level = 3;
  } else if (sys->respect_points < 3500) {
    sys->respect_level = 4;
  } else if (sys->respect_points < 5000) {
    sys->respect_level = 5;
  } else if (sys->respect_points < 6500) {
    sys->respect_level = 6;
  } else if (sys->respect_points < 8000) {
    sys->respect_level = 7;
  } else {
    sys->respect_level = 8;  // Maximum
  }
}

/**
 * Add respect points for good gameplay
 * Points capped at 9999 (level 8)
 */
static inline void respect_add(RespectSystem *sys, uint16_t amount) {
  sys->respect_points = (sys->respect_points + amount > 9999) ? 9999 : sys->respect_points + amount;
  respect_recalculate_level(sys);
}

/**
 * Earn respect from minigame victory
 * +20 respect per win
 */
static inline void respect_earn_minigame_win(RespectSystem *sys) {
  respect_add(sys, 20);
}

/**
 * Earn respect from proper daily pet care
 * +10 respect for each care action (feed/clean/sleep)
 */
static inline void respect_earn_proper_care(RespectSystem *sys) {
  respect_add(sys, 10);
}

/**
 * Earn respect from daily login/play
 * +50 respect for logging in and caring for pet
 */
static inline void respect_earn_daily_bonus(RespectSystem *sys) {
  respect_add(sys, 50);
}

/**
 * Earn respect from tier advancement
 * +200 respect for significant achievement
 */
static inline void respect_earn_tier_up(RespectSystem *sys) {
  respect_add(sys, 200);
}

/**
 * Get catch rate bonus from respect level
 * Each level adds +10 to base catch rate
 * Level 1 = +10, Level 8 = +80
 */
static inline uint8_t respect_get_catch_bonus(uint8_t respect_level) {
  if (respect_level > 8) respect_level = 8;
  return respect_level * 10;  // 10-80 catch rate bonus
}

/**
 * Get progress to next respect level
 * Returns percentage (0-100)
 */
static inline uint8_t respect_get_progress_to_next(RespectSystem *sys) {
  uint16_t current_level_threshold[] = {0, 500, 1000, 2000, 3500, 5000, 6500, 8000, 9999};

  if (sys->respect_level >= 8) {
    return 100;  // Already at max
  }

  uint16_t current_threshold = current_level_threshold[sys->respect_level];
  uint16_t next_threshold = current_level_threshold[sys->respect_level + 1];

  if (next_threshold <= current_threshold) {
    return 100;
  }

  uint16_t progress = (sys->respect_points - current_threshold);
  uint16_t total_needed = next_threshold - current_threshold;

  return (uint8_t)((progress * 100) / total_needed);
}

/**
 * Get respect level name (for display)
 */
static inline const char* respect_get_level_name(uint8_t level) {
  switch (level) {
    case 1: return "Novice";
    case 2: return "Friendly";
    case 3: return "Trusted";
    case 4: return "Companion";
    case 5: return "Bond";
    case 6: return "Guardian";
    case 7: return "Keeper";
    case 8: return "Legend";
    default: return "Unknown";
  }
}

/**
 * Get description of current respect level
 */
static inline const char* respect_get_description(uint8_t level) {
  switch (level) {
    case 1: return "Animals barely know you";
    case 2: return "Some animals show interest";
    case 3: return "Animals starting to trust you";
    case 4: return "Clear bond with animals";
    case 5: return "Deep connection formed";
    case 6: return "Animals seek you out";
    case 7: return "You are their protector";
    case 8: return "Legendary animal companion";
    default: return "";
  }
}

/**
 * Check if respect level increased
 * Call this to detect tier-ups for UI notifications
 */
static inline bool respect_leveled_up(uint8_t old_level, uint8_t new_level) {
  return new_level > old_level;
}

/**
 * Get points needed for next level
 * Returns 0 if already at max
 */
static inline uint16_t respect_get_points_to_next_level(RespectSystem *sys) {
  uint16_t next_level_threshold[] = {500, 1000, 2000, 3500, 5000, 6500, 8000, 9999};

  if (sys->respect_level >= 8) {
    return 0;  // Already at max
  }

  uint16_t needed = next_level_threshold[sys->respect_level - 1];
  if (needed <= sys->respect_points) {
    return 0;  // Already reached next level
  }

  return needed - sys->respect_points;
}
