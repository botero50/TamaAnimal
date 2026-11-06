/**
 * Sprite Animation States - Emotional & Status States for Animals
 * Supports sad, happy, poisoned, sick, sleeping, excited, scared, and more
 * Each state has multiple animation frames for smooth transitions
 */

#pragma once
#include <stdint.h>
#include <string.h>

// Animation states for sprites
typedef enum {
  STATE_NORMAL = 0,      // Default idle/movement state
  STATE_HAPPY = 1,       // Joyful, celebrating
  STATE_SAD = 2,         // Unhappy, crying
  STATE_SICK = 3,        // Ill, weak
  STATE_POISONED = 4,    // Poisoned, green tint
  STATE_SLEEPING = 5,    // Sleeping, eyes closed
  STATE_EXCITED = 6,     // Very happy, jumping
  STATE_SCARED = 7,      // Frightened, shaking
  STATE_ANGRY = 8,       // Angry, aggressive
  STATE_CONFUSED = 9,    // Confused, dizzy
  STATE_FROZEN = 10,     // Frozen/stunned
  STATE_BURNING = 11,    // On fire, red tint
  STATE_COUNT = 12       // Total number of states
} SpriteAnimationState;

// State metadata
typedef struct {
  SpriteAnimationState state;
  const char *name;
  const char *description;
  uint8_t animation_frame_count;  // How many unique frames per state
  uint32_t frame_duration_ms;     // How long each frame displays
  bool looping;                    // Whether animation loops
  uint8_t severity;               // 0=positive, 5=neutral, 10=negative (for UI tinting)
  uint32_t tint_color;            // RGB565 color for state (0=none)
} SpriteStateMetadata;

// State metadata table
static const SpriteStateMetadata SPRITE_STATE_INFO[STATE_COUNT] = {
  // STATE_NORMAL
  {
    .state = STATE_NORMAL,
    .name = "Normal",
    .description = "Happy and healthy",
    .animation_frame_count = 4,
    .frame_duration_ms = 200,
    .looping = true,
    .severity = 5,
    .tint_color = 0x0000  // No tint
  },

  // STATE_HAPPY
  {
    .state = STATE_HAPPY,
    .name = "Happy",
    .description = "Very joyful",
    .animation_frame_count = 6,
    .frame_duration_ms = 150,
    .looping = true,
    .severity = 0,
    .tint_color = 0xFFFF  // Yellow tint (RGB565)
  },

  // STATE_SAD
  {
    .state = STATE_SAD,
    .name = "Sad",
    .description = "Unhappy, needs care",
    .animation_frame_count = 5,
    .frame_duration_ms = 300,
    .looping = true,
    .severity = 9,
    .tint_color = 0x001F  // Blue tint
  },

  // STATE_SICK
  {
    .state = STATE_SICK,
    .name = "Sick",
    .description = "Ill and weak",
    .animation_frame_count = 4,
    .frame_duration_ms = 250,
    .looping = true,
    .severity = 8,
    .tint_color = 0x07E0  // Green tint
  },

  // STATE_POISONED
  {
    .state = STATE_POISONED,
    .name = "Poisoned",
    .description = "Toxic status effect",
    .animation_frame_count = 5,
    .frame_duration_ms = 200,
    .looping = true,
    .severity = 10,
    .tint_color = 0x07E0  // Green tint
  },

  // STATE_SLEEPING
  {
    .state = STATE_SLEEPING,
    .name = "Sleeping",
    .description = "Resting peacefully",
    .animation_frame_count = 3,
    .frame_duration_ms = 400,
    .looping = true,
    .severity = 5,
    .tint_color = 0xB8E6  // Light blue tint
  },

  // STATE_EXCITED
  {
    .state = STATE_EXCITED,
    .name = "Excited",
    .description = "Extremely happy!",
    .animation_frame_count = 8,
    .frame_duration_ms = 100,
    .looping = true,
    .severity = 0,
    .tint_color = 0xFFFF  // Yellow tint
  },

  // STATE_SCARED
  {
    .state = STATE_SCARED,
    .name = "Scared",
    .description = "Frightened and anxious",
    .animation_frame_count = 6,
    .frame_duration_ms = 150,
    .looping = true,
    .severity = 9,
    .tint_color = 0xF800  // Red tint
  },

  // STATE_ANGRY
  {
    .state = STATE_ANGRY,
    .name = "Angry",
    .description = "Aggressive and upset",
    .animation_frame_count = 5,
    .frame_duration_ms = 180,
    .looping = true,
    .severity = 8,
    .tint_color = 0xF800  // Red tint
  },

  // STATE_CONFUSED
  {
    .state = STATE_CONFUSED,
    .name = "Confused",
    .description = "Disoriented and dizzy",
    .animation_frame_count = 4,
    .frame_duration_ms = 160,
    .looping = true,
    .severity = 6,
    .tint_color = 0xA514  // Purple tint
  },

  // STATE_FROZEN
  {
    .state = STATE_FROZEN,
    .name = "Frozen",
    .description = "Ice status effect",
    .animation_frame_count = 3,
    .frame_duration_ms = 300,
    .looping = true,
    .severity = 7,
    .tint_color = 0x041F  // Cyan tint
  },

  // STATE_BURNING
  {
    .state = STATE_BURNING,
    .name = "Burning",
    .description = "Fire status effect",
    .animation_frame_count = 6,
    .frame_duration_ms = 150,
    .looping = true,
    .severity = 10,
    .tint_color = 0xF800  // Red tint
  }
};

