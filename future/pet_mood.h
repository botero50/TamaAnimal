#pragma once

// ============================================================================
// PET MOOD SYSTEM - Phase 2 Engagement Improvement
// ============================================================================
// Pet mood affects minigame rewards - creates variable reward timing
// Players play minigames hoping pet is in good mood for bonus rewards
// Research: Variable rewards are 5x more engaging than fixed rewards

#include <stdint.h>
#include <time.h>

// Mood levels
enum PetMoodLevel {
  MOOD_UNHAPPY = 0,    // <40%:  0.8x rewards
  MOOD_TIRED = 1,      // 40-60%: Normal rewards
  MOOD_NEUTRAL = 2,    // 40-60%: Normal rewards
  MOOD_CONTENT = 3,    // 60-80%: 1.2x rewards
  MOOD_HAPPY = 4,      // >80%:  1.5x rewards
};

// Mood names
static const char *MOOD_NAMES[] = {
  "Unhappy",
  "Tired",
  "Neutral",
  "Content",
  "Happy",
};

// Mood emojis
static const char *MOOD_EMOJIS[] = {
  "😟",  // Unhappy
  "😴",  // Tired
  "😐",  // Neutral
  "😊",  // Content
  "😍",  // Happy
};

struct PetMood {
  uint8_t mood_percent;          // 0-100 scale
  uint32_t last_play_time;       // Unix timestamp of last play (minigame)
  uint32_t last_care_time;       // Unix timestamp of last care action
  uint32_t last_walk_time;       // Unix timestamp of last walking
  uint16_t recent_steps;         // Steps in last hour (for walking bonus)
};

// ============================================================================
// Initialize pet mood
// ============================================================================
static inline void pet_mood_init(PetMood *mood) {
  mood->mood_percent = 50;       // Start neutral
  uint32_t now = time(NULL);
  mood->last_play_time = now;
  mood->last_care_time = now;
  mood->last_walk_time = now;
  mood->recent_steps = 0;
}

// ============================================================================
// Record a play session (minigame)
// ============================================================================
static inline void pet_mood_record_play(PetMood *mood) {
  mood->last_play_time = time(NULL);
}

// ============================================================================
// Record a care action (feed/clean/sleep)
// ============================================================================
static inline void pet_mood_record_care(PetMood *mood) {
  mood->last_care_time = time(NULL);
}

// ============================================================================
// Record walking activity
// ============================================================================
static inline void pet_mood_record_walking(PetMood *mood, uint16_t steps) {
  mood->last_walk_time = time(NULL);
  mood->recent_steps = steps;
}

// ============================================================================
// Calculate current mood (0-100%)
// Affected by:
// - Time since last play (-1% per 30 minutes)
// - Time since last care (-2% per hour)
// - Recent walking (+5% per 1000 steps in last hour)
// - Base: 50%
// ============================================================================
static inline uint8_t pet_mood_calculate(const PetMood *mood) {
  uint32_t now = time(NULL);
  int mood_value = 50;  // Neutral baseline

  // Time since play: -1% per 30 minutes (max -20%)
  uint32_t play_age_minutes = (now - mood->last_play_time) / 60;
  int play_penalty = (play_age_minutes / 30);  // 1% per 30 min
  if (play_penalty > 20) play_penalty = 20;
  mood_value -= play_penalty;

  // Time since care: -2% per hour (max -30%)
  uint32_t care_age_hours = (now - mood->last_care_time) / 3600;
  int care_penalty = care_age_hours * 2;
  if (care_penalty > 30) care_penalty = 30;
  mood_value -= care_penalty;

  // Recent walking bonus: +5% per 1000 steps (max +20%)
  uint32_t walk_age_hours = (now - mood->last_walk_time) / 3600;
  if (walk_age_hours <= 1) {  // Only within last hour
    int walk_bonus = (mood->recent_steps / 1000) * 5;
    if (walk_bonus > 20) walk_bonus = 20;
    mood_value += walk_bonus;
  }

  // Clamp to 0-100
  if (mood_value < 0) mood_value = 0;
  if (mood_value > 100) mood_value = 100;

  return (uint8_t)mood_value;
}

