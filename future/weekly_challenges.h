#pragma once

// ============================================================================
// WEEKLY CHALLENGES SYSTEM - Phase 3 Community Improvement
// ============================================================================
// 7 different challenges each week - player must complete 4 of 7 to earn reward
// NO STREAK PUNISHMENT - missing days doesn't destroy progress
// Research shows flexible goals are more engaging than daily streaks

#include <stdint.h>
#include <time.h>
#include <string.h>

#define CHALLENGES_PER_WEEK 7
#define CHALLENGES_REQUIRED 4
#define CHALLENGE_WEEKS 12  // Track 12 weeks of challenge history

enum ChallengeType {
  CHALLENGE_WALK = 0,          // Walk 20,000 steps
  CHALLENGE_CATCH = 1,         // Catch 5 animals
  CHALLENGE_MINIGAME = 2,      // Win 10 minigames
  CHALLENGE_COINS = 3,         // Earn 500 coins
  CHALLENGE_RESPECT = 4,       // Earn 200 respect
  CHALLENGE_CARE = 5,          // Feed/clean/sleep 3 times
  CHALLENGE_FACTS = 6,         // Learn 5 animal facts
};

struct WeeklyChallenge {
  uint8_t id;
  uint8_t type;                // CHALLENGE_WALK, etc.
  const char *name;
  const char *description;
  uint32_t target;             // Goal amount
  uint32_t current;            // Current progress
  bool completed;              // Challenge completed?
  uint32_t deadline;           // Unix timestamp when week ends
};

struct WeeklyChallengeState {
  uint8_t week_number;         // 0-51 (52 weeks/year)
  uint32_t week_started;       // Unix timestamp
  uint32_t week_ends;          // Unix timestamp (7 days later)
  WeeklyChallenge challenges[CHALLENGES_PER_WEEK];
  uint8_t completed_count;     // How many of 7 completed
  bool weekly_reward_claimed;  // Reward for completing 4+ challenges
};

struct ChallengeHistory {
  uint8_t week_number;
  uint8_t completed_challenges;
  uint32_t week_timestamp;
  uint16_t reward_earned;      // Coins/tokens earned
};

// Challenge definitions for each type
static const WeeklyChallenge CHALLENGE_TEMPLATE[] = {
  {0, CHALLENGE_WALK, "Walker", "Walk 20,000 steps", 20000, 0, false, 0},
  {1, CHALLENGE_CATCH, "Hunter", "Catch 5 animals", 5, 0, false, 0},
  {2, CHALLENGE_MINIGAME, "Gamer", "Win 10 minigames", 10, 0, false, 0},
  {3, CHALLENGE_COINS, "Collector", "Earn 500 coins", 500, 0, false, 0},
  {4, CHALLENGE_RESPECT, "Friend", "Earn 200 respect", 200, 0, false, 0},
  {5, CHALLENGE_CARE, "Caregiver", "Care for pet 3 times", 3, 0, false, 0},
  {6, CHALLENGE_FACTS, "Scholar", "Learn 5 animal facts", 5, 0, false, 0},
};

// ============================================================================
// Initialize weekly challenges
// ============================================================================
static inline void weekly_challenges_init(WeeklyChallengeState *state) {
  state->week_number = 0;
  state->week_started = time(NULL);
  state->week_ends = state->week_started + (7 * 24 * 3600);
  state->completed_count = 0;
  state->weekly_reward_claimed = false;

  // Copy templates
  for (int i = 0; i < CHALLENGES_PER_WEEK; i++) {
    state->challenges[i] = CHALLENGE_TEMPLATE[i];
    state->challenges[i].current = 0;
    state->challenges[i].completed = false;
    state->challenges[i].deadline = state->week_ends;
  }
}

// ============================================================================
// Add progress to a challenge
// Returns: true if challenge was completed this call
// ============================================================================
static inline bool weekly_challenges_add_progress(WeeklyChallengeState *state,
                                                   uint8_t challenge_type, uint32_t amount) {
  bool just_completed = false;

  for (int i = 0; i < CHALLENGES_PER_WEEK; i++) {
    if (state->challenges[i].type == challenge_type && !state->challenges[i].completed) {
      uint32_t old_progress = state->challenges[i].current;
      state->challenges[i].current += amount;

      if (state->challenges[i].current >= state->challenges[i].target) {
        state->challenges[i].completed = true;
        state->completed_count++;
        just_completed = true;
      }

      break;
    }
  }

  return just_completed;
}

// ============================================================================
// Get challenge progress (0-100%)
// ============================================================================
static inline uint8_t weekly_challenges_get_progress_percent(const WeeklyChallenge *challenge) {
  if (challenge->target == 0) return 100;
  uint8_t percent = (challenge->current * 100) / challenge->target;
  if (percent > 100) percent = 100;
  return percent;
}

// ============================================================================
// Check if weekly goal is met (4+ of 7 completed)
// ============================================================================
static inline bool weekly_challenges_is_goal_met(const WeeklyChallengeState *state) {
  return state->completed_count >= CHALLENGES_REQUIRED;
}

// ============================================================================
// Get weekly reward (increases each week)
// ============================================================================
static inline uint16_t weekly_challenges_get_reward(uint8_t week_number) {
  // Rewards escalate over month
  uint8_t month = (week_number % 52) / 4;
  uint16_t base_reward = 100;  // Week 1: 100 coins

  switch (month) {
    case 0:
      return base_reward;
    case 1:
      return base_reward + 50;   // Week 5: 150 coins
    case 2:
      return base_reward + 100;  // Week 9: 200 coins
    case 3:
      return base_reward + 200;  // Week 13: 300 coins + rare token
    default:
      return base_reward + 200;
  }
}

