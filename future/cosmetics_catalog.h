#pragma once

// ============================================================================
// COSMETICS CATALOG - Battle Pass & Premium Content
// ============================================================================
// 100+ cosmetics across 4 categories
// All cosmetics are cosmetic-only (no gameplay advantage)
// Available through battle pass ($3.99/season) or earned through gameplay

#include <stdint.h>
#include <string.h>

#define COSMETICS_TOTAL 120

enum CosmeticType {
  COSMETIC_PET_SKIN = 0,        // Pet appearance variants
  COSMETIC_TRAINER_OUTFIT = 1,  // Trainer outfit
  COSMETIC_HABITAT_THEME = 2,   // Game UI theme
  COSMETIC_EFFECT = 3,          // Particle/animation effects
};

enum BattlePassSeason {
  SEASON_1 = 1,   // Spring
  SEASON_2 = 2,   // Summer
  SEASON_3 = 3,   // Fall
  SEASON_4 = 4,   // Winter
};

struct Cosmetic {
  uint8_t id;
  const char *name;
  const char *description;
  uint8_t type;              // COSMETIC_PET_SKIN, etc.
  uint8_t season;            // Season introduced
  uint8_t rarity;            // 0=common, 1=rare, 2=epic, 3=legendary
  uint16_t battle_pass_tier; // Which BP tier (1-100)
  bool earnable_free;        // Can be earned without paying?
  uint16_t color;            // RGB565 preview color
};

// ============================================================================
// PET SKINS (30) - Different animal appearances
// ============================================================================

static const Cosmetic PET_SKINS[] = {
  // Seasonal Variants (Spring - Season 1)
  {0, "Spring Blossom", "Cherry blossom themed pet (pink with floral patterns)",
   COSMETIC_PET_SKIN, SEASON_1, 1, 5, true, 0xF81F},
  {1, "Spring Butterfly", "Butterfly wings on pet's back",
   COSMETIC_PET_SKIN, SEASON_1, 1, 15, false, 0xFF80},
  {2, "Spring Garden", "Surrounded by tiny flowers",
   COSMETIC_PET_SKIN, SEASON_1, 2, 25, false, 0x07E0},

  // Summer Variants (Season 2)
  {3, "Summer Beach", "Tropical beach outfit (sunglasses, flip-flops)",
   COSMETIC_PET_SKIN, SEASON_2, 1, 5, true, 0xFBC0},
  {4, "Summer Surfer", "Surfboard and wave effects",
   COSMETIC_PET_SKIN, SEASON_2, 2, 35, false, 0x0FFF},
  {5, "Summer Ice Cream", "Ice cream cone costume",
   COSMETIC_PET_SKIN, SEASON_2, 1, 12, false, 0xF8A0},
  {6, "Watermelon Pet", "Watermelon colored with seed pattern",
   COSMETIC_PET_SKIN, SEASON_2, 2, 50, false, 0x0600},

  // Fall Variants (Season 3)
  {7, "Fall Harvest", "Leaves and pumpkin colors",
   COSMETIC_PET_SKIN, SEASON_3, 1, 5, true, 0xFBE0},
  {8, "Fall Spooky", "Halloween ghost costume",
   COSMETIC_PET_SKIN, SEASON_3, 1, 30, false, 0xFFFF},
  {9, "Fall Pumpkin", "Full pumpkin costume",
   COSMETIC_PET_SKIN, SEASON_3, 2, 60, false, 0xFC80},

  // Winter Variants (Season 4)
  {10, "Winter Snow", "Icy blue with snowflake patterns",
   COSMETIC_PET_SKIN, SEASON_4, 1, 5, true, 0xF0FF},
  {11, "Winter Snowman", "Snowman costume",
   COSMETIC_PET_SKIN, SEASON_4, 1, 20, false, 0xFFFF},
  {12, "Winter Santa", "Santa outfit (red suit, hat)",
   COSMETIC_PET_SKIN, SEASON_4, 2, 75, false, 0xF800},
  {13, "Reindeer", "Reindeer horns and nose",
   COSMETIC_PET_SKIN, SEASON_4, 2, 85, false, 0x8410},

  // Legendary Skins
  {14, "Crystal Pet", "Shimmering crystal appearance",
   COSMETIC_PET_SKIN, SEASON_1, 3, 100, false, 0x0FFF},
  {15, "Golden Pet", "Golden armor costume",
   COSMETIC_PET_SKIN, SEASON_2, 3, 100, false, 0xFFC0},
  {16, "Shadow Pet", "Dark purple with glowing eyes",
   COSMETIC_PET_SKIN, SEASON_3, 3, 100, false, 0x8010},
  {17, "Dragon Pet", "Dragon wings and horns",
   COSMETIC_PET_SKIN, SEASON_4, 3, 100, false, 0xF800},

  // Mythical Skins
  {18, "Phoenix", "Flaming red with fire effect",
   COSMETIC_PET_SKIN, SEASON_2, 3, 90, false, 0xF900},
  {19, "Unicorn", "Pink with horn and sparkles",
   COSMETIC_PET_SKIN, SEASON_1, 2, 45, false, 0xFB1F},
  {20, "Angel Pet", "White with wings and halo",
   COSMETIC_PET_SKIN, SEASON_4, 2, 50, false, 0xFFFF},
  {21, "Devil Pet", "Red with horns and tail",
   COSMETIC_PET_SKIN, SEASON_3, 2, 55, false, 0xF800},

  // Special Skins
  {22, "Void Pet", "Transparent with cosmic effect",
   COSMETIC_PET_SKIN, SEASON_1, 3, 95, false, 0x0010},
  {23, "Nature Pet", "Green with plant growth",
   COSMETIC_PET_SKIN, SEASON_2, 2, 40, false, 0x0400},
  {24, "Cyber Pet", "Neon purple with digital effect",
   COSMETIC_PET_SKIN, SEASON_3, 2, 65, false, 0xF01F},
  {25, "Retro Pet", "8-bit pixel art style",
   COSMETIC_PET_SKIN, SEASON_4, 1, 10, false, 0xFFE0},

  // Earned Skins (no payment needed)
  {26, "Birthday Pet", "Party hat and confetti (free on birthday)",
   COSMETIC_PET_SKIN, SEASON_1, 1, 0, true, 0xF81F},
  {27, "Anniversary Pet", "Special marking (1 year anniversary)",
   COSMETIC_PET_SKIN, SEASON_2, 2, 0, true, 0xFFE0},
  {28, "Collection Master", "Shiny appearance (catch all 988 animals)",
   COSMETIC_PET_SKIN, SEASON_1, 3, 0, true, 0xFFC0},
};

