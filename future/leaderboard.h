#pragma once

// ============================================================================
// SEGMENTED LEADERBOARD SYSTEM - Phase 3 Community Improvement
// ============================================================================
// Fair competition through segmentation by level tier
// Research shows segmented boards drive 3x more DAU than global leaderboards
// New players don't face insurmountable veteran scores (no discouragement)

#include <stdint.h>
#include <time.h>

#define LEADERBOARD_MAX_ENTRIES 100
#define LEADERBOARD_SEGMENTS 4

// Leaderboard segments (by respect level tier)
enum LeaderboardSegment {
  SEGMENT_BEGINNER = 0,      // Respect levels 1-2
  SEGMENT_INTERMEDIATE = 1,  // Respect levels 3-4
  SEGMENT_ADVANCED = 2,      // Respect levels 5-6
  SEGMENT_MASTER = 3,        // Respect levels 7-8
};

struct LeaderboardEntry {
  uint32_t player_id;
  char player_name[32];
  uint16_t rank;
  uint32_t score;            // Primary score (respect points, collection count, etc.)
  uint8_t secondary_type;    // 0=respect, 1=collection, 2=achievements, 3=coins
  uint32_t secondary_score;  // Secondary score for tie-breaking
  uint32_t updated_at;       // Last score update
};

struct Leaderboard {
  uint8_t segment;
  char segment_name[32];
  LeaderboardEntry entries[LEADERBOARD_MAX_ENTRIES];
  uint16_t entry_count;
  uint32_t last_reset;       // When leaderboard was last reset (weekly)
};

struct PlayerLeaderboardStats {
  uint8_t segment;
  uint16_t current_rank;
  uint32_t current_score;
  uint16_t rank_change;      // Change from last week
  uint8_t percentile;        // Top X% in segment
};

// ============================================================================
// Initialize leaderboard for segment
// ============================================================================
static inline void leaderboard_init(Leaderboard *lb, uint8_t segment) {
  lb->segment = segment;
  switch (segment) {
    case SEGMENT_BEGINNER:
      strcpy(lb->segment_name, "Beginner Trainers");
      break;
    case SEGMENT_INTERMEDIATE:
      strcpy(lb->segment_name, "Intermediate Trainers");
      break;
    case SEGMENT_ADVANCED:
      strcpy(lb->segment_name, "Advanced Trainers");
      break;
    case SEGMENT_MASTER:
      strcpy(lb->segment_name, "Master Trainers");
      break;
    default:
      strcpy(lb->segment_name, "Unknown");
  }
  lb->entry_count = 0;
  lb->last_reset = time(NULL);
}

// ============================================================================
// Get segment for player based on respect level
// ============================================================================
static inline uint8_t leaderboard_get_segment(uint8_t respect_level) {
  if (respect_level <= 2) return SEGMENT_BEGINNER;
  if (respect_level <= 4) return SEGMENT_INTERMEDIATE;
  if (respect_level <= 6) return SEGMENT_ADVANCED;
  return SEGMENT_MASTER;
}

// ============================================================================
// Add or update player entry
// ============================================================================
static inline bool leaderboard_update_entry(Leaderboard *lb, uint32_t player_id,
                                            const char *player_name, uint32_t score) {
  // Find existing entry
  int existing_idx = -1;
  for (int i = 0; i < lb->entry_count; i++) {
    if (lb->entries[i].player_id == player_id) {
      existing_idx = i;
      break;
    }
  }

  if (existing_idx >= 0) {
    // Update existing entry
    lb->entries[existing_idx].score = score;
    lb->entries[existing_idx].updated_at = time(NULL);
  } else {
    // Add new entry if room
    if (lb->entry_count >= LEADERBOARD_MAX_ENTRIES) {
      // Remove lowest scorer if full
      uint32_t lowest_score = lb->entries[0].score;
      int lowest_idx = 0;
      for (int i = 1; i < lb->entry_count; i++) {
        if (lb->entries[i].score < lowest_score) {
          lowest_score = lb->entries[i].score;
          lowest_idx = i;
        }
      }

      if (score <= lowest_score) return false;  // Don't add if score too low

      // Replace lowest
      existing_idx = lowest_idx;
      lb->entries[existing_idx].player_id = player_id;
      strncpy(lb->entries[existing_idx].player_name, player_name, 31);
      lb->entries[existing_idx].score = score;
      lb->entries[existing_idx].updated_at = time(NULL);
    } else {
      // Add to end
      lb->entries[lb->entry_count].player_id = player_id;
      strncpy(lb->entries[lb->entry_count].player_name, player_name, 31);
      lb->entries[lb->entry_count].score = score;
      lb->entries[lb->entry_count].updated_at = time(NULL);
      lb->entry_count++;
    }
  }

  // Re-sort by score (simple bubble sort for small arrays)
  for (int i = 0; i < lb->entry_count; i++) {
    for (int j = i + 1; j < lb->entry_count; j++) {
      if (lb->entries[j].score > lb->entries[i].score) {
        LeaderboardEntry temp = lb->entries[i];
        lb->entries[i] = lb->entries[j];
        lb->entries[j] = temp;
      }
    }
  }

  // Update ranks
  for (int i = 0; i < lb->entry_count; i++) {
    lb->entries[i].rank = i + 1;
  }

  return true;
}

// ============================================================================
// Get player rank in segment
// ============================================================================
static inline uint16_t leaderboard_get_player_rank(const Leaderboard *lb, uint32_t player_id) {
  for (int i = 0; i < lb->entry_count; i++) {
    if (lb->entries[i].player_id == player_id) {
      return lb->entries[i].rank;
    }
  }
  return 0;  // Not found
}

