/**
 * Sprite State Transition System
 * Manages when and how animals change emotional states based on game events
 */

#pragma once
#include "sprite_animation_states.h"
#include <stdint.h>
#include <stdbool.h>

// State transition triggers
typedef enum {
  TRIGGER_USER_INTERACTION,    // User taps/pats
  TRIGGER_FEED,                // User feeds
  TRIGGER_PLAY,                // User plays minigame
  TRIGGER_EVOLUTION,           // Pet evolves
  TRIGGER_NEGLECT,             // No interaction for 30+ min
  TRIGGER_HUNGER,              // Hunger level critical
  TRIGGER_SICK_EXPOSURE,       // Exposed to illness
  TRIGGER_POISONED_EXPOSURE,   // Exposed to poison
  TRIGGER_NIGHTTIME,           // 22:00 - 06:00
  TRIGGER_RARE_ENCOUNTER,      // Found rare animal
  TRIGGER_MINIGAME_WIN,        // Won minigame
  TRIGGER_MINIGAME_LOSS,       // Lost minigame
  TRIGGER_BATTLE_WIN,          // Won battle
  TRIGGER_BATTLE_LOSS,         // Lost battle
  TRIGGER_COLD_EXPOSURE,       // Cold environment
  TRIGGER_HEAT_EXPOSURE,       // Hot environment
  TRIGGER_EVOLUTION_MOMENT,    // Evolving to next stage
  TRIGGER_ACHIEVEMENT,         // Unlocked achievement
} StateTransitionTrigger;

// Animal state tracking
typedef struct {
  SpriteAnimationState current_state;
  uint32_t state_start_time_ms;      // When state started
  uint32_t last_interaction_ms;      // Last user action
  uint32_t happiness_level;          // 0-100
  uint32_t hunger_level;             // 0-100 (100 = starving)
  uint32_t health_level;             // 0-100 (100 = healthy)
  bool is_sick;
  bool is_poisoned;
  uint8_t stage;                     // Evolution stage 1-6
} AnimalStateContext;

/**
 * Determine next state based on trigger and current conditions
 */
static inline SpriteAnimationState sprite_transition_get_next_state(
  AnimalStateContext *ctx,
  StateTransitionTrigger trigger
) {
  // Check persistent negative states first
  if (ctx->is_poisoned) {
    return STATE_POISONED;  // Poisoned takes priority
  }

  if (ctx->is_sick && trigger != TRIGGER_MINIGAME_WIN && trigger != TRIGGER_EVOLUTION) {
    return STATE_SICK;  // Stay sick unless positive event
  }

  // Sleeping during night?
  if (trigger == TRIGGER_NIGHTTIME) {
    return STATE_SLEEPING;
  }

  // Map triggers to states
  switch (trigger) {
    case TRIGGER_USER_INTERACTION:
    case TRIGGER_FEED:
    case TRIGGER_PLAY:
      return STATE_HAPPY;  // Direct interaction = happy

    case TRIGGER_EVOLUTION:
    case TRIGGER_EVOLUTION_MOMENT:
    case TRIGGER_MINIGAME_WIN:
    case TRIGGER_BATTLE_WIN:
    case TRIGGER_ACHIEVEMENT:
    case TRIGGER_RARE_ENCOUNTER:
      return STATE_EXCITED;  // Major positive event

    case TRIGGER_NEGLECT:
      return STATE_SAD;  // Lonely from neglect

    case TRIGGER_HUNGER:
      return STATE_SAD;  // Hungry and sad

    case TRIGGER_SICK_EXPOSURE:
      ctx->is_sick = true;
      return STATE_SICK;

    case TRIGGER_POISONED_EXPOSURE:
      ctx->is_poisoned = true;
      return STATE_POISONED;

    case TRIGGER_MINIGAME_LOSS:
    case TRIGGER_BATTLE_LOSS:
      return STATE_SCARED;  // Lost = scared

    case TRIGGER_COLD_EXPOSURE:
      return STATE_FROZEN;

    case TRIGGER_HEAT_EXPOSURE:
      return STATE_BURNING;

    case TRIGGER_NIGHTTIME:
      return STATE_SLEEPING;

    default:
      return STATE_NORMAL;
  }
}

/**
 * Calculate how long state should last (milliseconds)
 * Some states last until trigger is resolved, others timeout
 */