// ============================================================================
// Get mood level from percentage
// ============================================================================
static inline uint8_t pet_mood_get_level(uint8_t mood_percent) {
  if (mood_percent > 80) return MOOD_HAPPY;
  if (mood_percent > 60) return MOOD_CONTENT;
  if (mood_percent > 40) return MOOD_NEUTRAL;
  if (mood_percent >= 40) return MOOD_TIRED;
  return MOOD_UNHAPPY;
}

// ============================================================================
// Get minigame reward multiplier based on mood
// Returns: percentage multiplier (100 = 1.0x, 150 = 1.5x, etc.)
// ============================================================================
static inline uint16_t pet_mood_get_reward_multiplier(uint8_t mood_percent) {
  if (mood_percent > 80) return 150;  // 1.5x: Happy
  if (mood_percent > 60) return 120;  // 1.2x: Content
  if (mood_percent > 40) return 100;  // 1.0x: Neutral
  return 80;                           // 0.8x: Unhappy
}

// ============================================================================
// Get mood name
// ============================================================================
static inline const char *pet_mood_get_name(uint8_t mood_percent) {
  uint8_t level = pet_mood_get_level(mood_percent);
  if (level > MOOD_HAPPY) level = MOOD_HAPPY;
  return MOOD_NAMES[level];
}

// ============================================================================
// Get mood emoji
// ============================================================================
static inline const char *pet_mood_get_emoji(uint8_t mood_percent) {
  uint8_t level = pet_mood_get_level(mood_percent);
  if (level > MOOD_HAPPY) level = MOOD_HAPPY;
  return MOOD_EMOJIS[level];
}

// ============================================================================
// Get color for mood indicator (RGB565)
// ============================================================================
static inline uint16_t pet_mood_get_color(uint8_t mood_percent) {
  if (mood_percent > 80) return 0xF81F;  // Magenta: Happy
  if (mood_percent > 60) return 0x07E0;  // Green: Content
  if (mood_percent > 40) return 0xFFE0;  // Yellow: Neutral
  if (mood_percent >= 20) return 0xFB60; // Orange: Tired
  return 0xF800;                         // Red: Unhappy
}

// ============================================================================
// Get suggestion for improving mood
// ============================================================================
static inline const char *pet_mood_get_suggestion(uint8_t mood_percent) {
  if (mood_percent > 80) return "Pet is very happy!";
  if (mood_percent > 60) return "Pet is content.";
  if (mood_percent > 40) return "Play a minigame to cheer up!";
  if (mood_percent >= 20) return "Time for a nap? Or some care?";
  return "Pet needs attention!";
}

// ============================================================================
// Should show mood-based reward preview in minigame?
// ============================================================================
static inline bool pet_mood_should_show_preview(uint8_t mood_percent) {
  // Show preview if mood is extreme (very happy or unhappy)
  return mood_percent > 80 || mood_percent < 40;
}

// ============================================================================
// Update mood (call from game loop, calculate based on time since events)
// ============================================================================
static inline uint8_t pet_mood_update(PetMood *mood) {
  uint8_t current_mood = pet_mood_calculate(mood);
  mood->mood_percent = current_mood;
  return current_mood;
}

// ============================================================================
// Get rewards adjusted by mood
// Call after minigame win to apply mood multiplier
// ============================================================================
static inline uint16_t pet_mood_apply_multiplier(uint16_t base_reward, uint8_t mood_percent) {
  uint16_t multiplier = pet_mood_get_reward_multiplier(mood_percent);
  return (base_reward * multiplier) / 100;
}

// ============================================================================
// Serialize for NVS storage (20 bytes)
// ============================================================================
static inline void pet_mood_serialize(PetMood *mood, uint8_t *buffer) {
  buffer[0] = mood->mood_percent;
  uint32_t *p = (uint32_t *)(buffer + 1);
  p[0] = mood->last_play_time;
  p[1] = mood->last_care_time;
  p[2] = mood->last_walk_time;
  uint16_t *p2 = (uint16_t *)(buffer + 13);
  p2[0] = mood->recent_steps;
}

// ============================================================================
// Deserialize from NVS storage
// ============================================================================
static inline void pet_mood_deserialize(PetMood *mood, const uint8_t *buffer) {
  mood->mood_percent = buffer[0];
  const uint32_t *p = (const uint32_t *)(buffer + 1);
  mood->last_play_time = p[0];
  mood->last_care_time = p[1];
  mood->last_walk_time = p[2];
  const uint16_t *p2 = (const uint16_t *)(buffer + 13);
  mood->recent_steps = p2[0];
}
