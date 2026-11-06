#pragma once

// ============================================================================
// GUILD/CLAN SYSTEM - Phase 3 Community Improvement
// ============================================================================
// Cooperative teams (max 50 players) that work toward shared goals
// Research shows guild members have 3x higher retention
// Creates sense of belonging (relatedness need from self-determination theory)

#include <stdint.h>
#include <time.h>
#include <string.h>

#define GUILD_MAX_MEMBERS 50
#define GUILD_MAX_NAME_LEN 32
#define GUILD_MAX_DESC_LEN 128
#define MAX_GUILDS 1000  // System-wide limit

// Guild member roles
enum GuildRole {
  ROLE_MEMBER = 0,
  ROLE_OFFICER = 1,
  ROLE_LEADER = 2,
};

// Guild achievement levels
enum GuildLevel {
  GUILD_LEVEL_1 = 1,
  GUILD_LEVEL_2 = 2,
  GUILD_LEVEL_3 = 3,
  GUILD_LEVEL_4 = 4,
  GUILD_LEVEL_5 = 5,
};

struct GuildMember {
  uint32_t player_id;
  char name[32];
  uint8_t role;           // ROLE_MEMBER, ROLE_OFFICER, ROLE_LEADER
  uint16_t contribution;  // Guild goal contribution (catches, coins, etc.)
  uint32_t joined_at;     // Unix timestamp
};

struct GuildGoal {
  uint8_t animal_habitat;     // Which habitat to catch from (0-10)
  uint16_t target_count;      // Goal: catch N animals
  uint16_t current_count;     // Progress toward goal
  uint32_t week_started;      // Unix timestamp of week start
  bool completed;             // Goal completed this week?
};

struct GuildAchievement {
  uint8_t id;
  const char *name;
  const char *description;
  uint16_t requirement;       // What's needed to unlock
  uint8_t requirement_type;   // 0=level, 1=members, 2=weeks_active
};

struct Guild {
  uint32_t guild_id;
  char name[GUILD_MAX_NAME_LEN];
  char description[GUILD_MAX_DESC_LEN];
  uint8_t level;              // 1-5
  uint8_t member_count;
  uint32_t created_at;
  uint32_t leader_id;

  // Current week's goal
  GuildGoal weekly_goal;

  // Members list (simplified for this implementation)
  // Full implementation would have separate member storage
  GuildMember members[GUILD_MAX_MEMBERS];

  // Unlocked achievements
  bool achievements_unlocked[10];
};

struct GuildState {
  uint32_t player_guild_id;     // 0 = not in guild
  uint8_t player_role;          // Role in guild
  uint16_t player_contribution; // Individual contribution to goal
};

// ============================================================================
// Initialize guild system
// ============================================================================
static inline void guild_system_init(Guild *guild) {
  guild->guild_id = 0;
  guild->name[0] = '\0';
  guild->description[0] = '\0';
  guild->level = 1;
  guild->member_count = 0;
  guild->created_at = time(NULL);
  guild->leader_id = 0;
  guild->weekly_goal.target_count = 50;
  guild->weekly_goal.current_count = 0;
  guild->weekly_goal.completed = false;
  for (int i = 0; i < GUILD_MAX_MEMBERS; i++) {
    guild->members[i].player_id = 0;
  }
  for (int i = 0; i < 10; i++) {
    guild->achievements_unlocked[i] = false;
  }
}

// ============================================================================
// Create a new guild
// ============================================================================
static inline bool guild_create(Guild *guild, uint32_t leader_id,
                                const char *name, const char *description) {
  if (guild->guild_id != 0) return false;  // Already in guild
  if (strlen(name) == 0 || strlen(name) > GUILD_MAX_NAME_LEN - 1) return false;

  guild->guild_id = leader_id;  // Simplified: use leader_id as guild_id
  guild->leader_id = leader_id;
  strncpy(guild->name, name, GUILD_MAX_NAME_LEN - 1);
  strncpy(guild->description, description, GUILD_MAX_DESC_LEN - 1);
  guild->level = 1;
  guild->created_at = time(NULL);
  guild->member_count = 1;

  // Add leader as first member
  guild->members[0].player_id = leader_id;
  guild->members[0].role = ROLE_LEADER;

  return true;
}

