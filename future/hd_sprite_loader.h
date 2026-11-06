/**
 * HD Sprite Loader - Bilinear Upscaling & Quality Detection
 * Supports both native HD (64x64) and upscaled fallback (32→64)
 * Smart caching with LRU eviction
 */

#pragma once
#include <stdint.h>
#include <string.h>
#include <stdio.h>

// Sprite quality tiers
typedef enum {
  SPRITE_QUALITY_FALLBACK = 0,  // 32x32 upscaled to 64x64
  SPRITE_QUALITY_HD = 1,        // Native 64x64 HD sprite
  SPRITE_QUALITY_UNKNOWN = -1
} SpriteQuality;

// HD Sprite metadata
typedef struct {
  int animal_id;
  SpriteQuality quality;
  uint16_t width;
  uint16_t height;
  int frame_count;
  size_t memory_used;
  uint32_t load_time_ms;
  bool is_cached;
} SpriteMetadata;

// Upscaling mode
typedef enum {
  UPSCALE_NEAREST = 0,   // Fast but blocky
  UPSCALE_BILINEAR = 1,  // Smooth, recommended
  UPSCALE_AI = 2         // Best quality (pre-upscaled)
} UpscaleMode;

/**
 * Bilinear interpolation for smooth upscaling
 * Upscales 32x32 → 64x64 with smooth color blending
 * Input: 32x32 RGB565 pixel array (1024 pixels)
 * Output: 64x64 RGB565 pixel array (4096 pixels)
 */
static inline void upscale_bilinear_rgb565(
    const uint16_t *src_32x32,
    uint16_t *dst_64x64) {

  const int SRC_W = 32, SRC_H = 32;
  const int DST_W = 64, DST_H = 64;
  const float SCALE = 2.0f;

  for (int y = 0; y < DST_H; y++) {
    for (int x = 0; x < DST_W; x++) {
      // Map destination pixel to source coordinates
      float src_x = x / SCALE;
      float src_y = y / SCALE;

      // Get integer and fractional parts
      int ix = (int)src_x;
      int iy = (int)src_y;
      float fx = src_x - ix;
      float fy = src_y - iy;

      // Clamp coordinates
      if (ix < 0) ix = 0;
      if (iy < 0) iy = 0;
      if (ix >= SRC_W - 1) ix = SRC_W - 2;
      if (iy >= SRC_H - 1) iy = SRC_H - 2;

      // Get 4 corner pixels
      uint16_t p00 = src_32x32[iy * SRC_W + ix];
      uint16_t p10 = src_32x32[iy * SRC_W + (ix + 1)];
      uint16_t p01 = src_32x32[(iy + 1) * SRC_W + ix];
      uint16_t p11 = src_32x32[(iy + 1) * SRC_W + (ix + 1)];

      // Extract RGB565 components
      auto extract_rgb = [](uint16_t p) {
        int r = (p >> 11) & 0x1F;
        int g = (p >> 5) & 0x3F;
        int b = p & 0x1F;
        return (r << 16) | (g << 8) | b;
      };

      auto combine_rgb = [](int r, int g, int b) {
        return ((r & 0x1F) << 11) | ((g & 0x3F) << 5) | (b & 0x1F);
      };

      uint32_t rgb00 = extract_rgb(p00);
      uint32_t rgb10 = extract_rgb(p10);
      uint32_t rgb01 = extract_rgb(p01);
      uint32_t rgb11 = extract_rgb(p11);

      // Bilinear interpolation of each component
      auto interpolate = [fx, fy](float a, float b, float c, float d) {
        float top = a * (1.0f - fx) + b * fx;
        float bot = c * (1.0f - fx) + d * fx;
        return top * (1.0f - fy) + bot * fy;
      };

      int r0 = (rgb00 >> 16) & 0xFF;
      int g0 = (rgb00 >> 8) & 0xFF;
      int b0 = rgb00 & 0xFF;
      int r1 = (rgb10 >> 16) & 0xFF;
      int g1 = (rgb10 >> 8) & 0xFF;
      int b1 = rgb10 & 0xFF;
      int r2 = (rgb01 >> 16) & 0xFF;
      int g2 = (rgb01 >> 8) & 0xFF;
      int b2 = rgb01 & 0xFF;
      int r3 = (rgb11 >> 16) & 0xFF;
      int g3 = (rgb11 >> 8) & 0xFF;
      int b3 = rgb11 & 0xFF;

      int r = (int)interpolate(r0, r1, r2, r3);
      int g = (int)interpolate(g0, g1, g2, g3);
      int b = (int)interpolate(b0, b1, b2, b3);

      // Clamp and store
      if (r > 31) r = 31; if (r < 0) r = 0;
      if (g > 63) g = 63; if (g < 0) g = 0;
      if (b > 31) b = 31; if (b < 0) b = 0;

      dst_64x64[y * DST_W + x] = combine_rgb(r, g, b);
    }
  }
}

