#pragma once

// ============================================================================
// MENTAL HEALTH SAFEGUARDS - Phase 4
// ============================================================================
// Responsible gaming features: session limits, play time caps, alerts

#include <stdint.h>
#include <time.h>

#define SESSION_ALERT_MS (2 * 60 * 60 * 1000)  // 2 hours

enum PlayTimeLimit {
  LIMIT_NONE = 0,
  LIMIT_30_MIN = 30,
  LIMIT_60_MIN = 60,
  LIMIT_90_MIN = 90,
};

enum MoodLevel {
  MOOD_EXCELLENT = 0,
  MOOD_GOOD = 1,
  MOOD_OK = 2,
  MOOD_STRESSED = 3,
  MOOD_TIRED = 4,
};

struct SafeguardsState {
  bool safeguards_enabled;
  uint8_t daily_limit_minutes;     // 0 = no limit, else 30/60/90
  uint32_t session_start;
  bool session_alert_shown;
  uint8_t play_time_today;         // Minutes played today
  uint8_t mood_check_status;       // Last mood selected
  uint32_t last_mood_check;
  bool break_recommended;
  uint32_t last_break_taken;
};

// ============================================================================
// Initialize safeguards
// ============================================================================
static inline void safeguards_init(SafeguardsState *state) {
  state->safeguards_enabled = true;
  state->daily_limit_minutes = LIMIT_NONE;
  state->session_start = time(NULL);
  state->session_alert_shown = false;
  state->play_time_today = 0;
  state->mood_check_status = MOOD_GOOD;
  state->last_mood_check = time(NULL);
  state->break_recommended = false;
  state->last_break_taken = time(NULL);
}

// ============================================================================
// Set daily play time limit
// ============================================================================
static inline void safeguards_set_daily_limit(SafeguardsState *state, uint8_t minutes) {
  state->daily_limit_minutes = minutes;
}

// ============================================================================
// Update session time
// ============================================================================
static inline void safeguards_update_session(SafeguardsState *state, uint32_t delta_ms) {
  state->play_time_today += (delta_ms / 60000);  // Add minutes
}

// ============================================================================
// Check if session alert needed (2 hours of play)
// ============================================================================
static inline bool safeguards_should_show_session_alert(SafeguardsState *state) {
  if (!state->safeguards_enabled) return false;
  if (state->session_alert_shown) return false;

  uint32_t now = time(NULL);
  uint32_t session_duration = (now - state->session_start) * 1000;

  if (session_duration >= SESSION_ALERT_MS) {
    state->session_alert_shown = true;
    state->break_recommended = true;
    return true;
  }

  return false;
}

// ============================================================================
// Check if daily limit reached
// ============================================================================
static inline bool safeguards_daily_limit_reached(const SafeguardsState *state) {
  if (state->daily_limit_minutes == LIMIT_NONE) return false;
  return state->play_time_today >= state->daily_limit_minutes;
}

// ============================================================================
// Get time remaining today
// ============================================================================
static inline uint32_t safeguards_get_time_remaining(const SafeguardsState *state) {
  if (state->daily_limit_minutes == LIMIT_NONE) return 999999;
  if (state->play_time_today >= state->daily_limit_minutes) return 0;
  return state->daily_limit_minutes - state->play_time_today;
}

// ============================================================================
// Check mood and provide resources
// Returns: should show mood check dialog?
// ============================================================================
static inline bool safeguards_should_check_mood(SafeguardsState *state) {
  uint32_t now = time(NULL);
  uint32_t minutes_since_check = (now - state->last_mood_check) / 60;

  // Check mood every 30 minutes if break was recommended
  if (state->break_recommended && minutes_since_check > 30) {
    return true;
  }

  return false;
}

// ============================================================================
// Record mood selection
// ============================================================================
static inline void safeguards_record_mood(SafeguardsState *state, uint8_t mood) {
  state->mood_check_status = mood;
  state->last_mood_check = time(NULL);

  if (mood == MOOD_STRESSED || mood == MOOD_TIRED) {
    state->break_recommended = true;
  }
}

// ============================================================================
// Get mood description
// ============================================================================
static inline const char *safeguards_get_mood_text(uint8_t mood) {
  switch (mood) {
    case MOOD_EXCELLENT:
      return "Feeling great! Keep enjoying the game.";
    case MOOD_GOOD:
      return "Feeling good! Enjoy your gameplay.";
    case MOOD_OK:
      return "Doing okay. Consider a short break if you've been playing a while.";
    case MOOD_STRESSED:
      return "Feeling stressed? Games should be fun. Take a break, get some fresh air.";
    case MOOD_TIRED:
      return "Feeling tired? Rest is important. Maybe grab some sleep or take a break.";
    default:
      return "How are you feeling?";
  }
}

// ============================================================================
// Get mental health resources
// ============================================================================
static inline const char *safeguards_get_resource_link(uint8_t mood) {
  if (mood == MOOD_STRESSED) {
    return "https://www.mindful.org/meditation/mindfulness-getting-started/";
  }
  if (mood == MOOD_TIRED) {
    return "https://www.sleepfoundation.org/";
  }
  return "";
}

// ============================================================================
// Record break taken
// ============================================================================
static inline void safeguards_record_break(SafeguardsState *state) {
  state->last_break_taken = time(NULL);
  state->break_recommended = false;
  state->session_alert_shown = false;
}

// ============================================================================
// Reset daily counters (called at midnight)
// ============================================================================
static inline void safeguards_reset_daily(SafeguardsState *state) {
  state->play_time_today = 0;
  state->session_start = time(NULL);
  state->session_alert_shown = false;
  state->break_recommended = false;
}

// ============================================================================
// Serialize safeguards state
// ============================================================================
static inline void safeguards_serialize(const SafeguardsState *state, uint8_t *buffer) {
  buffer[0] = state->safeguards_enabled ? 1 : 0;
  buffer[1] = state->daily_limit_minutes;
  buffer[2] = state->play_time_today;
  buffer[3] = state->mood_check_status;
}

// ============================================================================
// Deserialize safeguards state
// ============================================================================
static inline void safeguards_deserialize(SafeguardsState *state, const uint8_t *buffer) {
  state->safeguards_enabled = buffer[0] != 0;
  state->daily_limit_minutes = buffer[1];
  state->play_time_today = buffer[2];
  state->mood_check_status = buffer[3];
}