// ============================================================================
// TRAINER OUTFITS (25) - Appearance of player avatar
// ============================================================================

static const Cosmetic TRAINER_OUTFITS[] = {
  // Seasonal Outfits
  {30, "Spring Garden", "Pastel colors with flower accessories",
   COSMETIC_TRAINER_OUTFIT, SEASON_1, 1, 8, true, 0xFDB0},
  {31, "Spring Breeze", "Light jacket with nature theme",
   COSMETIC_TRAINER_OUTFIT, SEASON_1, 2, 42, false, 0x07E0},

  {32, "Summer Explorer", "Tropical shirt and khaki shorts",
   COSMETIC_TRAINER_OUTFIT, SEASON_2, 1, 7, true, 0xFBC0},
  {33, "Summer Surfer", "Wetsuit and board shorts",
   COSMETIC_TRAINER_OUTFIT, SEASON_2, 2, 48, false, 0x001F},
  {34, "Beach Party", "Colorful casual beach wear",
   COSMETIC_TRAINER_OUTFIT, SEASON_2, 1, 15, false, 0xFF80},

  {35, "Fall Ranger", "Brown leather with autumn colors",
   COSMETIC_TRAINER_OUTFIT, SEASON_3, 1, 6, true, 0xA2A0},
  {36, "Fall Cozy", "Sweater and scarf outfit",
   COSMETIC_TRAINER_OUTFIT, SEASON_3, 1, 28, false, 0xE260},
  {37, "Halloween Costume", "Spooky costume options",
   COSMETIC_TRAINER_OUTFIT, SEASON_3, 2, 70, false, 0x001F},

  {38, "Winter Coat", "Warm winter jacket and boots",
   COSMETIC_TRAINER_OUTFIT, SEASON_4, 1, 5, true, 0xF81F},
  {39, "Winter Formal", "Elegant winter dress/tuxedo",
   COSMETIC_TRAINER_OUTFIT, SEASON_4, 2, 80, false, 0xFFFF},
  {40, "Santa Suit", "Red suit with white trim",
   COSMETIC_TRAINER_OUTFIT, SEASON_4, 2, 92, false, 0xF800},

  // Professional Outfits
  {41, "Scientist", "Lab coat and goggles",
   COSMETIC_TRAINER_OUTFIT, SEASON_1, 2, 55, false, 0xFFFF},
  {42, "Ranger", "Official wildlife ranger uniform",
   COSMETIC_TRAINER_OUTFIT, SEASON_2, 2, 60, false, 0x0600},
  {43, "Veterinarian", "Doctor's coat with stethoscope",
   COSMETIC_TRAINER_OUTFIT, SEASON_1, 2, 65, false, 0xFFFF},
  {44, "Zookeeper", "Khaki uniform with tools",
   COSMETIC_TRAINER_OUTFIT, SEASON_2, 2, 70, false, 0xE260},

  // Fantasy Outfits
  {45, "Knight", "Full plate armor",
   COSMETIC_TRAINER_OUTFIT, SEASON_3, 2, 75, false, 0x8410},
  {46, "Wizard", "Robes and pointed hat",
   COSMETIC_TRAINER_OUTFIT, SEASON_3, 2, 80, false, 0x7809},
  {47, "Pirate", "Captain's outfit with skull emblem",
   COSMETIC_TRAINER_OUTFIT, SEASON_2, 2, 85, false, 0x2800},
  {48, "Space Suit", "Futuristic space explorer gear",
   COSMETIC_TRAINER_OUTFIT, SEASON_3, 3, 95, false, 0x001F},

  // Special Outfits
  {49, "Athletic", "Sports gear and sneakers",
   COSMETIC_TRAINER_OUTFIT, SEASON_1, 1, 12, false, 0xFBC0},
  {50, "Casual", "Everyday comfortable clothing",
   COSMETIC_TRAINER_OUTFIT, SEASON_1, 0, 1, true, 0xE260},
  {51, "Formal", "Business suit or elegant dress",
   COSMETIC_TRAINER_OUTFIT, SEASON_1, 2, 50, false, 0x1082},
  {52, "Rainbow", "Colorful pride-themed outfit",
   COSMETIC_TRAINER_OUTFIT, SEASON_2, 2, 88, false, 0xFF80},
  {53, "Retro", "80s/90s style outfit",
   COSMETIC_TRAINER_OUTFIT, SEASON_4, 1, 18, false, 0xF81F},
  {54, "Royal", "Crown and elegant robes",
   COSMETIC_TRAINER_OUTFIT, SEASON_1, 3, 100, false, 0xFFC0},
};