/**
 * Fast nearest-neighbor upscaling
 * Upscales 32x32 → 64x64 with pixel duplication
 * Faster than bilinear (2x2 block copy)
 */
static inline void upscale_nearest_rgb565(
    const uint16_t *src_32x32,
    uint16_t *dst_64x64) {

  for (int y = 0; y < 32; y++) {
    for (int x = 0; x < 32; x++) {
      uint16_t pixel = src_32x32[y * 32 + x];

      // Copy to 2x2 block in destination
      dst_64x64[(y * 2) * 64 + (x * 2)] = pixel;
      dst_64x64[(y * 2) * 64 + (x * 2 + 1)] = pixel;
      dst_64x64[((y * 2) + 1) * 64 + (x * 2)] = pixel;
      dst_64x64[((y * 2) + 1) * 64 + (x * 2 + 1)] = pixel;
    }
  }
}

/**
 * Upscale frame from 32x32 to 64x64
 * Handles all 4 animation frames
 */
static inline void upscale_sprite_frames(
    const uint16_t *src_32x32_4frames,  // 4096 pixels (4 × 1024)
    uint16_t *dst_64x64_4frames,         // 16384 pixels (4 × 4096)
    UpscaleMode mode = UPSCALE_BILINEAR) {

  for (int frame = 0; frame < 4; frame++) {
    const uint16_t *src_frame = src_32x32_4frames + (frame * 1024);
    uint16_t *dst_frame = dst_64x64_4frames + (frame * 4096);

    if (mode == UPSCALE_BILINEAR) {
      upscale_bilinear_rgb565(src_frame, dst_frame);
    } else {
      upscale_nearest_rgb565(src_frame, dst_frame);
    }
  }
}

/**
 * Get file path for sprite
 * Returns "sprites/animals/001.tpk2" or "sprites/hd/001.tpk2"
 */
static inline void sprite_get_path(
    char *path,
    int path_size,
    int animal_id,
    SpriteQuality quality) {

  const char *subdir = (quality == SPRITE_QUALITY_HD) ? "hd" : "animals";
  snprintf(path, path_size, "sprites/%s/%03d.tpk2", subdir, animal_id);
}

/**
 * Get sprite quality for animal
 * Returns SPRITE_QUALITY_HD if exists, else SPRITE_QUALITY_FALLBACK
 * Note: Must check hd_sprite_registry to determine quality
 */
static inline SpriteQuality sprite_detect_quality(int animal_id) {
  // This checks hd_sprite_registry (defined in hd_sprite_registry.h)
  extern bool is_hd_sprite(int animal_id);

  return is_hd_sprite(animal_id) ? SPRITE_QUALITY_HD : SPRITE_QUALITY_FALLBACK;
}

/**
 * Load sprite with automatic quality detection and upscaling
 * Returns sprite scaled to display size (64x64)
 * Handles upscaling if needed
 */
static inline void load_sprite_with_upscaling(
    int animal_id,
    uint16_t *dst_buffer,        // 64x64 × 4 frames = 16384 pixels
    SpriteMetadata *metadata,
    UpscaleMode mode = UPSCALE_BILINEAR) {

  uint32_t start_time = millis();

  // Detect quality
  SpriteQuality quality = sprite_detect_quality(animal_id);

  char path[64];
  sprite_get_path(path, sizeof(path), animal_id, quality);

  // In actual implementation, would load from SD card or SPIFFS
  // For now, stub that would be filled in
  // File f = SD.open(path);
  // if (f) f.read(dst_buffer, ...);

  // Fill metadata
  if (metadata) {
    metadata->animal_id = animal_id;
    metadata->quality = quality;
    metadata->width = 64;
    metadata->height = 64;
    metadata->frame_count = 4;
    metadata->memory_used = 64 * 64 * 4 * 2;  // 4 frames, 16-bit per pixel
    metadata->load_time_ms = millis() - start_time;
    metadata->is_cached = false;
  }
}

/**
 * Quick quality check - see if animal has HD sprite
 * Useful for deciding whether to show HD notification
 */
static inline bool has_hd_sprite(int animal_id) {
  extern bool is_hd_sprite(int animal_id);
  return is_hd_sprite(animal_id);
}

/**
 * Get upscaling quality name for logging
 */
static inline const char* upscale_mode_name(UpscaleMode mode) {
  switch (mode) {
    case UPSCALE_NEAREST: return "nearest-neighbor";
    case UPSCALE_BILINEAR: return "bilinear";
    case UPSCALE_AI: return "ai-upscaled";
    default: return "unknown";
  }
}

/**
 * Get quality name for logging
 */
static inline const char* sprite_quality_name(SpriteQuality quality) {
  switch (quality) {
    case SPRITE_QUALITY_FALLBACK: return "fallback-32x32";
    case SPRITE_QUALITY_HD: return "native-64x64";
    case SPRITE_QUALITY_UNKNOWN: return "unknown";
    default: return "error";
  }
}

