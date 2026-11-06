/**
 * HD Sprite Registry - 150 Priority Animals
 * Animals with native HD (64x64) sprites at launch
 * Remaining 838 animals use bilinear upscaling fallback
 */

#pragma once
#include <stdint.h>
#include <string.h>

// List of 150 animals with HD sprites at launch
// Prioritized by: popularity, catch rate, educational value, geographic distribution
static const int HD_SPRITE_COUNT = 150;

static const int HD_SPRITE_ANIMALS[150] = {
  // TIER 1: Starter Animals & Most Popular (20 animals)
  // These are the most caught and recognized animals
  1,    // Lion (Africa - savanna icon)
  2,    // African Elephant (Africa - largest land animal)
  3,    // Zebra (Africa - instantly recognizable)
  4,    // Tiger (Asia - most iconic big cat)
  5,    // Giant Panda (Asia - endangered, beloved)
  6,    // Emperor Penguin (Antarctica - instantly recognizable)
  7,    // Polar Bear (Arctic - climate change symbol)
  8,    // Grizzly Bear (North America - powerful predator)
  9,    // Bison (North America - restoration success)
  10,   // Jaguar (South America - rainforest predator)
  11,   // Alpaca (South America - cute, educational)
  12,   // Koala (Australia - iconic marsupial)
  13,   // Kangaroo (Australia - unique hopper)
  14,   // Giraffe (Africa - unique long neck)
  15,   // Cheetah (Africa - fastest land animal)
  16,   // Rhino (Africa - endangered, important)
  17,   // Gorilla (Africa - intelligent primate)
  18,   // Dolphin (Ocean - marine intelligence)
  19,   // Whale (Ocean - largest animal)
  20,   // Eagle (Global - powerful flyer)

  // TIER 2: Educational & Endangered (30 animals)
  // Focus on learning value and conservation status
  21, 22, 23, 24, 25,  // Various primates
  26, 27, 28, 29, 30,  // Big cats
  31, 32, 33, 34, 35,  // Endangered species
  36, 37, 38, 39, 40,  // Ocean creatures
  41, 42, 43, 44, 45,  // Flying creatures
  46, 47, 48, 49, 50,  // Reptiles

  // TIER 3: Habitat Representatives (40 animals)
  // One representative per habitat × 11 habitats × 3 per habitat
  51, 52, 53, 54, 55,  // African Savanna (5 animals)
  56, 57, 58, 59, 60,  // African Rainforest (5 animals)
  61, 62, 63, 64, 65,  // Asian Jungle (5 animals)
  66, 67, 68, 69, 70,  // Temperate Forest (5 animals)
  71, 72, 73, 74, 75,  // Taiga/Boreal (5 animals)
  76, 77, 78, 79, 80,  // Polar Regions (5 animals)
  81, 82, 83, 84, 85,  // Mountain Peaks (5 animals)
  86, 87, 88, 89, 90,  // Desert (5 animals)

  // TIER 4: Regional Heroes (30 animals)
  // Popular animals per continent
  91, 92, 93, 94, 95,   // Africa region heroes
  96, 97, 98, 99, 100,  // Asia region heroes
  101, 102, 103, 104, 105,  // Europe/North America heroes
  106, 107, 108, 109, 110,  // South America region heroes
  111, 112, 113, 114, 115,  // Australia/Oceania heroes
  116, 117, 118, 119, 120,  // Ocean/Marine heroes

  // TIER 5: Seasonal & Special Events (20 animals)
  // Animals featured in special events or seasons
  121, 122, 123, 124, 125,  // Holiday season animals
  126, 127, 128, 129, 130,  // Spring animals
  131, 132, 133, 134, 135,  // Summer animals
  136, 137, 138, 139, 140,  // Fall animals

  // TIER 6: Diverse Educational Mix (10 animals)
  // Fill gaps with educational variety
  141, 142, 143, 144, 145,  // Insects/Small creatures
  146, 147, 148, 149, 150   // Amphibians/Other educational focus
};

/**
 * Fast lookup: Check if animal has HD sprite
 * Uses binary search for O(log n) performance
 * Can be optimized to O(1) with bitfield if needed
 */
static inline bool is_hd_sprite(int animal_id) {
  // Quick bounds check
  if (animal_id < 1 || animal_id > 988) {
    return false;
  }

  // Binary search in sorted array
  int left = 0, right = HD_SPRITE_COUNT - 1;

  while (left <= right) {
    int mid = (left + right) / 2;
    int mid_id = HD_SPRITE_ANIMALS[mid];

    if (mid_id == animal_id) {
      return true;
    } else if (mid_id < animal_id) {
      left = mid + 1;
    } else {
      right = mid - 1;
    }
  }

  return false;
}

/**
 * Get HD sprite index (0-149) if exists, else -1
 * Useful for metadata lookups
 */
static inline int get_hd_sprite_index(int animal_id) {
  int left = 0, right = HD_SPRITE_COUNT - 1;

  while (left <= right) {
    int mid = (left + right) / 2;
    int mid_id = HD_SPRITE_ANIMALS[mid];

    if (mid_id == animal_id) {
      return mid;
    } else if (mid_id < animal_id) {
      left = mid + 1;
    } else {
      right = mid - 1;
    }
  }

  return -1;
}

/**
 * Count total HD sprites (should be 150)
 * Useful for verification
 */
static inline int get_hd_sprite_count() {
  return HD_SPRITE_COUNT;
}

/**
 * Get animal ID at index (0-149)
 * Useful for UI display of HD animals
 */
static inline int get_hd_sprite_id(int index) {
  if (index < 0 || index >= HD_SPRITE_COUNT) {
    return -1;
  }
  return HD_SPRITE_ANIMALS[index];
}

/**
 * Check if enough HD sprites loaded for launch
 * Returns percentage coverage (0-100)
 * Target: 100% for 150 animals
 */
static inline int calculate_hd_coverage(int loaded_count) {
  return (loaded_count * 100) / HD_SPRITE_COUNT;
}

/**
 * Get coverage description
 * Useful for status reporting
 */
static inline const char* get_hd_coverage_status(int loaded_count) {
  int coverage = calculate_hd_coverage(loaded_count);

  if (coverage < 25) return "Starting (< 25%)";
  if (coverage < 50) return "In Progress (25-50%)";
  if (coverage < 75) return "Half Done (50-75%)";
  if (coverage < 90) return "Nearly Complete (75-90%)";
  if (coverage < 100) return "Almost There (90-99%)";
  return "Complete (100%)";
}

/**
 * Return size in bytes for all 150 HD sprites in memory
 * Assumes: 64x64 pixels × 4 frames × 2 bytes/pixel × 150 animals
 * = 4096 × 4 × 2 × 150 = 4.9 MB (if all in memory)
 * With LRU cache (max 12 sprites): ~3 MB max allocated
 */
static inline size_t get_hd_memory_budget() {
  // Per sprite: 64×64×4frames×2bytes = 32KB
  // 12 sprites max in cache = 384 KB
  // Plus overhead: ~450 KB total
  return 450 * 1024;  // 450 KB budget for HD sprites in PSRAM
}

/**
 * Get recommended cache size
 * Balance between memory usage and performance
 */
static inline int get_recommended_cache_size() {
  // Can fit 12-15 full 64x64×4frame sprites in 450KB
  // Recommended: 10-12 for safety
  return 12;
}

