/**
 * Sprite Loader for Tamanimals
 * Loads TPK2 sprite files for rendering
 * Supports multi-frame animation
 */

#pragma once
#include <stdint.h>
#include <string.h>

// TPK2 Header structure
typedef struct {
  char signature[4];       // "TPK2"
  uint8_t version;         // 1
  uint8_t flags;           // Bit 0: has palette
  uint16_t tile_count;     // Number of animation frames
  uint8_t width;           // Sprite width (32)
  uint8_t height;          // Sprite height (32)
} TPK2Header;

// Sprite in memory
typedef struct {
  uint16_t *pixels;        // RGBA5551 or RGB565 pixel data
  uint16_t *palette;       // 256-color palette if present
  int width;
  int height;
  int frame_count;
  bool has_palette;
} Sprite;

// Animation frame reference
typedef struct {
  int frame_index;         // Which frame (0-3)
  int duration_ms;         // Frame duration in milliseconds
} AnimationFrame;

/**
 * Load a sprite from TPK2 file
 * Returns Sprite structure with pixel data
 * Must be freed with sprite_free()
 */
static inline Sprite* sprite_load_tpk2(const char *filename) {
  // This would read from actual file
  // For now, stub implementation
  Sprite *spr = (Sprite*)malloc(sizeof(Sprite));
  if (spr) {
    spr->width = 32;
    spr->height = 32;
    spr->frame_count = 4;
    spr->has_palette = true;
    spr->pixels = NULL;      // Would allocate from file
    spr->palette = NULL;     // Would load from file
  }
  return spr;
}

/**
 * Get a specific animation frame
 */
static inline const uint16_t* sprite_get_frame(const Sprite *spr, int frame_idx) {
  if (!spr || !spr->pixels || frame_idx < 0 || frame_idx >= spr->frame_count) {
    return NULL;
  }
  int frame_size = spr->width * spr->height;
  return &spr->pixels[frame_idx * frame_size];
}

/**
 * Free sprite memory
 */
static inline void sprite_free(Sprite *spr) {
  if (spr) {
    if (spr->pixels) free(spr->pixels);
    if (spr->palette) free(spr->palette);
    free(spr);
  }
}

/**
 * Get sprite file path from animal ID
 */
static inline void sprite_get_path(char *path, int path_size, int animal_id) {
  snprintf(path, path_size, "sprites/animals/%03d.tpk2", animal_id);
}

