#pragma once

// ============================================================================
// ANIMAL FACTS SYSTEM - Phase 2 Engagement Improvement
// ============================================================================
// Educational content about real animals
// Players learn facts on first catch
// Habitat mastery unlocks bonuses (catch rate, cosmetics)
// Adds intrinsic motivation through learning (competence need)

#include <stdint.h>
#include <string.h>

// Facts per animal (phase 2 starts with 3, expandable to 5+)
#define FACTS_PER_ANIMAL 3

struct AnimalFact {
  const char *title;       // Short title (e.g., "Diet")
  const char *description; // Full fact (e.g., "Snow leopards eat... 15 animals per year")
};

struct AnimalFactProfile {
  uint8_t animal_id;
  const char *name;
  const char *scientific_name;
  AnimalFact facts[FACTS_PER_ANIMAL];
};

// Collection tracking for facts learned
struct FactsLearned {
  bool learned[988];  // One bit per animal (can be optimized to bitmap)
  uint16_t total_facts_learned;
  uint8_t habitats_mastered;  // Bitmask of habitats with all 25 animals learned
};

// ============================================================================
// Example animal facts (would be expanded for all 988 animals)
// ============================================================================

// Note: Real implementation would have entries for all 988 animals
// This is a sampling showing structure

static const AnimalFactProfile ANIMAL_FACTS_SAMPLE[] = {
  {
    1, // Lion
    "Lion",
    "Panthera leo",
    {
      {"Apex Predator", "Lions are one of the few naturally social cats. They live in prides of 4-30 individuals."},
      {"Hunting Strategy", "Lionesses do 90% of the hunting while males defend territory. A pride hunts together to take down large prey."},
      {"Roar", "A lion's roar can be heard up to 5 miles away and serves to communicate with pride members and warn rivals."},
    }
  },
  {
    2, // Zebra
    "Zebra",
    "Equus quagga",
    {
      {"Stripe Pattern", "Each zebra has unique stripe patterns, like human fingerprints. These help zebras identify each other."},
      {"Herd Defense", "Zebras live in family herds and can run up to 40 mph. Their stripes may confuse predators during group movement."},
      {"Teeth", "Zebras' teeth never stop growing. They graze for most of the day to wear down their continuously growing molars."},
    }
  },
  {
    3, // Giraffe
    "Giraffe",
    "Giraffa camelopardalis",
    {
      {"Tallest Land Animal", "Adult giraffes reach 14-18 feet tall. Males are taller and heavier than females."},
      {"Unique Heart", "A giraffe's heart weighs 25 pounds and pumps extra hard to push blood all the way up their long necks."},
      {"Sleeping", "Giraffes only need 30 minutes of sleep per day. They sleep standing up and have guards watch for predators."},
    }
  },
};

// ============================================================================
// Initialize facts system
// ============================================================================
static inline void facts_learned_init(FactsLearned *facts) {
  for (int i = 0; i < 988; i++) {
    facts->learned[i] = false;
  }
  facts->total_facts_learned = 0;
  facts->habitats_mastered = 0;
}

// ============================================================================
// Mark animal facts as learned (called on first catch)
// ============================================================================
static inline bool facts_learned_mark(FactsLearned *facts, uint8_t animal_id) {
  if (animal_id >= 988) return false;
  if (facts->learned[animal_id]) return false;  // Already learned

  facts->learned[animal_id] = true;
  facts->total_facts_learned++;
  return true;  // Newly learned
}

// ============================================================================
// Check if facts learned for animal
// ============================================================================
static inline bool facts_learned_has_facts(const FactsLearned *facts, uint8_t animal_id) {
  if (animal_id >= 988) return false;
  return facts->learned[animal_id];
}

// ============================================================================
// Get total facts learned
// ============================================================================
static inline uint16_t facts_learned_get_total(const FactsLearned *facts) {
  return facts->total_facts_learned;
}

// ============================================================================
// Get facts learned percentage
// ============================================================================
static inline uint8_t facts_learned_get_percent(const FactsLearned *facts) {
  if (988 == 0) return 100;
  return (facts->total_facts_learned * 100) / 988;
}