/**
 * Get state metadata by state enum
 */
static inline const SpriteStateMetadata* sprite_state_get_info(SpriteAnimationState state) {
  if (state < 0 || state >= STATE_COUNT) {
    return &SPRITE_STATE_INFO[STATE_NORMAL];  // Default to normal
  }
  return &SPRITE_STATE_INFO[state];
}

/**
 * Get state name string
 */
static inline const char* sprite_state_get_name(SpriteAnimationState state) {
  return sprite_state_get_info(state)->name;
}

/**
 * Get state description
 */
static inline const char* sprite_state_get_description(SpriteAnimationState state) {
  return sprite_state_get_info(state)->description;
}

/**
 * Check if state is negative/bad (severity > 5)
 */
static inline bool sprite_state_is_bad(SpriteAnimationState state) {
  return sprite_state_get_info(state)->severity > 5;
}

/**
 * Check if state is positive/good (severity < 5)
 */
static inline bool sprite_state_is_good(SpriteAnimationState state) {
  return sprite_state_get_info(state)->severity < 5;
}

/**
 * Get current animation frame based on time and state
 */
static inline int sprite_state_get_frame(SpriteAnimationState state, uint32_t elapsed_ms) {
  const SpriteStateMetadata *info = sprite_state_get_info(state);

  if (info->frame_duration_ms == 0) {
    return 0;
  }

  uint32_t frame_index = (elapsed_ms / info->frame_duration_ms) % info->animation_frame_count;
  return (int)frame_index;
}

/**
 * Calculate animation progress (0.0 = start, 1.0 = end)
 */
static inline float sprite_state_get_animation_progress(SpriteAnimationState state, uint32_t elapsed_ms) {
  const SpriteStateMetadata *info = sprite_state_get_info(state);

  if (!info->looping) {
    uint32_t total_duration = info->animation_frame_count * info->frame_duration_ms;
    float progress = (float)elapsed_ms / total_duration;
    return (progress > 1.0f) ? 1.0f : progress;
  }

  return 0.0f;  // Looping animations don't have a fixed progress
}

/**
 * Apply color tint based on state
 * Blends tint color with original sprite color
 * tint_strength: 0.0 = no tint, 1.0 = full tint color
 */
static inline uint16_t sprite_state_apply_tint(uint16_t original_color, SpriteAnimationState state, float tint_strength) {
  const SpriteStateMetadata *info = sprite_state_get_info(state);

  if (info->tint_color == 0 || tint_strength <= 0.0f) {
    return original_color;
  }

  // Extract RGB565 components
  uint8_t orig_r = (original_color >> 11) & 0x1F;
  uint8_t orig_g = (original_color >> 5) & 0x3F;
  uint8_t orig_b = original_color & 0x1F;

  uint8_t tint_r = (info->tint_color >> 11) & 0x1F;
  uint8_t tint_g = (info->tint_color >> 5) & 0x3F;
  uint8_t tint_b = info->tint_color & 0x1F;

  // Blend colors
  uint8_t blend_r = (uint8_t)(orig_r * (1.0f - tint_strength) + tint_r * tint_strength);
  uint8_t blend_g = (uint8_t)(orig_g * (1.0f - tint_strength) + tint_g * tint_strength);
  uint8_t blend_b = (uint8_t)(orig_b * (1.0f - tint_strength) + tint_b * tint_strength);

  return ((blend_r & 0x1F) << 11) | ((blend_g & 0x3F) << 5) | (blend_b & 0x1F);
}

/**
 * Get UI indicator for state severity
 * Returns emoji-like indicator (0-10)
 */
static inline int sprite_state_get_severity_indicator(SpriteAnimationState state) {
  return sprite_state_get_info(state)->severity;
}

