#pragma once

// ============================================================================
// NVS PERSISTENCE SYSTEM - Phase 4
// ============================================================================
// Save/load all game state to flash storage (survives reboot)

#include <stdint.h>
#include <nvs_flash.h>

#define NVS_NAMESPACE "tamanimals"
#define NVS_KEY_VERSION "version"
#define NVS_KEY_PLAYER_NAME "pname"
#define NVS_KEY_LAST_SAVE "lastsave"

// Namespace keys for each system
#define NVS_CATCHING "catching"
#define NVS_DAILY_REWARDS "daily_rewards"
#define NVS_ACHIEVEMENTS "achievements"
#define NVS_STEP_COUNTER "step_counter"
#define NVS_PET_EVOLUTION "pet_evo"
#define NVS_PET_MOOD "pet_mood"
#define NVS_ACHIEVEMENTS_V2 "achv2"
#define NVS_FACTS_LEARNED "facts"
#define NVS_GUILD "guild"
#define NVS_LEADERBOARD "leaderboard"
#define NVS_WEEKLY_CHALLENGES "weekly_ch"
#define NVS_COLLECTION_TIERS "coll_tiers"
#define NVS_BATTLE_PASS "battle_pass"
#define NVS_HABITAT_UNLOCK "habitat"
#define NVS_SAFEGUARDS "safeguards"
#define NVS_COSMETICS "cosmetics"
#define NVS_PET_STATE_CTX "pet_state"
#define NVS_PET_STATE_TIME "pet_state_time"

struct NVSPersistenceState {
  bool initialized;
  uint32_t last_save_time;
  uint8_t version;
  char player_name[32];
};

// ============================================================================
// Initialize NVS
// ============================================================================
static inline bool nvs_persistence_init(NVSPersistenceState *state) {
  esp_err_t err = nvs_flash_init();

  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    nvs_flash_erase();
    err = nvs_flash_init();
  }

  if (err != ESP_OK) {
    Serial.println("NVS init failed!");
    return false;
  }

  state->initialized = true;
  state->version = 1;
  return true;
}

// ============================================================================
// Save game state
// ============================================================================
static inline bool nvs_persistence_save_all(const NVSPersistenceState *state,
                                            const uint8_t *catching_data, uint16_t catching_size,
                                            const uint8_t *achievements_data, uint16_t ach_size,
                                            const uint8_t *facts_data, uint16_t facts_size,
                                            const uint8_t *cosmetics_data, uint16_t cos_size,
                                            const void *pet_state_ctx, uint16_t pet_state_size) {
  if (!state->initialized) return false;

  nvs_handle_t handle;
  esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
  if (err != ESP_OK) return false;

  // Save version and timestamp
  nvs_set_u8(handle, NVS_KEY_VERSION, state->version);
  nvs_set_u32(handle, NVS_KEY_LAST_SAVE, time(NULL));

  // Save all systems
  if (catching_data) nvs_set_blob(handle, NVS_CATCHING, catching_data, catching_size);
  if (achievements_data) nvs_set_blob(handle, NVS_ACHIEVEMENTS_V2, achievements_data, ach_size);
  if (facts_data) nvs_set_blob(handle, NVS_FACTS_LEARNED, facts_data, facts_size);
  if (cosmetics_data) nvs_set_blob(handle, NVS_COSMETICS, cosmetics_data, cos_size);

  // Save pet sprite animation state (Phase 4)
  if (pet_state_ctx && pet_state_size > 0) {
    nvs_set_blob(handle, NVS_PET_STATE_CTX, pet_state_ctx, pet_state_size);
    nvs_set_u32(handle, NVS_PET_STATE_TIME, time(NULL));
  }

  err = nvs_commit(handle);
  nvs_close(handle);

  return err == ESP_OK;
}

