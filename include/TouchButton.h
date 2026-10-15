/**
 * @file TouchButton.h
 * @brief Debounced edge detector for a push-button or a TTP223 touch module.
 *
 * CONCEPT - debouncing: a mechanical contact "chatters" for a few milliseconds when pressed,
 * producing a burst of HIGH/LOW changes. We accept a new level only after it has stayed
 * unchanged for DEBOUNCE_MS, then report a single "pressed" event on the idle->active edge.
 * (The TTP223 does not bounce, but the same code keeps the two builds identical.)
 */
#pragma once
#include <Arduino.h>
#include "config.h"

class TouchButton {
 public:
  void begin(uint8_t pin) {
    pin_ = pin;
    pinMode(pin_, TOUCH_USE_PULLUP ? INPUT_PULLUP : INPUT);
    stable_ = last_raw_ = readActive();
    changed_at_ = millis();
  }

  /// Call every loop(). Returns true exactly once per press (rising edge of "active").
  bool pressed(uint32_t now) {
    bool raw = readActive();
    if (raw != last_raw_) {          // level changed: restart the stability timer
      last_raw_ = raw;
      changed_at_ = now;
    }
    if (raw != stable_ && (now - changed_at_) >= DEBOUNCE_MS) {
      stable_ = raw;                 // level has been steady long enough: accept it
      return stable_;                // true only on the idle -> active transition
    }
    return false;
  }

 private:
  /// Normalises both wiring styles to "true = pressed/touched".
  bool readActive() const { return (digitalRead(pin_) == HIGH) == TOUCH_ACTIVE_HIGH; }

  uint8_t pin_ = 0;
  bool stable_ = false, last_raw_ = false;
  uint32_t changed_at_ = 0;
};
