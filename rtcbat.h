#pragma once
#include <Arduino.h>

// RTC PCF85063: time persists as long as the board has power
bool rtcBegin();
uint32_t rtcEpoch();             // unix seconds; 0 if the RTC is not valid
void rtcSetEpoch(uint32_t e);

// Which board this is. The 1.75C has 32 MB of flash where the plain 1.75 has 16 MB, and the flash
// size can be read before any pin is touched -- which matters, because on the 1.75C the plain
// board's microSD pins reset the display.
bool boardIs175C();

// PMU AXP2101: battery state
bool batBegin();
void pmuEnablePanel();           // turns on BLDO1 (OLED VDD 3.3V); call before gfx->begin()
int batSmoothMv();                // the smoothed battery voltage in mV, 0 if unknown
int batPercent();                // 0-100, -1 if no battery is connected
bool batCharging();
bool usbPresent();
bool batShouldShutdown();        // on battery, the smoothed voltage has stayed under the cut-off (see batcurve.h)
void batPowerOff();              // the PMU turns every rail off (save the pet first)

// AXP2101 PWR button: long press 4s = physical power-off (RTC stays alive);
// the short press is captured by the firmware (screen on/off)
void pwrSetup();
bool pwrShortPressed();  // poll in the loop
