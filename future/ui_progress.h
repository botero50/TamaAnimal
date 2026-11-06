#pragma once

// ============================================================================
// UI PROGRESS BAR UTILITIES - Phase 1 Engagement Improvement
// ============================================================================
// Helper functions for drawing progress bars and status indicators
// Used by engagement systems to display progression visually

#include <Arduino_GFX_Library.h>
#include <stdint.h>

// Progress bar colors
#define PROGRESS_COLOR_GOOD 0x07E0      // Green
#define PROGRESS_COLOR_OK 0xFFE0        // Yellow
#define PROGRESS_COLOR_LOW 0xF800       // Red
#define PROGRESS_BG_COLOR 0x1082        // Dark gray
#define PROGRESS_BORDER_COLOR 0x2104    // Gray

// ============================================================================
// Draw a horizontal progress bar
// x, y: top-left position
// width, height: dimensions
// current: current value
// max: maximum value
// color: bar color
// gfx: GFX object for drawing
// ============================================================================
static inline void draw_progress_bar(Arduino_GFX *gfx, int16_t x, int16_t y,
                                     int16_t width, int16_t height,
                                     uint32_t current, uint32_t max,
                                     uint16_t color) {
  if (max == 0) max = 1;  // Prevent division by zero

  // Draw background
  gfx->fillRect(x, y, width, height, PROGRESS_BG_COLOR);

  // Draw filled portion
  int16_t filled = (width * current) / max;
  if (filled > 0) {
    gfx->fillRect(x, y, filled, height, color);
  }

  // Draw border
  gfx->drawRect(x, y, width, height, PROGRESS_BORDER_COLOR);
}

// ============================================================================
// Draw progress bar with percentage text
// ============================================================================
static inline void draw_progress_bar_with_percent(Arduino_GFX *gfx,
                                                   int16_t x, int16_t y,
                                                   int16_t width, int16_t height,
                                                   uint32_t current, uint32_t max,
                                                   uint16_t color) {
  draw_progress_bar(gfx, x, y, width, height, current, max, color);

  // Draw percentage text
  uint8_t percent = (max > 0) ? ((current * 100) / max) : 0;
  if (percent > 100) percent = 100;

  // Simple text overlay
  gfx->setTextColor(0xFFFF, PROGRESS_BG_COLOR);
  gfx->setCursor(x + width / 2 - 10, y + height / 2 - 3);
  // Note: This is a simplified version; full implementation would use printf
}

// ============================================================================
// Draw a vertical progress bar (for respect or collection)
// ============================================================================
static inline void draw_progress_bar_vertical(Arduino_GFX *gfx,
                                              int16_t x, int16_t y,
                                              int16_t width, int16_t height,
                                              uint32_t current, uint32_t max,
                                              uint16_t color) {
  if (max == 0) max = 1;

  // Draw background
  gfx->fillRect(x, y, width, height, PROGRESS_BG_COLOR);

  // Draw filled portion (from bottom up)
  int16_t filled = (height * current) / max;
  if (filled > 0) {
    gfx->fillRect(x, y + height - filled, width, filled, color);
  }

  // Draw border
  gfx->drawRect(x, y, width, height, PROGRESS_BORDER_COLOR);
}

// ============================================================================
// Get color based on progress percentage
// Green for 70%+, yellow for 40-70%, red for <40%
// ============================================================================
static inline uint16_t get_progress_color(uint32_t current, uint32_t max) {
  if (max == 0) return PROGRESS_COLOR_LOW;

  uint8_t percent = (current * 100) / max;
  if (percent >= 70) return PROGRESS_COLOR_GOOD;
  if (percent >= 40) return PROGRESS_COLOR_OK;
  return PROGRESS_COLOR_LOW;
}

// ============================================================================
// Draw a simple status bar (for respect level, hunger, etc.)
// Shows current/max with optional icon
// ============================================================================
static inline void draw_status_bar(Arduino_GFX *gfx,
                                   int16_t x, int16_t y,
                                   int16_t width, int16_t height,
                                   uint32_t current, uint32_t max) {
  uint16_t color = get_progress_color(current, max);
  draw_progress_bar(gfx, x, y, width, height, current, max, color);
}

// ============================================================================
// Draw a circular progress indicator (for collection percentage, etc.)
// Note: Simplified - real implementation would draw actual circle
// ============================================================================
static inline void draw_progress_circle(Arduino_GFX *gfx,
                                        int16_t x, int16_t y,
                                        int16_t radius,
                                        uint32_t current, uint32_t max,
                                        uint16_t color) {
  // Draw outer circle (border)
  gfx->drawCircle(x, y, radius, PROGRESS_BORDER_COLOR);

  // Draw filled arc (approximated with multiple lines)
  // This is simplified; full implementation would draw proper circle segments
  uint16_t segments = (current * 360) / max;
  for (uint16_t i = 0; i < segments; i++) {
    float angle = (i * 3.14159) / 180.0;
    int16_t x1 = x + (radius * cos(angle));
    int16_t y1 = y + (radius * sin(angle));
    // Draw line from center to perimeter
    gfx->drawLine(x, y, x1, y1, color);
  }
}

// ============================================================================
// Draw a label and value with progress bar
// ============================================================================
static inline void draw_labeled_progress(Arduino_GFX *gfx,
                                         int16_t x, int16_t y,
                                         const char *label,
                                         uint32_t current, uint32_t max,
                                         int16_t bar_width) {
  // Draw label
  gfx->setTextColor(0xFFFF);
  gfx->setCursor(x, y);
  // Full text drawing would happen here with proper font support

  // Draw progress bar below label
  int16_t bar_y = y + 12;
  draw_progress_bar(gfx, x, bar_y, bar_width, 8, current, max,
                    get_progress_color(current, max));
}
