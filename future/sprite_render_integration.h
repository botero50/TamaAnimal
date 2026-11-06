/**
 * Sprite Rendering Integration
 * Integrates sprite animation system with game render loop
 */

#pragma once
#include "sprite_animation_states.h"
#include "sprite_state_transitions.h"
#include "sprite_cache.h"
#include "hd_sprite_loader.h"
#include <stdint.h>
#include <string.h>

// Render context for a single animal
typedef struct {
  uint16_t animal_id;
  AnimalStateContext *state_ctx;
  uint32_t screen_x;
  uint32_t screen_y;
  uint32_t current_time_ms;
  float tint_strength;      // 0.0 = no tint, 1.0 = full tint
} SpriteRenderContext;

/**
 * Calculate which animation frame to display
 * Returns frame index (0 to frame_count-1)
 */
static inline int sprite_render_get_current_frame(
  AnimalStateContext *state_ctx,
  uint32_t current_time_ms
) {
  if (!state_ctx) {
    return 0;
  }

  uint32_t elapsed_ms = current_time_ms - state_ctx->state_start_time_ms;
  return sprite_state_get_frame(state_ctx->current_state, elapsed_ms);
}

/**
 * Get tint color for current state
 * Returns RGB565 color for blending
 */
static inline uint16_t sprite_render_get_tint_color(
  AnimalStateContext *state_ctx
) {
  if (!state_ctx) {
    return 0x0000;  // No tint
  }

  const SpriteStateMetadata *info = sprite_state_get_info(state_ctx->current_state);
  return info->tint_color;
}

/**
 * Apply state tint to a single pixel
 */
static inline uint16_t sprite_render_apply_state_tint(
  uint16_t pixel,
  AnimalStateContext *state_ctx,
  float tint_strength
) {
  if (!state_ctx || tint_strength <= 0.0f) {
    return pixel;
  }

  return sprite_state_apply_tint(pixel, state_ctx->current_state, tint_strength);
}

/**
 * Render sprite with current animation frame and tint
 * Draws directly to framebuffer at given coordinates
 */
static inline void sprite_render_draw(
  SpriteRenderContext *ctx,
  uint16_t *framebuffer,
  uint16_t framebuffer_width,
  uint16_t framebuffer_height
) {
  if (!ctx || !ctx->state_ctx || !framebuffer) {
    return;
  }

  // Get current animation frame
  int frame_idx = sprite_render_get_current_frame(ctx->state_ctx, ctx->current_time_ms);

  // Get sprite data from cache
  uint16_t *sprite_data = get_cached_animal_sprite(
    ctx->animal_id,
    ctx->state_ctx->current_state,
    frame_idx
  );

  if (!sprite_data) {
    return;  // Sprite not available
  }

  // Get sprite dimensions (assume 32x32, could be stored in metadata)
  const uint16_t SPRITE_WIDTH = 32;
  const uint16_t SPRITE_HEIGHT = 32;

  // Draw sprite to framebuffer
  for (uint16_t y = 0; y < SPRITE_HEIGHT; y++) {
    uint16_t fb_y = ctx->screen_y + y;

    // Clip vertical
    if (fb_y >= framebuffer_height) {
      break;
    }

    for (uint16_t x = 0; x < SPRITE_WIDTH; x++) {
      uint16_t fb_x = ctx->screen_x + x;

      // Clip horizontal
      if (fb_x >= framebuffer_width) {
        break;
      }

      // Get pixel from sprite
      uint16_t pixel = sprite_data[y * SPRITE_WIDTH + x];

      // Skip transparent pixels (0x0000)
      if (pixel == 0x0000) {
        continue;
      }

      // Apply state tint
      uint16_t tinted_pixel = sprite_render_apply_state_tint(
        pixel,
        ctx->state_ctx,
        ctx->tint_strength
      );

      // Draw to framebuffer
      uint32_t fb_offset = fb_y * framebuffer_width + fb_x;
      framebuffer[fb_offset] = tinted_pixel;
    }
  }
}

/**
 * Render state indicator (severity bar under sprite)
 */
static inline void sprite_render_draw_severity_indicator(
  SpriteRenderContext *ctx,
  uint16_t *framebuffer,
  uint16_t framebuffer_width,
  uint16_t framebuffer_height,
  uint16_t bar_height
) {
  if (!ctx || !ctx->state_ctx) {
    return;
  }

  const SpriteStateMetadata *info = sprite_state_get_info(ctx->state_ctx->current_state);

  // Draw severity bar below sprite
  uint16_t bar_y = ctx->screen_y + 32 + 2;  // 2 pixel gap below sprite
  uint16_t bar_width = 32;
  uint16_t bar_x = ctx->screen_x;

  // Severity color (0=green, 5=white, 10=red)
  uint16_t bar_color;
  if (info->severity < 5) {
    bar_color = 0x07E0;  // Green (good)
  } else if (info->severity == 5) {
    bar_color = 0xFFFF;  // White (neutral)
  } else {
    bar_color = 0xF800;  // Red (bad)
  }

  // Draw severity bar
  for (uint16_t y = 0; y < bar_height; y++) {
    uint16_t fb_y = bar_y + y;
    if (fb_y >= framebuffer_height) break;

    for (uint16_t x = 0; x < bar_width; x++) {
      uint16_t fb_x = bar_x + x;
      if (fb_x >= framebuffer_width) break;

      uint32_t fb_offset = fb_y * framebuffer_width + fb_x;
      framebuffer[fb_offset] = bar_color;
    }
  }
}