// ============================================================================
// HABITAT THEMES (25) - Game UI appearance
// ============================================================================

static const Cosmetic HABITAT_THEMES[] = {
  // Seasonal Themes
  {55, "Spring Garden", "Pastel colors, blooming flowers, soft music",
   COSMETIC_HABITAT_THEME, SEASON_1, 1, 10, true, 0xF81F},
  {56, "Spring Cherry Blossom", "Pink trees, traditional Japanese feel",
   COSMETIC_HABITAT_THEME, SEASON_1, 2, 33, false, 0xFB1F},

  {57, "Summer Beach", "Ocean waves, tropical colors, beach sounds",
   COSMETIC_HABITAT_THEME, SEASON_2, 1, 9, true, 0x0FFF},
  {58, "Summer Neon", "Cyberpunk neon aesthetic, electronic music",
   COSMETIC_HABITAT_THEME, SEASON_2, 2, 58, false, 0xF01F},
  {59, "Summer Safari", "Desert animals, sandy colors, wildlife sounds",
   COSMETIC_HABITAT_THEME, SEASON_2, 1, 22, false, 0xFBC0},

  {60, "Fall Forest", "Autumn leaves, cozy cabin vibes",
   COSMETIC_HABITAT_THEME, SEASON_3, 1, 11, true, 0xFBE0},
  {61, "Fall Spooky", "Halloween theme with pumpkins and ghosts",
   COSMETIC_HABITAT_THEME, SEASON_3, 2, 72, false, 0x001F},
  {62, "Fall Harvest", "Thanksgiving colors, harvest festival",
   COSMETIC_HABITAT_THEME, SEASON_3, 1, 40, false, 0xFC80},

  {63, "Winter Snow", "Snow-covered world, winter wonderland",
   COSMETIC_HABITAT_THEME, SEASON_4, 1, 12, true, 0xF0FF},
  {64, "Winter Holiday", "Christmas lights and decorations",
   COSMETIC_HABITAT_THEME, SEASON_4, 2, 93, false, 0xF800},
  {65, "Winter Cozy", "Cabin interior with fireplace",
   COSMETIC_HABITAT_THEME, SEASON_4, 1, 38, false, 0xE260},

  // Location Themes
  {66, "Jungle", "Dense rainforest canopy, vibrant life",
   COSMETIC_HABITAT_THEME, SEASON_1, 2, 52, false, 0x0600},
  {67, "Desert", "Sand dunes, heat shimmer, oasis",
   COSMETIC_HABITAT_THEME, SEASON_2, 2, 62, false, 0xFC80},
  {68, "Mountain", "Snowy peaks, alpine meadows, crisp air",
   COSMETIC_HABITAT_THEME, SEASON_3, 2, 68, false, 0x0FFF},
  {69, "Ocean", "Deep sea vibes, bioluminescent creatures",
   COSMETIC_HABITAT_THEME, SEASON_2, 2, 77, false, 0x0010},
  {70, "Arctic", "Northern lights, ice formations, aurora",
   COSMETIC_HABITAT_THEME, SEASON_4, 2, 82, false, 0x07FF},
  {71, "Space", "Outer space, stars, cosmic radiation",
   COSMETIC_HABITAT_THEME, SEASON_3, 3, 98, false, 0x0010},

  // Aesthetic Themes
  {72, "Dark Mode", "Dark background with light UI elements",
   COSMETIC_HABITAT_THEME, SEASON_1, 0, 1, true, 0x1082},
  {73, "Light Mode", "Bright background, minimal design",
   COSMETIC_HABITAT_THEME, SEASON_1, 0, 1, true, 0xFFFF},
  {74, "Retro", "8-bit pixel art, chiptune music",
   COSMETIC_HABITAT_THEME, SEASON_4, 1, 28, false, 0xFFE0},
  {75, "Cyberpunk", "Neon colors, digital rain effect",
   COSMETIC_HABITAT_THEME, SEASON_3, 2, 73, false, 0xF01F},
  {76, "Nature", "Organic shapes, earth tones, nature sounds",
   COSMETIC_HABITAT_THEME, SEASON_2, 1, 24, false, 0x07E0},
  {77, "Steampunk", "Brass gears, industrial aesthetic",
   COSMETIC_HABITAT_THEME, SEASON_3, 2, 78, false, 0xA2A0},
  {78, "Minimalist", "Clean, simple design, no clutter",
   COSMETIC_HABITAT_THEME, SEASON_1, 1, 14, false, 0x1082},
  {79, "Royal", "Gold accents, elegant fonts, orchestral music",
   COSMETIC_HABITAT_THEME, SEASON_4, 2, 89, false, 0xFFC0},
};

