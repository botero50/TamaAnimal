/**
 * Tamanimals - Collection Tracking System
 * Tracks which animals have been caught/seen by the player
 * Supports 988 real animals across 11 habitats
 */

#pragma once
#include <stdint.h>
#include <string.h>

// ============================================================================
// COLLECTION DEFINES
// ============================================================================

#define COLLECTION_SIZE 988  // Total animals in Tamanimals
#define COLLECTION_BYTES (COLLECTION_SIZE / 8 + 1)  // Bitmap size in bytes

// ============================================================================
// COLLECTION STATE
// ============================================================================

typedef struct {
  uint8_t caught_bitmap[COLLECTION_BYTES];  // Bitmap of caught animals (1 bit per animal)
  uint16_t total_caught;                    // Count of caught animals
} Collection;

// ============================================================================
// COLLECTION FUNCTIONS
// ============================================================================

/**
 * Initialize collection (empty)
 */
static inline void collection_init(Collection *col) {
  memset(col->caught_bitmap, 0, COLLECTION_BYTES);
  col->total_caught = 0;
}

/**
 * Mark an animal as caught
 */
static inline void collection_catch(Collection *col, uint8_t animal_id) {
  if (animal_id < 1 || animal_id > COLLECTION_SIZE) return;

  uint16_t byte_index = animal_id / 8;
  uint8_t bit_index = animal_id % 8;

  // Check if already caught
  if (!(col->caught_bitmap[byte_index] & (1 << bit_index))) {
    // Mark as caught
    col->caught_bitmap[byte_index] |= (1 << bit_index);
    col->total_caught++;
  }
}

/**
 * Check if an animal is caught
 */
static inline bool collection_is_caught(Collection *col, uint8_t animal_id) {
  if (animal_id < 1 || animal_id > COLLECTION_SIZE) return false;

  uint16_t byte_index = animal_id / 8;
  uint8_t bit_index = animal_id % 8;

  return (col->caught_bitmap[byte_index] & (1 << bit_index)) != 0;
}

/**
 * Get total caught count
 */
static inline uint16_t collection_get_count(Collection *col) {
  return col->total_caught;
}

/**
 * Get catch percentage (0-100)
 */
static inline uint8_t collection_get_percent(Collection *col) {
  if (COLLECTION_SIZE == 0) return 0;
  return (col->total_caught * 100) / COLLECTION_SIZE;
}