// ============================================================================
// Load game state
// ============================================================================
static inline bool nvs_persistence_load_all(NVSPersistenceState *state,
                                            uint8_t *catching_data, uint16_t *catching_size,
                                            uint8_t *achievements_data, uint16_t *ach_size,
                                            uint8_t *facts_data, uint16_t *facts_size,
                                            uint8_t *cosmetics_data, uint16_t *cos_size,
                                            void *pet_state_ctx, uint16_t pet_state_size) {
  if (!state->initialized) return false;

  nvs_handle_t handle;
  esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
  if (err != ESP_OK) return false;

  // Load version and timestamp
  nvs_get_u8(handle, NVS_KEY_VERSION, &state->version);
  uint32_t save_time = 0;
  nvs_get_u32(handle, NVS_KEY_LAST_SAVE, &save_time);
  state->last_save_time = save_time;

  // Load all systems
  if (catching_data && catching_size) {
    nvs_get_blob(handle, NVS_CATCHING, catching_data, catching_size);
  }
  if (achievements_data && ach_size) {
    nvs_get_blob(handle, NVS_ACHIEVEMENTS_V2, achievements_data, ach_size);
  }
  if (facts_data && facts_size) {
    nvs_get_blob(handle, NVS_FACTS_LEARNED, facts_data, facts_size);
  }
  if (cosmetics_data && cos_size) {
    nvs_get_blob(handle, NVS_COSMETICS, cosmetics_data, cos_size);
  }

  // Load pet sprite animation state (Phase 4)
  if (pet_state_ctx && pet_state_size > 0) {
    uint32_t retrieved_size = pet_state_size;
    esp_err_t err_state = nvs_get_blob(handle, NVS_PET_STATE_CTX, pet_state_ctx, &retrieved_size);
    if (err_state != ESP_OK) {
      // State not found - initialize to default (will be set by pet_new)
      Serial.println("Pet state not found in NVS (first run)");
    } else {
      Serial.println("✅ Pet state restored from NVS");
    }
  }

  nvs_close(handle);
  return err == ESP_OK;
}

// ============================================================================
// Save single system
// ============================================================================
static inline bool nvs_persistence_save_system(const char *key, const uint8_t *data, size_t size) {
  nvs_handle_t handle;
  esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
  if (err != ESP_OK) return false;

  err = nvs_set_blob(handle, key, data, size);
  if (err != ESP_OK) {
    nvs_close(handle);
    return false;
  }

  err = nvs_commit(handle);
  nvs_close(handle);
  return err == ESP_OK;
}

// ============================================================================
// Load single system
// ============================================================================
static inline bool nvs_persistence_load_system(const char *key, uint8_t *data, size_t *size) {
  nvs_handle_t handle;
  esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
  if (err != ESP_OK) return false;

  size_t required_size = *size;
  err = nvs_get_blob(handle, key, data, &required_size);

  if (err == ESP_ERR_NVS_NOT_FOUND) {
    nvs_close(handle);
    return false;
  }

  *size = required_size;
  nvs_close(handle);
  return err == ESP_OK;
}

// ============================================================================
// Clear all save data (factory reset)
// ============================================================================
static inline bool nvs_persistence_factory_reset() {
  nvs_handle_t handle;
  esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
  if (err != ESP_OK) return false;

  err = nvs_erase_all(handle);
  if (err != ESP_OK) {
    nvs_close(handle);
    return false;
  }

  err = nvs_commit(handle);
  nvs_close(handle);
  return err == ESP_OK;
}

// ============================================================================
// Get last save time
// ============================================================================
static inline uint32_t nvs_persistence_get_last_save(const NVSPersistenceState *state) {
  return state->last_save_time;
}

// ============================================================================
// Check if save exists
// ============================================================================
static inline bool nvs_persistence_has_save() {
  nvs_handle_t handle;
  esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
  if (err != ESP_OK) return false;

  uint32_t save_time = 0;
  err = nvs_get_u32(handle, NVS_KEY_LAST_SAVE, &save_time);
  nvs_close(handle);

  return err == ESP_OK && save_time > 0;
}

// ============================================================================
// Get save info string
// ============================================================================
static inline void nvs_persistence_get_save_info(char *buffer, size_t size) {
  nvs_handle_t handle;
  if (nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle) != ESP_OK) {
    snprintf(buffer, size, "No save found");
    return;
  }

  uint32_t save_time = 0;
  nvs_get_u32(handle, NVS_KEY_LAST_SAVE, &save_time);
  nvs_close(handle);

  time_t t = (time_t)save_time;
  struct tm *timeinfo = localtime(&t);
  strftime(buffer, size, "Last save: %Y-%m-%d %H:%M:%S", timeinfo);
}