// ============================================================================
// EFFECTS (20) - Particle effects and animations
// ============================================================================

static const Cosmetic EFFECTS[] = {
  // Catch Effects
  {80, "Rainbow Sparkles", "Rainbow particles on successful catch",
   COSMETIC_EFFECT, SEASON_1, 1, 16, true, 0xFF80},
  {81, "Golden Stars", "Gold stars burst on catch",
   COSMETIC_EFFECT, SEASON_2, 2, 51, false, 0xFFC0},
  {82, "Ice Crystals", "Frozen crystal effect",
   COSMETIC_EFFECT, SEASON_4, 2, 63, false, 0x0FFF},
  {83, "Fire Burst", "Flame particles on catch",
   COSMETIC_EFFECT, SEASON_3, 2, 69, false, 0xF900},

  // Pet Effects
  {84, "Glow Aura", "Glowing light around pet",
   COSMETIC_EFFECT, SEASON_1, 1, 17, false, 0xFFC0},
  {85, "Shadow Aura", "Dark shadow effect around pet",
   COSMETIC_EFFECT, SEASON_3, 1, 31, false, 0x0010},
  {86, "Sparkle Trail", "Sparkles follow pet movement",
   COSMETIC_EFFECT, SEASON_2, 1, 19, false, 0xFF80},
  {87, "Flame Trail", "Flame trail behind pet",
   COSMETIC_EFFECT, SEASON_3, 2, 71, false, 0xF800},
  {88, "Water Droplets", "Water effects around pet",
   COSMETIC_EFFECT, SEASON_2, 1, 25, false, 0x0FFF},
  {89, "Leaf Trail", "Leaves swirl around pet",
   COSMETIC_EFFECT, SEASON_1, 1, 27, false, 0x07E0},

  // UI Effects
  {90, "Confetti", "Confetti on achievements",
   COSMETIC_EFFECT, SEASON_1, 0, 1, true, 0xFF80},
  {91, "Particle Rain", "Particles rain on screen",
   COSMETIC_EFFECT, SEASON_4, 1, 30, false, 0x001F},
  {92, "Screen Shake", "Camera shakes on major events",
   COSMETIC_EFFECT, SEASON_2, 1, 23, false, 0x8410},
  {93, "Glow Pulse", "Pulsing glow on selection",
   COSMETIC_EFFECT, SEASON_3, 1, 36, false, 0xFFC0},
  {94, "Slow Motion", "Slow-mo effect on catch animations",
   COSMETIC_EFFECT, SEASON_1, 2, 87, false, 0x0FFF},

  // Sound Effects (cosmetic audio)
  {95, "Chime", "Musical chime on events",
   COSMETIC_EFFECT, SEASON_1, 0, 1, true, 0xFFFF},
  {96, "Bells", "Jingle bell sounds",
   COSMETIC_EFFECT, SEASON_4, 1, 26, false, 0xFFC0},
  {97, "Digital Beep", "Retro 8-bit beep sounds",
   COSMETIC_EFFECT, SEASON_3, 1, 32, false, 0xFFE0},
  {98, "Whoosh", "Wind sound effects",
   COSMETIC_EFFECT, SEASON_2, 1, 20, false, 0x07E0},
  {99, "Magical", "Mystical sparkle/magic sounds",
   COSMETIC_EFFECT, SEASON_3, 2, 76, false, 0xF01F},
};

