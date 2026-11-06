/**
 * Sprite Cache System - LRU Cache with Auto-Eviction
 * Manages up to 12 sprites in memory with least-recently-used eviction
 * Tracks memory usage and load times
 */

#pragma once
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define SPRITE_CACHE_SIZE 12     // Max 12 sprites in memory
#define SPRITE_FRAME_SIZE (64 * 64 * 2)  // 64x64 pixels × 2 bytes (RGB565)
#define SPRITE_TOTAL_SIZE (SPRITE_FRAME_SIZE * 4)  // 4 animation frames

// Cache entry
typedef struct {
  int animal_id;                    // Which animal this is (-1 = empty)
  uint16_t *pixel_data;             // Pointer to pixel buffer (allocated)
  uint32_t last_accessed_ms;        // When this was last used (for LRU)
  uint32_t load_time_ms;            // Time to load this sprite
  bool is_hd;                       // Whether this is HD or upscaled
  size_t memory_bytes;              // Bytes allocated for this sprite
} CacheEntry;

// Cache statistics
typedef struct {
  int total_loads;
  int cache_hits;
  int cache_misses;
  int evictions;
  size_t total_memory_used;
  uint32_t avg_load_time_ms;
} CacheStats;

// Global cache state
static CacheEntry sprite_cache[SPRITE_CACHE_SIZE];
static CacheStats cache_stats = {0};
static int cache_initialized = 0;

/**
 * Initialize cache (call once on startup)
 */
static inline void cache_init() {
  if (cache_initialized) return;

  for (int i = 0; i < SPRITE_CACHE_SIZE; i++) {
    sprite_cache[i].animal_id = -1;
    sprite_cache[i].pixel_data = NULL;
    sprite_cache[i].last_accessed_ms = 0;
    sprite_cache[i].load_time_ms = 0;
    sprite_cache[i].is_hd = false;
    sprite_cache[i].memory_bytes = 0;
  }

  memset(&cache_stats, 0, sizeof(CacheStats));
  cache_initialized = 1;
}

/**
 * Find cache entry by animal ID
 * Returns index (0-11) or -1 if not found
 */
static inline int cache_find(int animal_id) {
  for (int i = 0; i < SPRITE_CACHE_SIZE; i++) {
    if (sprite_cache[i].animal_id == animal_id) {
      return i;
    }
  }
  return -1;
}

/**
 * Find empty cache slot
 * Returns index (0-11) or -1 if cache full
 */
static inline int cache_find_empty() {
  for (int i = 0; i < SPRITE_CACHE_SIZE; i++) {
    if (sprite_cache[i].animal_id == -1) {
      return i;
    }
  }
  return -1;
}

/**
 * Find least-recently-used (oldest) cache entry
 * Used for eviction when cache is full
 * Returns index (0-11)
 */
static inline int cache_find_lru() {
  int lru_idx = 0;
  uint32_t oldest_time = sprite_cache[0].last_accessed_ms;

  for (int i = 1; i < SPRITE_CACHE_SIZE; i++) {
    if (sprite_cache[i].last_accessed_ms < oldest_time) {
      oldest_time = sprite_cache[i].last_accessed_ms;
      lru_idx = i;
    }
  }

  return lru_idx;
}

/**
 * Evict entry from cache (free memory)
 * Returns true if successful
 */
static inline bool cache_evict(int index) {
  if (index < 0 || index >= SPRITE_CACHE_SIZE) {
    return false;
  }

  CacheEntry *entry = &sprite_cache[index];

  if (entry->pixel_data) {
    // Update stats
    cache_stats.total_memory_used -= entry->memory_bytes;
    cache_stats.evictions++;

    // Free memory
    free(entry->pixel_data);
    entry->pixel_data = NULL;
  }

  // Mark as empty
  entry->animal_id = -1;
  entry->last_accessed_ms = 0;
  entry->memory_bytes = 0;

  return true;
}

/**
 * Add sprite to cache
 * Allocates memory, adds to cache, evicts LRU if needed
 * Returns cache index or -1 if error
 */
static inline int cache_add(
    int animal_id,
    const uint16_t *pixel_data,
    bool is_hd,
    uint32_t load_time_ms) {

  // Check if already in cache
  int existing = cache_find(animal_id);
  if (existing >= 0) {
    // Already cached, just update access time
    sprite_cache[existing].last_accessed_ms = millis();
    cache_stats.cache_hits++;
    return existing;
  }

  // Try to find empty slot
  int slot = cache_find_empty();

  // If cache full, evict LRU
  if (slot < 0) {
    slot = cache_find_lru();
    cache_evict(slot);
  }

  // Allocate memory for sprite
  CacheEntry *entry = &sprite_cache[slot];
  entry->pixel_data = (uint16_t*)malloc(SPRITE_TOTAL_SIZE);

  if (!entry->pixel_data) {
    // Allocation failed - try to evict one more and retry
    cache_evict(cache_find_lru());
    entry->pixel_data = (uint16_t*)malloc(SPRITE_TOTAL_SIZE);
    if (!entry->pixel_data) {
      return -1;  // Still failed
    }
  }

  // Copy pixel data
  memcpy(entry->pixel_data, pixel_data, SPRITE_TOTAL_SIZE);

  // Fill entry
  entry->animal_id = animal_id;
  entry->last_accessed_ms = millis();
  entry->load_time_ms = load_time_ms;
  entry->is_hd = is_hd;
  entry->memory_bytes = SPRITE_TOTAL_SIZE;

  // Update stats
  cache_stats.total_memory_used += SPRITE_TOTAL_SIZE;
  cache_stats.total_loads++;
  cache_stats.cache_misses++;

  return slot;
}

