/**
 * Sprite File Loader - Load TPK2 sprites from storage
 * Handles SD card or SPIFFS file operations
 * Integrates with hd_sprite_loader.h for upscaling
 */

#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// File I/O abstraction (works with SD card or SPIFFS)
typedef struct {
  uint16_t *data;
  int size;
  bool loaded;
} FileBuffer;

/**
 * Load raw sprite file from storage
 * Reads TPK2 file and extracts pixel data
 * Returns allocated buffer with sprite data
 */
static inline FileBuffer* load_sprite_file(const char *filepath) {
  FileBuffer *buffer = (FileBuffer*)malloc(sizeof(FileBuffer));
  if (!buffer) return NULL;

  // Try to open file (SD card or SPIFFS)
  // This is a stub - actual implementation depends on storage system
  FILE *f = fopen(filepath, "rb");
  if (!f) {
    buffer->loaded = false;
    buffer->size = 0;
    buffer->data = NULL;
    return buffer;
  }

  // Read TPK2 header (10 bytes)
  uint8_t header[10];
  if (fread(header, 1, 10, f) != 10) {
    fclose(f);
    buffer->loaded = false;
    return buffer;
  }

  // Parse header
  uint16_t tile_count = (header[3] << 8) | header[2];
  uint8_t width = header[8];
  uint8_t height = header[9];

  // Calculate data size (RGB565 = 2 bytes per pixel)
  int frame_size = width * height * 2;
  int total_size = frame_size * tile_count;

  // Allocate and read frame data
  buffer->data = (uint16_t*)malloc(total_size);
  if (!buffer->data) {
    fclose(f);
    buffer->loaded = false;
    return buffer;
  }

  if (fread(buffer->data, 1, total_size, f) != total_size) {
    free(buffer->data);
    fclose(f);
    buffer->loaded = false;
    return buffer;
  }

  fclose(f);
  buffer->size = total_size;
  buffer->loaded = true;
  return buffer;
}

/**
 * Free sprite file buffer
 */
static inline void free_sprite_file(FileBuffer *buffer) {
  if (buffer) {
    if (buffer->data) {
      free(buffer->data);
    }
    free(buffer);
  }
}

/**
 * Load sprite from storage with automatic HD detection & upscaling
 * Returns allocated 64×64×4frame sprite data
 * Caller must free with sprite_free()
 */
static inline uint16_t* load_animal_sprite_from_disk(
    int animal_id,
    bool *is_hd_out) {

  // Check if HD sprite exists
  extern bool is_hd_sprite(int animal_id);
  bool is_hd = is_hd_sprite(animal_id);

  // Load appropriate file
  char filepath[128];
  if (is_hd) {
    snprintf(filepath, sizeof(filepath), "sprites/hd/%03d.tpk2", animal_id);
  } else {
    snprintf(filepath, sizeof(filepath), "sprites/animals/%03d.tpk2", animal_id);
  }

  // Load file
  FileBuffer *file_data = load_sprite_file(filepath);
  if (!file_data || !file_data->loaded) {
    free_sprite_file(file_data);
    if (is_hd_out) *is_hd_out = false;
    return NULL;
  }

  // Allocate output buffer (always 64×64×4frames)
  uint16_t *output = (uint16_t*)malloc(64 * 64 * 4 * 2);
  if (!output) {
    free_sprite_file(file_data);
    return NULL;
  }

  if (is_hd) {
    // HD sprite: directly 64×64, just copy
    memcpy(output, file_data->data, file_data->size);
  } else {
    // Fallback sprite: 32×32, upscale to 64×64
    // Call bilinear upscaling for each frame
    extern void upscale_sprite_frames(
      const uint16_t *src, uint16_t *dst, int mode);

    upscale_sprite_frames(file_data->data, output, 1);  // 1 = bilinear
  }

  free_sprite_file(file_data);

  if (is_hd_out) *is_hd_out = is_hd;
  return output;
}

/**
 * Batch load multiple sprites from disk
 * Useful for preloading at startup
 */
static inline int load_multiple_sprites(
    int *animal_ids,
    int count,
    uint16_t **output_buffers) {

  int loaded = 0;
  for (int i = 0; i < count; i++) {
    bool is_hd = false;
    uint16_t *sprite = load_animal_sprite_from_disk(animal_ids[i], &is_hd);

    if (sprite) {
      output_buffers[i] = sprite;
      loaded++;
    } else {
      output_buffers[i] = NULL;
    }
  }

  return loaded;
}