// ============================================================================
// Battle Pass Structure
// ============================================================================

struct BattlePassCosmeticTier {
  uint8_t tier;           // 1-100
  uint8_t cosmetic_id;    // Which cosmetic at this tier
  const char *reward_name;
};

// ============================================================================
// Cosmetic State (player inventory)
// ============================================================================

struct CosmeticInventory {
  bool owned[COSMETICS_TOTAL];  // Which cosmetics player has
  uint8_t equipped_pet_skin;
  uint8_t equipped_trainer;
  uint8_t equipped_habitat;
  uint8_t equipped_effect;
};

// ============================================================================
// Initialize cosmetics inventory
// ============================================================================
static inline void cosmetics_init(CosmeticInventory *inv) {
  // All players start with basic cosmetics
  inv->owned[0] = true;   // Spring Blossom (free)
  inv->owned[30] = true;  // Spring Garden outfit (free)
  inv->owned[55] = true;  // Spring Garden theme (free)
  inv->owned[80] = true;  // Rainbow Sparkles (free)
  inv->owned[90] = true;  // Confetti (free)

  inv->equipped_pet_skin = 0;
  inv->equipped_trainer = 30;
  inv->equipped_habitat = 55;
  inv->equipped_effect = 80;
}

// ============================================================================
// Unlock cosmetic
// ============================================================================
static inline bool cosmetics_unlock(CosmeticInventory *inv, uint8_t cosmetic_id) {
  if (cosmetic_id >= COSMETICS_TOTAL) return false;
  if (inv->owned[cosmetic_id]) return false;

  inv->owned[cosmetic_id] = true;
  return true;
}

// ============================================================================
// Equip cosmetic
// ============================================================================
static inline bool cosmetics_equip(CosmeticInventory *inv, uint8_t cosmetic_id,
                                    uint8_t cosmetic_type) {
  if (cosmetic_id >= COSMETICS_TOTAL) return false;
  if (!inv->owned[cosmetic_id]) return false;

  switch (cosmetic_type) {
    case COSMETIC_PET_SKIN:
      inv->equipped_pet_skin = cosmetic_id;
      return true;
    case COSMETIC_TRAINER_OUTFIT:
      inv->equipped_trainer = cosmetic_id;
      return true;
    case COSMETIC_HABITAT_THEME:
      inv->equipped_habitat = cosmetic_id;
      return true;
    case COSMETIC_EFFECT:
      inv->equipped_effect = cosmetic_id;
      return true;
    default:
      return false;
  }
}

// ============================================================================
// Get total cosmetics owned
// ============================================================================
static inline uint16_t cosmetics_get_owned_count(const CosmeticInventory *inv) {
  uint16_t count = 0;
  for (int i = 0; i < COSMETICS_TOTAL; i++) {
    if (inv->owned[i]) count++;
  }
  return count;
}

// ============================================================================
// Get cosmetics by type
// ============================================================================
static inline uint8_t cosmetics_get_type_count(const CosmeticInventory *inv, uint8_t type) {
  uint8_t count = 0;
  // Would iterate through catalog and count by type
  // Simplified for this implementation
  return count;
}
