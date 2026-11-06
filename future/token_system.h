/**
 * Tamanimals - Token System
 * Habitat-based tokens for catching animals
 *
 * Tokens represent "artifacts of trust" from each habitat.
 * Players earn tokens and use them to build mutual respect with animals.
 */

#pragma once
#include <stdint.h>

// ============================================================================
// TOKEN TYPES (One per habitat)
// ============================================================================

typedef enum {
  TOKEN_BONE = 0,       // Savanna - bone carving
  TOKEN_WOOD = 1,       // Forest - wooden charm
  TOKEN_PEARL = 2,      // Ocean - shell pendant
  TOKEN_CRYSTAL = 3,    // Arctic - ice crystal
  TOKEN_SAND = 4,       // Desert - sand sculpture
  TOKEN_HERB = 5,       // Rainforest - herb bundle
  TOKEN_STONE = 6,      // Mountain - stone tablet
  TOKEN_SEED = 7,       // Grassland - seed amulet
  TOKEN_REED = 8,       // Wetland - reed carving
  TOKEN_CORAL = 9,      // Coral Reef - coral artifact
  TOKEN_SNOW = 10,      // Tundra - snow crystal
  TOKEN_GOLDEN = 11,    // Ancient/Special - golden artifact (100% catch)
  TOKEN_MAX = 12
} TokenType;

// ============================================================================
// TOKEN INVENTORY
// ============================================================================

typedef struct {
  uint16_t tokens[TOKEN_MAX];  // Count for each token type (max 9999 each)
  uint32_t coins;              // Currency earned from minigames (max 99999)
} TokenInventory;

// ============================================================================
// TOKEN SYSTEM FUNCTIONS
// ============================================================================

/**
 * Initialize token inventory with starter tokens
 */
static inline void tokens_init(TokenInventory *inv) {
  inv->coins = 0;

  // Start with basic tokens to learn system
  inv->tokens[TOKEN_BONE] = 5;     // Savanna tokens
  inv->tokens[TOKEN_WOOD] = 3;     // Forest tokens
  inv->tokens[TOKEN_SEED] = 3;     // Grassland tokens

  for (int i = 0; i < TOKEN_MAX; i++) {
    if (i != TOKEN_BONE && i != TOKEN_WOOD && i != TOKEN_SEED) {
      inv->tokens[i] = 0;
    }
  }
}

/**
 * Add coins to inventory
 * Earned from minigames, tier-ups, daily rewards
 */
static inline void tokens_add_coins(TokenInventory *inv, uint16_t amount) {
  inv->coins = (inv->coins + amount > 99999) ? 99999 : inv->coins + amount;
}

/**
 * Use a token (reduce count)
 * Returns true if token was available and used
 */
static inline bool tokens_use(TokenInventory *inv, uint8_t token_type) {
  if (token_type < TOKEN_MAX && inv->tokens[token_type] > 0) {
    inv->tokens[token_type]--;
    return true;
  }
  return false;
}

/**
 * Add tokens to inventory
 * Can come from minigames, daily rewards, tier-ups
 */
static inline void tokens_add(TokenInventory *inv, uint8_t token_type, uint16_t count) {
  if (token_type >= TOKEN_MAX) return;

  uint16_t max_val = (token_type == TOKEN_GOLDEN) ? 99 : 9999;
  inv->tokens[token_type] = (inv->tokens[token_type] + count > max_val) ?
                            max_val : inv->tokens[token_type] + count;
}

/**
 * Get token name for display
 */
static inline const char* tokens_get_name(uint8_t token_type) {
  switch (token_type) {
    case TOKEN_BONE: return "Bone Token";
    case TOKEN_WOOD: return "Wood Token";
    case TOKEN_PEARL: return "Pearl Token";
    case TOKEN_CRYSTAL: return "Crystal Token";
    case TOKEN_SAND: return "Sand Token";
    case TOKEN_HERB: return "Herb Token";
    case TOKEN_STONE: return "Stone Token";
    case TOKEN_SEED: return "Seed Token";
    case TOKEN_REED: return "Reed Token";
    case TOKEN_CORAL: return "Coral Token";
    case TOKEN_SNOW: return "Snow Token";
    case TOKEN_GOLDEN: return "Golden Token";
    default: return "Unknown Token";
  }
}

/**
 * Get habitat ID for a token type
 * Used to match token to animal habitat for bonus
 */
static inline uint8_t tokens_get_habitat(uint8_t token_type) {
  // Token type matches habitat ID directly
  if (token_type < TOKEN_MAX - 1) {
    return token_type;
  }
  return 0;  // Golden token works on all
}

/**
 * Get cost in coins for a token
 * Costs vary by rarity
 */
static inline uint16_t tokens_get_cost(uint8_t token_type) {
  switch (token_type) {
    case TOKEN_BONE:
    case TOKEN_WOOD:
    case TOKEN_SEED:
    case TOKEN_REED: return 2;     // Common - 2 coins

    case TOKEN_PEARL:
    case TOKEN_SAND:
    case TOKEN_HERB:
    case TOKEN_CORAL: return 25;   // Uncommon - 25 coins

    case TOKEN_CRYSTAL:
    case TOKEN_STONE:
    case TOKEN_SNOW: return 100;   // Rare - 100 coins

    case TOKEN_GOLDEN: return 500; // Special - 500 coins (very rare)

    default: return 0;
  }
}

/**
 * Buy tokens from shop using coins
 * Returns true if purchase successful
 */
static inline bool tokens_buy(TokenInventory *inv, uint8_t token_type, uint16_t quantity) {
  if (token_type >= TOKEN_MAX) return false;

  uint16_t cost_per = tokens_get_cost(token_type);
  uint32_t total_cost = (uint32_t)cost_per * quantity;

  if (inv->coins >= total_cost) {
    inv->coins -= total_cost;
    tokens_add(inv, token_type, quantity);
    return true;
  }

  return false;  // Not enough coins
}

/**
 * Get catch rate modifier for token type
 * Golden token = guaranteed 100%
 * Golden token returns very high value to indicate special case
 */
static inline uint8_t tokens_get_modifier(uint8_t token_type) {
  if (token_type == TOKEN_GOLDEN) {
    return 255;  // 100% catch rate
  }
  return 100;    // Base 1.0x modifier (combined with respect)
}

/**
 * Check if token is in stock (available to use)
 */
static inline bool tokens_is_available(TokenInventory *inv, uint8_t token_type) {
  if (token_type < TOKEN_MAX) {
    return inv->tokens[token_type] > 0;
  }
  return false;
}

/**
 * Get total tokens in inventory
 */
static inline uint16_t tokens_get_total(TokenInventory *inv) {
  uint16_t total = 0;
  for (int i = 0; i < TOKEN_MAX; i++) {
    total += inv->tokens[i];
  }
  return total;
}
