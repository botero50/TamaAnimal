#pragma once

// ============================================================================
// BATTLE PASS SYSTEM - Phase 4
// ============================================================================
// $3.99/season, cosmetics-only, no gameplay advantage
// 100 tiers, earnable rewards without payment

#include <stdint.h>
#include <time.h>

#define BATTLE_PASS_TIERS 100
#define BATTLE_PASS_COST_CENTS 399  // $3.99

enum BattlePassStatus {
  BP_NOT_OWNED = 0,
  BP_ACTIVE = 1,
  BP_EXPIRED = 2,
};

struct BattlePassTier {
  uint8_t tier;
  uint16_t required_xp;
  uint8_t cosmetic_reward_id;
  uint16_t coin_reward;
  bool is_premium_only;
};

struct BattlePassState {
  uint8_t season;
  uint8_t status;              // NOT_OWNED, ACTIVE, EXPIRED
  uint32_t purchase_time;
  uint32_t expiration_time;
  uint8_t current_tier;
  uint32_t current_xp;
  bool tiers_claimed[BATTLE_PASS_TIERS];  // Which tiers claimed
};

// ============================================================================
// Initialize battle pass
// ============================================================================
static inline void battle_pass_init(BattlePassState *bp) {
  bp->season = 0;
  bp->status = BP_NOT_OWNED;
  bp->purchase_time = 0;
  bp->expiration_time = 0;
  bp->current_tier = 0;
  bp->current_xp = 0;
  for (int i = 0; i < BATTLE_PASS_TIERS; i++) {
    bp->tiers_claimed[i] = false;
  }
}

// ============================================================================
// Purchase battle pass
// ============================================================================
static inline bool battle_pass_purchase(BattlePassState *bp, uint8_t season) {
  if (bp->status == BP_ACTIVE) return false;  // Already active

  bp->season = season;
  bp->status = BP_ACTIVE;
  bp->purchase_time = time(NULL);
  bp->expiration_time = bp->purchase_time + (84 * 24 * 3600);  // 84 days
  bp->current_tier = 0;
  bp->current_xp = 0;

  return true;
}

// ============================================================================
// Add XP to battle pass
// Returns: tier(s) advanced
// ============================================================================
static inline uint8_t battle_pass_add_xp(BattlePassState *bp, uint32_t xp) {
  if (bp->status != BP_ACTIVE) return 0;

  bp->current_xp += xp;
  uint32_t xp_per_tier = 1000;  // 1000 XP per tier
  uint8_t tiers_gained = 0;

  while (bp->current_xp >= xp_per_tier && bp->current_tier < BATTLE_PASS_TIERS) {
    bp->current_xp -= xp_per_tier;
    bp->current_tier++;
    tiers_gained++;
  }

  return tiers_gained;
}

// ============================================================================
// Get XP to next tier
// ============================================================================
static inline uint32_t battle_pass_get_xp_to_next(const BattlePassState *bp) {
  if (bp->current_tier >= BATTLE_PASS_TIERS) return 0;
  uint32_t xp_per_tier = 1000;
  return xp_per_tier - bp->current_xp;
}

// ============================================================================
// Get tier progress (0-100%)
// ============================================================================
static inline uint8_t battle_pass_get_progress(const BattlePassState *bp) {
  uint32_t xp_per_tier = 1000;
  return (bp->current_xp * 100) / xp_per_tier;
}

// ============================================================================
// Claim tier reward
// ============================================================================
static inline bool battle_pass_claim_tier(BattlePassState *bp, uint8_t tier) {
  if (tier >= BATTLE_PASS_TIERS) return false;
  if (tier > bp->current_tier) return false;  // Haven't reached this tier
  if (bp->tiers_claimed[tier]) return false;  // Already claimed

  bp->tiers_claimed[tier] = true;
  return true;
}

// ============================================================================
// Get total tiers claimed
// ============================================================================
static inline uint8_t battle_pass_get_claimed_count(const BattlePassState *bp) {
  uint8_t count = 0;
  for (int i = 0; i < BATTLE_PASS_TIERS; i++) {
    if (bp->tiers_claimed[i]) count++;
  }
  return count;
}

// ============================================================================
// Check if expired
// ============================================================================
static inline bool battle_pass_is_expired(BattlePassState *bp) {
  if (bp->status != BP_ACTIVE) return false;
  uint32_t now = time(NULL);
  if (now >= bp->expiration_time) {
    bp->status = BP_EXPIRED;
    return true;
  }
  return false;
}

// ============================================================================
// Get time remaining (seconds)
// ============================================================================
static inline uint32_t battle_pass_get_time_remaining(const BattlePassState *bp) {
  if (bp->status != BP_ACTIVE) return 0;
  uint32_t now = time(NULL);
  if (now >= bp->expiration_time) return 0;
  return bp->expiration_time - now;
}

// ============================================================================
// Serialize battle pass state
// ============================================================================
static inline void battle_pass_serialize(const BattlePassState *bp, uint8_t *buffer) {
  buffer[0] = bp->season;
  buffer[1] = bp->status;
  buffer[2] = bp->current_tier;
  uint32_t *p = (uint32_t *)(buffer + 3);
  p[0] = bp->current_xp;
  p[1] = bp->expiration_time;
}

// ============================================================================
// Deserialize battle pass state
// ============================================================================
static inline void battle_pass_deserialize(BattlePassState *bp, const uint8_t *buffer) {
  bp->season = buffer[0];
  bp->status = buffer[1];
  bp->current_tier = buffer[2];
  const uint32_t *p = (const uint32_t *)(buffer + 3);
  bp->current_xp = p[0];
  bp->expiration_time = p[1];
}