/**
 * Update animal state (called every game tick)
 * Applies state effects, checks for timeouts, etc.
 */
static inline void sprite_render_update_state(
  AnimalStateContext *ctx,
  uint32_t current_time_ms
) {
  if (!ctx) {
    return;
  }

  // Apply continuous state effects
  sprite_transition_apply_effects(ctx, ctx->current_state);

  // Check if state should timeout
  if (sprite_transition_should_update(ctx, current_time_ms)) {
    // Return to normal state
    SpriteAnimationState next = STATE_NORMAL;

    // But check if we should stay in a bad state
    if (ctx->happiness_level < 30) {
      next = STATE_SAD;
    }

    if (ctx->is_sick) {
      next = STATE_SICK;
    }

    if (ctx->is_poisoned) {
      next = STATE_POISONED;
    }

    ctx->current_state = next;
    ctx->state_start_time_ms = current_time_ms;
  }
}

/**
 * Handle trigger event (e.g., user tap)
 * Updates state and applies effects
 */
static inline void sprite_render_trigger_event(
  AnimalStateContext *ctx,
  StateTransitionTrigger trigger,
  uint32_t current_time_ms
) {
  if (!ctx) {
    return;
  }

  // Get next state based on trigger
  SpriteAnimationState next = sprite_transition_get_next_state(ctx, trigger);

  // Only transition if state actually changes
  if (next != ctx->current_state || trigger == TRIGGER_NIGHTTIME) {
    ctx->current_state = next;
    ctx->state_start_time_ms = current_time_ms;

    // Apply state effects
    sprite_transition_apply_effects(ctx, next);
  }
}

/**
 * Get display info for UI (name, emoji, color)
 */
static inline StateDisplayInfo sprite_render_get_display_info(
  AnimalStateContext *ctx
) {
  if (!ctx) {
    return sprite_transition_get_display_info(STATE_NORMAL);
  }

  return sprite_transition_get_display_info(ctx->current_state);
}

/**
 * Draw state indicator emoji above sprite
 */
static inline void sprite_render_draw_state_emoji(
  SpriteRenderContext *ctx,
  uint16_t emoji_x,
  uint16_t emoji_y
) {
  if (!ctx || !ctx->state_ctx) {
    return;
  }

  StateDisplayInfo info = sprite_render_get_display_info(ctx->state_ctx);

  // In real implementation, would draw emoji text
  // For now, just track that it would be drawn
  (void)emoji_x;
  (void)emoji_y;
  (void)info;
}

/**
 * Main render function for animal sprite
 * Called once per frame for each visible animal
 */
static inline void sprite_render_animal(
  uint16_t animal_id,
  AnimalStateContext *state_ctx,
  uint16_t screen_x,
  uint16_t screen_y,
  uint16_t *framebuffer,
  uint16_t framebuffer_width,
  uint16_t framebuffer_height,
  uint32_t current_time_ms
) {
  if (!state_ctx || !framebuffer) {
    return;
  }

  // Create render context
  SpriteRenderContext ctx = {
    .animal_id = animal_id,
    .state_ctx = state_ctx,
    .screen_x = screen_x,
    .screen_y = screen_y,
    .current_time_ms = current_time_ms,
    .tint_strength = 0.25f,  // 25% tint intensity
  };

  // Draw main sprite with animation and tint
  sprite_render_draw(&ctx, framebuffer, framebuffer_width, framebuffer_height);

  // Draw severity indicator bar
  sprite_render_draw_severity_indicator(
    &ctx, framebuffer, framebuffer_width, framebuffer_height, 2
  );

  // Draw state emoji (if UI layer present)
  sprite_render_draw_state_emoji(&ctx, screen_x - 5, screen_y - 10);
}

/**
 * Integration point for main game loop
 * Call this every frame for each animal
 */
static inline void sprite_render_game_loop_update(
  uint16_t animal_id,
  AnimalStateContext *state_ctx,
  uint16_t screen_x,
  uint16_t screen_y,
  uint16_t *framebuffer,
  uint16_t framebuffer_width,
  uint16_t framebuffer_height,
  uint32_t current_time_ms
) {
  // Update animal state (applies effects, checks timeouts)
  sprite_render_update_state(state_ctx, current_time_ms);

  // Render sprite to framebuffer
  sprite_render_animal(
    animal_id, state_ctx, screen_x, screen_y,
    framebuffer, framebuffer_width, framebuffer_height,
    current_time_ms
  );
}

