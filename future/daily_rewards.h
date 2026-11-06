#pragma once

// ============================================================================
// DAILY REWARDS SYSTEM - Phase 1 Engagement Improvement
// ============================================================================
// Provides daily login bonuses WITHOUT streak punishment (ethical engagement)
// Players earn bonus coins on first login each calendar day
// No penalty for missed days - encouraging without FOMO anxiety

#include <stdint.h>
#include <time.h>

// Daily reward amounts (coins)
#define DAILY_REWARD_BASE 15
#define DAILY_REWARD_WEEKEND_BONUS 5  // Extra bonus on Sat/Sun

struct DailyRewards {
  uint32_t last_login_day;   // Unix timestamp of last login (day-based)
  uint16_t coins_earned_today;
  uint8_t consecutive_days;  // Just for stats, not for penalties
};

// ============================================================================
// Initialize daily rewards system
// ============================================================================
static inline void daily_rewards_init(DailyRewards *rewards) {
  rewards->last_login_day = 0;
  rewards->coins_earned_today = 0;
  rewards->consecutive_days = 0;
}

// ============================================================================
// Get current day (0-indexed from epoch)
// ============================================================================
static inline uint32_t daily_rewards_get_day() {
  // Simple day calculation: seconds since epoch / 86400 (seconds per day)
  time_t now = time(NULL);
  return now / 86400;
}

// ============================================================================
// Check if it's a new day since last login
// ============================================================================
static inline bool daily_rewards_is_new_day(DailyRewards *rewards) {
  uint32_t current_day = daily_rewards_get_day();
  return current_day > rewards->last_login_day;
}

// ============================================================================
// Award daily bonus if first login of the day
// Returns: coins earned (0 if already earned today)
// ============================================================================
static inline uint16_t daily_rewards_check_login(DailyRewards *rewards) {
  uint32_t current_day = daily_rewards_get_day();

  // Already earned today?
  if (current_day == rewards->last_login_day && rewards->coins_earned_today > 0) {
    return 0;  // Already earned
  }

  // New day - award bonus
  rewards->last_login_day = current_day;

  // Base reward
  uint16_t bonus = DAILY_REWARD_BASE;

  // Weekend bonus (Saturday = 5, Sunday = 6)
  time_t now = time(NULL);
  struct tm *timeinfo = localtime(&now);
  int wday = timeinfo->tm_wday;
  if (wday == 0 || wday == 6) {  // Sunday or Saturday
    bonus += DAILY_REWARD_WEEKEND_BONUS;
  }

  // Track consecutive days (for display, not punishment)
  if (current_day - rewards->last_login_day <= 1) {
    rewards->consecutive_days++;
  } else {
    rewards->consecutive_days = 1;
  }

  rewards->coins_earned_today = bonus;
  return bonus;
}

// ============================================================================
// Get coins earned today (for display)
// ============================================================================
static inline uint16_t daily_rewards_get_today_total(DailyRewards *rewards) {
  return rewards->coins_earned_today;
}

// ============================================================================
// Get consecutive days count (for display/stats only)
// NOTE: This is informational, NOT used for penalties
// ============================================================================
static inline uint8_t daily_rewards_get_streak(DailyRewards *rewards) {
  return rewards->consecutive_days;
}

// ============================================================================
// Reset at midnight (called from save system)
// ============================================================================
static inline void daily_rewards_reset_day(DailyRewards *rewards) {
  rewards->coins_earned_today = 0;
  // last_login_day is NOT reset - it tracks when last earned
}

// ============================================================================
// Serialize for NVS storage
// ============================================================================
static inline void daily_rewards_serialize(DailyRewards *rewards, uint8_t *buffer) {
  uint32_t *p = (uint32_t *)buffer;
  p[0] = rewards->last_login_day;
  uint16_t *p2 = (uint16_t *)(buffer + 4);
  p2[0] = rewards->coins_earned_today;
  buffer[6] = rewards->consecutive_days;
}

// ============================================================================
// Deserialize from NVS storage
// ============================================================================
static inline void daily_rewards_deserialize(DailyRewards *rewards, const uint8_t *buffer) {
  const uint32_t *p = (const uint32_t *)buffer;
  rewards->last_login_day = p[0];
  const uint16_t *p2 = (const uint16_t *)(buffer + 4);
  rewards->coins_earned_today = p2[0];
  rewards->consecutive_days = buffer[6];
}