// ============================================================================
// Claim weekly reward (if 4+ challenges completed)
// ============================================================================
static inline bool weekly_challenges_claim_reward(WeeklyChallengeState *state) {
  if (state->weekly_reward_claimed) return false;  // Already claimed
  if (!weekly_challenges_is_goal_met(state)) return false;  // Not enough completed

  state->weekly_reward_claimed = true;
  return true;
}

// ============================================================================
// Check if week is over
// ============================================================================
static inline bool weekly_challenges_is_week_over(const WeeklyChallengeState *state) {
  uint32_t now = time(NULL);
  return now >= state->week_ends;
}

// ============================================================================
// Get time remaining in week (seconds)
// ============================================================================
static inline uint32_t weekly_challenges_get_time_remaining(const WeeklyChallengeState *state) {
  uint32_t now = time(NULL);
  if (now >= state->week_ends) return 0;
  return state->week_ends - now;
}

// ============================================================================
// Get time remaining as days:hours
// ============================================================================
static inline void weekly_challenges_get_time_remaining_text(const WeeklyChallengeState *state,
                                                              char *buffer) {
  uint32_t remaining = weekly_challenges_get_time_remaining(state);
  uint32_t days = remaining / (24 * 3600);
  uint32_t hours = (remaining % (24 * 3600)) / 3600;
  // sprintf(buffer, "%d days, %d hours", days, hours);
  buffer[0] = '\0';
}

// ============================================================================
// Reset to next week
// ============================================================================
static inline void weekly_challenges_next_week(WeeklyChallengeState *state) {
  state->week_number++;
  state->week_started = time(NULL);
  state->week_ends = state->week_started + (7 * 24 * 3600);
  state->completed_count = 0;
  state->weekly_reward_claimed = false;

  // Reset challenge progress but keep same challenges
  for (int i = 0; i < CHALLENGES_PER_WEEK; i++) {
    state->challenges[i].current = 0;
    state->challenges[i].completed = false;
    state->challenges[i].deadline = state->week_ends;
  }
}

// ============================================================================
// Rotate challenges (select random subset each week)
// ============================================================================
static inline void weekly_challenges_rotate(WeeklyChallengeState *state) {
  // Simple rotation: cycle through challenge types
  uint8_t start_idx = (state->week_number % 7);
  for (int i = 0; i < CHALLENGES_PER_WEEK; i++) {
    uint8_t challenge_idx = (start_idx + i) % 7;
    state->challenges[i] = CHALLENGE_TEMPLATE[challenge_idx];
    state->challenges[i].current = 0;
    state->challenges[i].completed = false;
    state->challenges[i].deadline = state->week_ends;
  }
}

// ============================================================================
// Get challenge name
// ============================================================================
static inline const char *weekly_challenges_get_name(uint8_t challenge_type) {
  switch (challenge_type) {
    case CHALLENGE_WALK:
      return "Walker";
    case CHALLENGE_CATCH:
      return "Hunter";
    case CHALLENGE_MINIGAME:
      return "Gamer";
    case CHALLENGE_COINS:
      return "Collector";
    case CHALLENGE_RESPECT:
      return "Friend";
    case CHALLENGE_CARE:
      return "Caregiver";
    case CHALLENGE_FACTS:
      return "Scholar";
    default:
      return "Unknown";
  }
}

// ============================================================================
// Get challenge description
// ============================================================================
static inline const char *weekly_challenges_get_description(uint8_t challenge_type) {
  switch (challenge_type) {
    case CHALLENGE_WALK:
      return "Walk 20,000 steps";
    case CHALLENGE_CATCH:
      return "Catch 5 animals";
    case CHALLENGE_MINIGAME:
      return "Win 10 minigames";
    case CHALLENGE_COINS:
      return "Earn 500 coins";
    case CHALLENGE_RESPECT:
      return "Earn 200 respect";
    case CHALLENGE_CARE:
      return "Care 3 times";
    case CHALLENGE_FACTS:
      return "Learn 5 facts";
    default:
      return "Unknown";
  }
}

// ============================================================================
// Get progress string
// ============================================================================
static inline void weekly_challenges_get_progress_text(const WeeklyChallenge *challenge,
                                                        char *buffer) {
  // sprintf(buffer, "%d / %d", challenge->current, challenge->target);
  buffer[0] = '\0';
}

// ============================================================================
// Add challenge to history
// ============================================================================
static inline void weekly_challenges_add_history(ChallengeHistory *history,
                                                 const WeeklyChallengeState *state) {
  history->week_number = state->week_number;
  history->completed_challenges = state->completed_count;
  history->week_timestamp = state->week_started;
  history->reward_earned = weekly_challenges_is_goal_met(state)
                          ? weekly_challenges_get_reward(state->week_number)
                          : 0;
}

// ============================================================================
// Serialize weekly challenges state
// ============================================================================
static inline void weekly_challenges_serialize(const WeeklyChallengeState *state, uint8_t *buffer) {
  buffer[0] = state->week_number;
  buffer[1] = state->completed_count;
  buffer[2] = state->weekly_reward_claimed ? 1 : 0;
  uint32_t *p = (uint32_t *)(buffer + 3);
  p[0] = state->week_started;
  p[1] = state->week_ends;
  // Full serialization would include all challenge data
}

// ============================================================================
// Deserialize weekly challenges state
// ============================================================================
static inline void weekly_challenges_deserialize(WeeklyChallengeState *state,
                                                  const uint8_t *buffer) {
  state->week_number = buffer[0];
  state->completed_count = buffer[1];
  state->weekly_reward_claimed = buffer[2] != 0;
  const uint32_t *p = (const uint32_t *)(buffer + 3);
  state->week_started = p[0];
  state->week_ends = p[1];
}