// ============================================================================
// Get top N entries from leaderboard
// ============================================================================
static inline uint16_t leaderboard_get_top_entries(const Leaderboard *lb, uint16_t count,
                                                    LeaderboardEntry *out_entries) {
  uint16_t to_get = (count > lb->entry_count) ? lb->entry_count : count;
  for (int i = 0; i < to_get; i++) {
    out_entries[i] = lb->entries[i];
  }
  return to_get;
}

// ============================================================================
// Get percentile rank (what % of players are below this score)
// ============================================================================
static inline uint8_t leaderboard_get_percentile(const Leaderboard *lb, uint32_t player_id) {
  uint16_t rank = leaderboard_get_player_rank(lb, player_id);
  if (rank == 0 || lb->entry_count == 0) return 0;

  // Percentile = (entry_count - rank) / entry_count * 100
  uint8_t percentile = ((lb->entry_count - rank) * 100) / lb->entry_count;
  return percentile;
}

// ============================================================================
// Reset leaderboard (weekly reset)
// ============================================================================
static inline void leaderboard_reset(Leaderboard *lb) {
  lb->entry_count = 0;
  lb->last_reset = time(NULL);
}

// ============================================================================
// Get time until next reset (weekly leaderboard)
// ============================================================================
static inline uint32_t leaderboard_get_time_to_reset(const Leaderboard *lb) {
  uint32_t now = time(NULL);
  uint32_t age = now - lb->last_reset;
  uint32_t week_seconds = 7 * 24 * 3600;

  if (age >= week_seconds) {
    return 0;  // Should reset now
  }

  return week_seconds - age;
}

// ============================================================================
// Get segment name
// ============================================================================
static inline const char *leaderboard_get_segment_name(uint8_t segment) {
  switch (segment) {
    case SEGMENT_BEGINNER:
      return "Beginner Trainers (Levels 1-2)";
    case SEGMENT_INTERMEDIATE:
      return "Intermediate Trainers (Levels 3-4)";
    case SEGMENT_ADVANCED:
      return "Advanced Trainers (Levels 5-6)";
    case SEGMENT_MASTER:
      return "Master Trainers (Levels 7-8)";
    default:
      return "Unknown";
  }
}

// ============================================================================
// Get rank tier name (for badge display)
// ============================================================================
static inline const char *leaderboard_get_tier_name(uint16_t rank) {
  if (rank <= 10) return "Top 10";
  if (rank <= 50) return "Top 50";
  if (rank <= 100) return "Top 100";
  return "Ranked";
}

// ============================================================================
// Get rank badge color (RGB565)
// ============================================================================
static inline uint16_t leaderboard_get_rank_color(uint16_t rank) {
  if (rank <= 3) return 0xFFC0;   // Gold/Yellow: Top 3
  if (rank <= 10) return 0xAAAD;  // Silver: Top 10
  if (rank <= 50) return 0xB5B2;  // Bronze: Top 50
  return 0x8410;                  // Gray: Other
}

// ============================================================================
// Compare two players for ranking (for sorting)
// ============================================================================
static inline int leaderboard_compare_entries(const LeaderboardEntry *a,
                                              const LeaderboardEntry *b) {
  // Primary: score (descending)
  if (a->score > b->score) return -1;
  if (a->score < b->score) return 1;

  // Secondary: secondary_score (descending)
  if (a->secondary_score > b->secondary_score) return -1;
  if (a->secondary_score < b->secondary_score) return 1;

  // Tertiary: update time (newer first)
  if (a->updated_at > b->updated_at) return -1;
  if (a->updated_at < b->updated_at) return 1;

  return 0;
}

// ============================================================================
// Get player's rank tier (for progress display)
// ============================================================================
static inline uint8_t leaderboard_get_rank_tier(uint16_t rank) {
  if (rank == 0) return 0;        // Not ranked
  if (rank <= 3) return 5;        // Gold tier
  if (rank <= 10) return 4;       // Silver tier
  if (rank <= 50) return 3;       // Bronze tier
  if (rank <= 100) return 2;      // Ranked tier
  return 1;                        // Participant tier
}

// ============================================================================
// Serialize leaderboard for storage
// ============================================================================
static inline void leaderboard_serialize(const Leaderboard *lb, uint8_t *buffer) {
  buffer[0] = lb->segment;
  buffer[1] = (uint8_t)lb->entry_count;
  uint32_t *p = (uint32_t *)(buffer + 2);
  p[0] = lb->last_reset;
  // Full serialization would include all entries
}

// ============================================================================
// Deserialize leaderboard from storage
// ============================================================================
static inline void leaderboard_deserialize(Leaderboard *lb, const uint8_t *buffer) {
  lb->segment = buffer[0];
  lb->entry_count = buffer[1];
  const uint32_t *p = (const uint32_t *)(buffer + 2);
  lb->last_reset = p[0];
}

// ============================================================================
// Initialize player leaderboard stats
// ============================================================================
static inline void leaderboard_player_stats_init(PlayerLeaderboardStats *stats) {
  stats->segment = SEGMENT_BEGINNER;
  stats->current_rank = 0;
  stats->current_score = 0;
  stats->rank_change = 0;
  stats->percentile = 0;
}

// ============================================================================
// Update player stats from leaderboard
// ============================================================================
static inline void leaderboard_player_stats_update(PlayerLeaderboardStats *stats,
                                                    const Leaderboard *lb, uint32_t player_id) {
  stats->segment = lb->segment;
  stats->current_rank = leaderboard_get_player_rank(lb, player_id);
  stats->percentile = leaderboard_get_percentile(lb, player_id);

  // Find player entry for score
  for (int i = 0; i < lb->entry_count; i++) {
    if (lb->entries[i].player_id == player_id) {
      stats->current_score = lb->entries[i].score;
      break;
    }
  }
}
