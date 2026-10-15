/**
 * @file config.h
 * @brief Every tunable value of the project in ONE place: pins, servo travel, timing, battery.
 *
 * Read this file first. Nothing else in the firmware contains "magic numbers".
 * Pin numbers are GPIO numbers (the silkscreen on the board prints the GPIO number).
 */
#pragma once
#include <Arduino.h>

// ---------------------------------------------------------------------------
// Build variant. Exactly one of BUILD_BENCH / BUILD_WEARABLE is defined by
// platformio.ini. Phase 1 (bench) and Phase 2 (wearable) differ only in:
//   - what the "trigger" input is (push-button vs TTP223 touch module)
//   - whether the battery voltage is measured and a low-battery cutoff is enforced
// ---------------------------------------------------------------------------
#if defined(BUILD_WEARABLE)
  constexpr bool    HAS_BATTERY_SENSE = true;   // 100k/100k divider wired to PIN_VBAT
  constexpr bool    TOUCH_ACTIVE_HIGH = true;   // TTP223 drives its output HIGH when touched
  constexpr bool    TOUCH_USE_PULLUP  = false;  // the module drives the line itself
  constexpr const char* BUILD_TAG     = "WEAR";
#else  // BUILD_BENCH (default)
  constexpr bool    HAS_BATTERY_SENSE = false;  // USB powered, nothing to monitor
  constexpr bool    TOUCH_ACTIVE_HIGH = false;  // push-button between pin and GND
  constexpr bool    TOUCH_USE_PULLUP  = true;   // internal pull-up keeps the line HIGH when idle
  constexpr const char* BUILD_TAG     = "BENCH";
#endif

// ---------------------------------------------------------------------------
// Pins (ESP32-C3 OLED board). GPIO5 / GPIO6 are used by the on-board OLED (I2C).
// GPIO2, GPIO8, GPIO9 are boot "strapping" pins -> deliberately avoided.
// GPIO0..GPIO4 are ADC1-capable; GPIO0 is used for the battery divider.
// Run `pio run -e hwcheck -t upload` to confirm these on YOUR board.
// ---------------------------------------------------------------------------
constexpr uint8_t PIN_OLED_SDA = 5;   // on-board, do not use for anything else
constexpr uint8_t PIN_OLED_SCL = 6;   // on-board, do not use for anything else
constexpr uint8_t PIN_SERVO    = 3;   // servo signal (orange/yellow wire)
constexpr uint8_t PIN_TOUCH    = 4;   // push-button (bench) or TTP223 OUT (wearable)
constexpr uint8_t PIN_VBAT     = 0;   // battery divider midpoint (ADC1_CH0)

// ---------------------------------------------------------------------------
// Servo
// ---------------------------------------------------------------------------
constexpr int      SERVO_MIN_US    = 500;    // pulse width at 0 degrees (microseconds)
constexpr int      SERVO_MAX_US    = 2400;   // pulse width at 180 degrees
constexpr float    ANGLE_OPEN_DEG  = 0.0f;   // wings open  (also the rest position)
constexpr float    ANGLE_CLOSED_DEG= 90.0f;  // wings closed. 1:1 gears -> wings sweep the same angle (90 = folded).
constexpr uint32_t FLAP_MS         = 1500;   // time for ONE half-flap (open->closed)

// ---------------------------------------------------------------------------
// Touch / button
// ---------------------------------------------------------------------------
constexpr uint32_t DEBOUNCE_MS     = 40;     // ignore contact bounce shorter than this

// ---------------------------------------------------------------------------
// Battery monitoring (wearable build)
// ---------------------------------------------------------------------------
constexpr float    VBAT_DIVIDER    = 2.0f;   // 100k + 100k -> pin sees half the battery voltage
constexpr float    VBAT_CAL        = 1.00f;  // multiply to calibrate against a multimeter
constexpr uint32_t VBAT_PERIOD_MS  = 500;    // how often to sample
constexpr uint16_t VBAT_CUTOFF_MV  = 3300;   // stop wings below this. Use 3500 if you skip the boost.
constexpr uint8_t  VBAT_LOW_COUNT  = 6;      // consecutive low samples required (rejects servo sag)
constexpr uint32_t DISPLAY_PERIOD_MS = 200;  // OLED refresh (5 Hz is plenty)