// ============================================================================
// Add member to guild
// ============================================================================
static inline bool guild_add_member(Guild *guild, uint32_t player_id, const char *player_name) {
  if (guild->member_count >= GUILD_MAX_MEMBERS) return false;  // Full
  if (guild->guild_id == 0) return false;  // Guild doesn't exist

  // Check if already member
  for (int i = 0; i < guild->member_count; i++) {
    if (guild->members[i].player_id == player_id) return false;
  }

  guild->members[guild->member_count].player_id = player_id;
  strncpy(guild->members[guild->member_count].name, player_name, 31);
  guild->members[guild->member_count].role = ROLE_MEMBER;
  guild->members[guild->member_count].joined_at = time(NULL);
  guild->members[guild->member_count].contribution = 0;
  guild->member_count++;

  return true;
}

// ============================================================================
// Remove member from guild
// ============================================================================
static inline bool guild_remove_member(Guild *guild, uint32_t player_id) {
  if (guild->guild_id == 0) return false;

  for (int i = 0; i < guild->member_count; i++) {
    if (guild->members[i].player_id == player_id) {
      // Can't remove leader (demote first)
      if (guild->members[i].role == ROLE_LEADER) return false;

      // Shift remaining members
      for (int j = i; j < guild->member_count - 1; j++) {
        guild->members[j] = guild->members[j + 1];
      }
      guild->member_count--;
      return true;
    }
  }
  return false;
}

// ============================================================================
// Get member role
// ============================================================================
static inline uint8_t guild_get_member_role(const Guild *guild, uint32_t player_id) {
  for (int i = 0; i < guild->member_count; i++) {
    if (guild->members[i].player_id == player_id) {
      return guild->members[i].role;
    }
  }
  return 0xFF;  // Not found
}

// ============================================================================
// Add contribution to weekly goal (called when player catches animal)
// ============================================================================
static inline void guild_add_contribution(Guild *guild, uint32_t player_id, uint16_t amount) {
  // Add to guild goal
  if (guild->weekly_goal.current_count < guild->weekly_goal.target_count) {
    guild->weekly_goal.current_count += amount;
    if (guild->weekly_goal.current_count >= guild->weekly_goal.target_count) {
      guild->weekly_goal.completed = true;
    }
  }

  // Add to player's personal contribution (for tracking)
  for (int i = 0; i < guild->member_count; i++) {
    if (guild->members[i].player_id == player_id) {
      guild->members[i].contribution += amount;
      break;
    }
  }
}

// ============================================================================
// Get weekly goal progress (0-100%)
// ============================================================================
static inline uint8_t guild_get_goal_progress_percent(const Guild *guild) {
  if (guild->weekly_goal.target_count == 0) return 100;
  uint8_t percent = (guild->weekly_goal.current_count * 100) / guild->weekly_goal.target_count;
  if (percent > 100) percent = 100;
  return percent;
}

// ============================================================================
// Complete week and reward members
// Returns: bonus percentage for next week (50% boost if goal met)
// ============================================================================
static inline uint8_t guild_complete_week(Guild *guild) {
  uint8_t bonus = 100;  // Base: 100% = 1.0x

  if (guild->weekly_goal.completed) {
    // Goal was met - reward all members
    bonus = 150;  // 1.5x token/coin multiplier for next week

    // Check for level-up
    uint32_t weeks_active = (time(NULL) - guild->created_at) / (7 * 24 * 3600);
    if (weeks_active > 0 && weeks_active % 5 == 0) {
      if (guild->level < 5) guild->level++;
    }
  }

  // Reset for next week
  guild->weekly_goal.current_count = 0;
  guild->weekly_goal.completed = false;
  guild->weekly_goal.week_started = time(NULL);

  // Randomize next habitat
  guild->weekly_goal.animal_habitat = (guild->weekly_goal.animal_habitat + 1) % 11;

  return bonus;
}

