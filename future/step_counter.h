#pragma once

// ============================================================================
// STEP COUNTER SYSTEM - Phase 1 Engagement Improvement
// ============================================================================
// Tracks daily steps and triggers guaranteed encounters at milestones
// Walking is the core engagement mechanic - make it visible and rewarding

#include <stdint.h>
#include <time.h>

#define STEPS_PER_ENCOUNTER 2000    // 2,000 steps = 1 guaranteed encounter
#define STEPS_PER_DAY_GOAL 10000    // Daily walking goal (visible to player)

struct StepCounter {
  uint32_t steps_today;           // Steps walked today
  uint32_t total_steps_alltime;   // Lifetime step counter
  uint32_t last_reset_day;        // Day (from epoch) when steps_today was reset
  uint32_t encounters_triggered;  // How many encounters from walking this session
};

// ============================================================================
// Initialize step counter
// ============================================================================
static inline void step_counter_init(StepCounter *counter) {
  counter->steps_today = 0;
  counter->total_steps_alltime = 0;
  counter->last_reset_day = (time(NULL) / 86400);
  counter->encounters_triggered = 0;
}

// ============================================================================
// Get current day (0-indexed from epoch)
// ============================================================================
static inline uint32_t step_counter_get_day() {
  time_t now = time(NULL);
  return now / 86400;
}

// ============================================================================
// Add a step to the counter
// Returns: number of new encounters triggered (0 or 1)
// ============================================================================
static inline uint8_t step_counter_add_step(StepCounter *counter) {
  uint32_t current_day = step_counter_get_day();

  // Reset daily counter if new day
  if (current_day > counter->last_reset_day) {
    counter->steps_today = 0;
    counter->last_reset_day = current_day;
  }

  counter->steps_today++;
  counter->total_steps_alltime++;

  // Check if encounter milestone reached
  uint8_t encounters = 0;
  if ((counter->steps_today % STEPS_PER_ENCOUNTER) == 0) {
    encounters = 1;
    counter->encounters_triggered++;
  }

  return encounters;
}

// ============================================================================
// Add multiple steps at once (batch update from IMU)
// Returns: number of new encounters triggered
// ============================================================================
static inline uint8_t step_counter_add_steps(StepCounter *counter, uint32_t step_count) {
  uint32_t current_day = step_counter_get_day();

  // Reset daily counter if new day
  if (current_day > counter->last_reset_day) {
    counter->steps_today = 0;
    counter->last_reset_day = current_day;
  }

  uint8_t encounters = 0;

  // For each batch of steps
  for (uint32_t i = 0; i < step_count; i++) {
    counter->steps_today++;
    counter->total_steps_alltime++;

    if ((counter->steps_today % STEPS_PER_ENCOUNTER) == 0) {
      encounters++;
      counter->encounters_triggered++;
    }
  }

  return encounters;
}

// ============================================================================
// Get steps today
// ============================================================================
static inline uint32_t step_counter_get_today(const StepCounter *counter) {
  return counter->steps_today;
}

// ============================================================================
// Get progress to next encounter (0-STEPS_PER_ENCOUNTER)
// ============================================================================
static inline uint32_t step_counter_get_progress(const StepCounter *counter) {
  return counter->steps_today % STEPS_PER_ENCOUNTER;
}

// ============================================================================
// Get steps remaining to next encounter
// ============================================================================
static inline uint32_t step_counter_get_remaining(const StepCounter *counter) {
  uint32_t progress = step_counter_get_progress(counter);
  return STEPS_PER_ENCOUNTER - progress;
}

// ============================================================================
// Get daily goal progress (0-100%)
// ============================================================================
static inline uint8_t step_counter_get_goal_percent(const StepCounter *counter) {
  if (STEPS_PER_DAY_GOAL == 0) return 100;
  uint32_t percent = (counter->steps_today * 100) / STEPS_PER_DAY_GOAL;
  if (percent > 100) percent = 100;
  return (uint8_t)percent;
}

// ============================================================================
// Get total lifetime steps
// ============================================================================
static inline uint32_t step_counter_get_alltime(const StepCounter *counter) {
  return counter->total_steps_alltime;
}

// ============================================================================
// Get total encounters triggered from walking
// ============================================================================
static inline uint32_t step_counter_get_encounters_triggered(const StepCounter *counter) {
  return counter->encounters_triggered;
}

// ============================================================================
// Reset daily counter (called at midnight or on load)
// ============================================================================
static inline void step_counter_reset_daily(StepCounter *counter) {
  counter->steps_today = 0;
  counter->last_reset_day = step_counter_get_day();
}

// ============================================================================
// Serialize for NVS storage
// ============================================================================
static inline void step_counter_serialize(StepCounter *counter, uint8_t *buffer) {
  uint32_t *p = (uint32_t *)buffer;
  p[0] = counter->steps_today;
  p[1] = counter->total_steps_alltime;
  p[2] = counter->last_reset_day;
  p[3] = counter->encounters_triggered;
}

// ============================================================================
// Deserialize from NVS storage
// ============================================================================
static inline void step_counter_deserialize(StepCounter *counter, const uint8_t *buffer) {
  const uint32_t *p = (const uint32_t *)buffer;
  counter->steps_today = p[0];
  counter->total_steps_alltime = p[1];
  counter->last_reset_day = p[2];
  counter->encounters_triggered = p[3];
}