/**
 * Get sprite from cache by animal ID
 * Updates last_accessed_ms for LRU tracking
 * Returns pixel data or NULL if not cached
 */
static inline const uint16_t* cache_get(int animal_id) {
  int idx = cache_find(animal_id);

  if (idx < 0) {
    return NULL;  // Not in cache
  }

  // Update access time for LRU
  sprite_cache[idx].last_accessed_ms = millis();
  cache_stats.cache_hits++;

  return sprite_cache[idx].pixel_data;
}

/**
 * Get specific animation frame from cached sprite
 * Frame 0-3 for 4-frame animation
 * Returns pixel data for that frame
 */
static inline const uint16_t* cache_get_frame(int animal_id, int frame) {
  if (frame < 0 || frame > 3) {
    return NULL;
  }

  const uint16_t *sprite_data = cache_get(animal_id);
  if (!sprite_data) {
    return NULL;
  }

  return &sprite_data[frame * SPRITE_FRAME_SIZE / 2];
}

/**
 * Check if sprite is in cache
 */
static inline bool cache_has(int animal_id) {
  return cache_find(animal_id) >= 0;
}

/**
 * Check if sprite is HD quality (vs upscaled fallback)
 */
static inline bool cache_is_hd(int animal_id) {
  int idx = cache_find(animal_id);
  if (idx < 0) return false;
  return sprite_cache[idx].is_hd;
}

/**
 * Get memory usage of cached sprite
 */
static inline size_t cache_get_sprite_memory(int animal_id) {
  int idx = cache_find(animal_id);
  if (idx < 0) return 0;
  return sprite_cache[idx].memory_bytes;
}

/**
 * Clear entire cache (free all memory)
 */
static inline void cache_clear() {
  for (int i = 0; i < SPRITE_CACHE_SIZE; i++) {
    cache_evict(i);
  }
  memset(&cache_stats, 0, sizeof(CacheStats));
}

/**
 * Get cache statistics
 */
static inline const CacheStats* cache_get_stats() {
  return &cache_stats;
}

/**
 * Get cache hit rate (0-100%)
 */
static inline int cache_get_hit_rate() {
  if (cache_stats.total_loads == 0) {
    return 0;
  }
  return (cache_stats.cache_hits * 100) / cache_stats.total_loads;
}

/**
 * Get memory usage string for logging
 * Examples: "384 KB", "256 KB", etc.
 */
static inline void cache_get_memory_string(char *buffer, int size) {
  int kb = cache_stats.total_memory_used / 1024;
  snprintf(buffer, size, "%d KB", kb);
}

/**
 * Print cache statistics (for debugging)
 */
static inline void cache_print_stats() {
  printf("=== Sprite Cache Statistics ===\n");
  printf("Total Loads: %d\n", cache_stats.total_loads);
  printf("Cache Hits: %d\n", cache_stats.cache_hits);
  printf("Cache Misses: %d\n", cache_stats.cache_misses);
  printf("Hit Rate: %d%%\n", cache_get_hit_rate());
  printf("Evictions: %d\n", cache_stats.evictions);
  printf("Memory Used: %d KB\n", cache_stats.total_memory_used / 1024);
  printf("Cached Sprites: %d/%d\n", cache_find_empty() < 0 ? SPRITE_CACHE_SIZE : (SPRITE_CACHE_SIZE - 1), SPRITE_CACHE_SIZE);
  printf("===============================\n");
}

/**
 * Get number of sprites currently cached
 */
static inline int cache_get_count() {
  int count = 0;
  for (int i = 0; i < SPRITE_CACHE_SIZE; i++) {
    if (sprite_cache[i].animal_id != -1) {
      count++;
    }
  }
  return count;
}

/**
 * Get average load time for cached sprites
 */
static inline uint32_t cache_get_avg_load_time() {
  if (cache_stats.total_loads == 0) {
    return 0;
  }

  uint32_t total_time = 0;
  int count = 0;

  for (int i = 0; i < SPRITE_CACHE_SIZE; i++) {
    if (sprite_cache[i].animal_id != -1) {
      total_time += sprite_cache[i].load_time_ms;
      count++;
    }
  }

  return count > 0 ? total_time / count : 0;
}

/**
 * Pre-load critical sprites at startup
 * Loads starter animals and popular animals
 * This improves perceived performance
 */
static inline void cache_preload_critical(
    int *animal_ids,
    int count,
    const uint16_t **pixel_arrays) {

  for (int i = 0; i < count; i++) {
    if (i >= SPRITE_CACHE_SIZE) break;

    cache_add(
      animal_ids[i],
      pixel_arrays[i],
      false,  // Assume fallback for now
      50      // Estimate 50ms load time
    );
  }
}