// ============================================================================
// Get guild level name
// ============================================================================
static inline const char *guild_get_level_name(uint8_t level) {
  switch (level) {
    case 1: return "Novice Guild";
    case 2: return "Rising Guild";
    case 3: return "Established Guild";
    case 4: return "Master Guild";
    case 5: return "Legendary Guild";
    default: return "Unknown";
  }
}

// ============================================================================
// Get catch rate bonus from guild level
// ============================================================================
static inline uint8_t guild_get_catch_bonus_percent(uint8_t guild_level) {
  // Each guild level gives +2% catch rate to all members
  return (guild_level - 1) * 2;
}

// ============================================================================
// Check if player can perform action based on role
// ============================================================================
static inline bool guild_can_player_action(uint8_t player_role, uint8_t action) {
  // action: 0=invite, 1=remove, 2=set_goal, 3=disband
  switch (action) {
    case 0:  // Invite
      return player_role >= ROLE_OFFICER;
    case 1:  // Remove
      return player_role >= ROLE_OFFICER;
    case 2:  // Set goal
      return player_role >= ROLE_LEADER;
    case 3:  // Disband
      return player_role == ROLE_LEADER;
    default:
      return false;
  }
}

// ============================================================================
// Get member count by role
// ============================================================================
static inline uint8_t guild_get_role_count(const Guild *guild, uint8_t role) {
  uint8_t count = 0;
  for (int i = 0; i < guild->member_count; i++) {
    if (guild->members[i].role == role) count++;
  }
  return count;
}

// ============================================================================
// Get average player level in guild (simplified)
// ============================================================================
static inline uint8_t guild_get_average_level(const Guild *guild) {
  // In real implementation, would track player levels
  // For now, return guild level
  return guild->level;
}

// ============================================================================
// Serialize guild for storage
// ============================================================================
static inline void guild_serialize(const Guild *guild, uint8_t *buffer, uint32_t buffer_size) {
  // Simplified: store only guild ID and name
  // Full implementation would serialize members and goals
  uint32_t *p = (uint32_t *)buffer;
  p[0] = guild->guild_id;
  p[1] = guild->level;
  memcpy(buffer + 8, guild->name, GUILD_MAX_NAME_LEN);
}

// ============================================================================
// Deserialize guild from storage
// ============================================================================
static inline void guild_deserialize(Guild *guild, const uint8_t *buffer) {
  const uint32_t *p = (const uint32_t *)buffer;
  guild->guild_id = p[0];
  guild->level = p[1];
  memcpy(guild->name, buffer + 8, GUILD_MAX_NAME_LEN);
}

// ============================================================================
// Player guild state initialization
// ============================================================================
static inline void guild_player_state_init(GuildState *state) {
  state->player_guild_id = 0;
  state->player_role = ROLE_MEMBER;
  state->player_contribution = 0;
}

// ============================================================================
// Join guild
// ============================================================================
static inline bool guild_player_join(GuildState *state, uint32_t guild_id, uint8_t role) {
  if (state->player_guild_id != 0) return false;  // Already in guild

  state->player_guild_id = guild_id;
  state->player_role = role;
  state->player_contribution = 0;
  return true;
}

// ============================================================================
// Leave guild
// ============================================================================
static inline bool guild_player_leave(GuildState *state) {
  if (state->player_guild_id == 0) return false;

  state->player_guild_id = 0;
  state->player_role = ROLE_MEMBER;
  state->player_contribution = 0;
  return true;
}

// ============================================================================
// Get role name
// ============================================================================
static inline const char *guild_get_role_name(uint8_t role) {
  switch (role) {
    case ROLE_MEMBER:
      return "Member";
    case ROLE_OFFICER:
      return "Officer";
    case ROLE_LEADER:
      return "Guild Leader";
    default:
      return "Unknown";
  }
}