static inline uint32_t sprite_transition_get_duration(
  SpriteAnimationState state,
  AnimalStateContext *ctx
) {
  switch (state) {
    case STATE_NORMAL:
      return UINT32_MAX;  // Indefinite until next trigger

    case STATE_HAPPY:
      return 10000;  // 10 seconds

    case STATE_EXCITED:
      return 15000;  // 15 seconds (celebratory)

    case STATE_SAD:
      return 30000;  // 30 seconds (until distracted)

    case STATE_SICK:
      // Until cured
      return ctx->is_sick ? UINT32_MAX : 5000;

    case STATE_POISONED:
      // Until antidote given
      return ctx->is_poisoned ? UINT32_MAX : 5000;

    case STATE_SLEEPING:
      // Until morning or wake event
      return 8000;  // 8 seconds per sleep segment

    case STATE_SCARED:
      return 5000;  // 5 seconds (quick recovery)

    case STATE_ANGRY:
      return 8000;  // 8 seconds

    case STATE_CONFUSED:
      return 6000;  // 6 seconds

    case STATE_FROZEN:
      return 7000;  // 7 seconds

    case STATE_BURNING:
      return 10000;  // 10 seconds

    default:
      return 5000;
  }
}

/**
 * Check if state should automatically transition
 */
static inline bool sprite_transition_should_update(
  AnimalStateContext *ctx,
  uint32_t current_time_ms
) {
  uint32_t elapsed = current_time_ms - ctx->state_start_time_ms;
  uint32_t duration = sprite_transition_get_duration(ctx->current_state, ctx);

  if (elapsed >= duration) {
    // Timeout - return to normal
    if (ctx->happiness_level > 70) {
      return true;  // Can transition
    }
    return false;  // Stay in sad state
  }

  return false;  // State still active
}

/**
 * Apply state side effects (heal, damage, etc.)
 */
static inline void sprite_transition_apply_effects(
  AnimalStateContext *ctx,
  SpriteAnimationState state
) {
  switch (state) {
    case STATE_HAPPY:
      ctx->happiness_level = 100;
      break;

    case STATE_EXCITED:
      ctx->happiness_level = 100;
      ctx->health_level += 10;
      break;

    case STATE_SAD:
      ctx->happiness_level = max(0, ctx->happiness_level - 5);
      break;

    case STATE_SICK:
      ctx->health_level -= 2;  // Slowly get worse
      if (ctx->health_level < 50) {
        ctx->is_sick = true;
      }
      break;

    case STATE_POISONED:
      ctx->health_level -= 5;  // Rapid damage
      break;

    case STATE_SLEEPING:
      ctx->happiness_level += 10;  // Rest = happy
      ctx->health_level += 5;      // Rest = healthier
      ctx->hunger_level += 3;       // Get hungry during sleep
      break;

    case STATE_SCARED:
      ctx->happiness_level = max(0, ctx->happiness_level - 10);
      break;

    case STATE_ANGRY:
      ctx->happiness_level = max(0, ctx->happiness_level - 15);
      break;

    case STATE_FROZEN:
      ctx->health_level -= 3;  // Slow damage
      break;

    case STATE_BURNING:
      ctx->health_level -= 4;  // Moderate damage
      break;

    default:
      break;
  }

  // Clamp values
  ctx->happiness_level = min(100, ctx->happiness_level);
  ctx->hunger_level = min(100, ctx->hunger_level);
  ctx->health_level = min(100, ctx->health_level);
}

/**
 * Get UI severity color for state
 */
static inline uint16_t sprite_transition_get_severity_color(
  SpriteAnimationState state
) {
  const SpriteStateMetadata *info = sprite_state_get_info(state);

  if (info->severity < 5) {
    return 0x07E0;  // Green (good)
  } else if (info->severity == 5) {
    return 0xFFFF;  // White (neutral)
  } else {
    return 0xF800;  // Red (bad)
  }
}

/**
 * Get icon emoji for state
 */
static inline const char* sprite_transition_get_state_emoji(
  SpriteAnimationState state
) {
  switch (state) {
    case STATE_NORMAL:
      return "😐";
    case STATE_HAPPY:
      return "😊";
    case STATE_SAD:
      return "😢";
    case STATE_SICK:
      return "🤒";
    case STATE_POISONED:
      return "☠️";
    case STATE_SLEEPING:
      return "😴";
    case STATE_EXCITED:
      return "🤩";
    case STATE_SCARED:
      return "😨";
    case STATE_ANGRY:
      return "😠";
    case STATE_CONFUSED:
      return "🤔";
    case STATE_FROZEN:
      return "🥶";
    case STATE_BURNING:
      return "🔥";
    default:
      return "❓";
  }
}

/**
 * Format state info for UI display
 */
typedef struct {
  const char *state_name;
  const char *state_emoji;
  const char *description;
  uint16_t ui_color;
  uint8_t severity;
} StateDisplayInfo;

static inline StateDisplayInfo sprite_transition_get_display_info(
  SpriteAnimationState state
) {
  const SpriteStateMetadata *info = sprite_state_get_info(state);

  return (StateDisplayInfo){
    .state_name = info->name,
    .state_emoji = sprite_transition_get_state_emoji(state),
    .description = info->description,
    .ui_color = sprite_transition_get_severity_color(state),
    .severity = info->severity,
  };
}