// ============================================================================
// Get facts learned in habitat (simplified - would need habitat mapping)
// ============================================================================
static inline uint16_t facts_learned_get_habitat_count(const FactsLearned *facts, uint8_t habitat_id) {
  // This would iterate through animals in habitat and count learned
  // Placeholder: returns 0
  return 0;
}

// ============================================================================
// Check if habitat is mastered (all 25 animals learned facts)
// ============================================================================
static inline bool facts_learned_habitat_mastered(const FactsLearned *facts, uint8_t habitat_id) {
  if (habitat_id >= 11) return false;
  return (facts->habitats_mastered & (1 << habitat_id)) != 0;
}

// ============================================================================
// Mark habitat as mastered
// ============================================================================
static inline void facts_learned_master_habitat(FactsLearned *facts, uint8_t habitat_id) {
  if (habitat_id >= 11) return;
  facts->habitats_mastered |= (1 << habitat_id);
}

// ============================================================================
// Get fact for display
// ============================================================================
static inline const AnimalFact *animal_facts_get_fact(uint8_t animal_id, uint8_t fact_index) {
  // In real implementation, this would look up animal_id in full facts database
  // For now, return NULL
  return NULL;
}

// ============================================================================
// Get animal fact profile
// ============================================================================
static inline const AnimalFactProfile *animal_facts_get_profile(uint8_t animal_id) {
  // This would do binary search in ANIMAL_FACTS_SAMPLE or full database
  // Placeholder: return NULL
  return NULL;
}

// ============================================================================
// Get habitat catch rate bonus for mastery
// Each mastered habitat gives +10% catch rate in that habitat
// ============================================================================
static inline uint8_t animal_facts_get_habitat_bonus_percent(uint8_t habitats_mastered, uint8_t habitat_id) {
  if (habitat_id >= 11) return 0;
  bool mastered = (habitats_mastered & (1 << habitat_id)) != 0;
  return mastered ? 10 : 0;
}

// ============================================================================
// Get total catch rate bonus from all mastered habitats
// ============================================================================
static inline uint8_t animal_facts_get_total_bonus_percent(uint8_t habitats_mastered) {
  uint8_t bonus = 0;
  for (int i = 0; i < 11; i++) {
    if (habitats_mastered & (1 << i)) {
      bonus += 5;  // Smaller bonus per habitat
    }
  }
  return bonus > 50 ? 50 : bonus;  // Cap at +50%
}

// ============================================================================
// Get achievement name for habitat mastery
// ============================================================================
static inline const char *animal_facts_get_habitat_achievement(uint8_t habitat_id) {
  switch (habitat_id) {
    case 0:
      return "African Expert";
    case 1:
      return "Asian Scholar";
    case 2:
      return "European Naturalist";
    case 3:
      return "South American Guide";
    case 4:
      return "North American Ranger";
    case 5:
      return "Australian Expert";
    case 6:
      return "Arctic Explorer";
    case 7:
      return "Ocean Master";
    case 8:
      return "Rainforest Sage";
    case 9:
      return "Extinct Historian";
    case 10:
      return "Magic Keeper";
    default:
      return "Unknown";
  }
}

// ============================================================================
// Serialize facts learned (125 bytes for bitmap of 988 animals)
// ============================================================================
static inline void facts_learned_serialize(FactsLearned *facts, uint8_t *buffer) {
  // Each animal = 1 bit, 988 animals = 124 bytes
  memset(buffer, 0, 124);
  for (int i = 0; i < 988; i++) {
    if (facts->learned[i]) {
      int byte_idx = i / 8;
      int bit_idx = i % 8;
      buffer[byte_idx] |= (1 << bit_idx);
    }
  }
  // Store mastered habitats in byte 124
  buffer[124] = facts->habitats_mastered;
  uint16_t *p = (uint16_t *)(buffer + 125);
  p[0] = facts->total_facts_learned;
}

// ============================================================================
// Deserialize facts learned
// ============================================================================
static inline void facts_learned_deserialize(FactsLearned *facts, const uint8_t *buffer) {
  for (int i = 0; i < 988; i++) {
    int byte_idx = i / 8;
    int bit_idx = i % 8;
    facts->learned[i] = (buffer[byte_idx] & (1 << bit_idx)) != 0;
  }
  facts->habitats_mastered = buffer[124];
  const uint16_t *p = (const uint16_t *)(buffer + 125);
  facts->total_facts_learned = p[0];
}
