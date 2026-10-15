/**
 * @file Battery.h
 * @brief Battery voltage, percentage and a latched low-battery flag.
 *
 * CONCEPT - voltage divider: the ADC pin tolerates ~2.5 V but a LiPo reaches 4.2 V, so two
 * equal resistors (100k + 100k) halve it:  Vpin = Vbat * R2/(R1+R2) = Vbat / 2.
 *
 * CONCEPT - smoothing + hysteresis: the servo makes the cell voltage sag for a moment.
 * We (1) low-pass filter the readings and (2) only declare "low" after several consecutive
 * low samples. Once declared, the flag is LATCHED until reboot: a cell that recovers when
 * unloaded would otherwise restart the servo and drop again (oscillation).
 */
#pragma once
#include <Arduino.h>
#include "config.h"

class Battery {
 public:
  void begin(uint8_t pin) {
    pin_ = pin;
    analogSetPinAttenuation(pin_, ADC_11db);   // full-scale ~2.5 V, covers the halved 4.2 V
    mv_ = sampleMv();
  }

  void update(uint32_t now) {
    if (now - last_ms_ < VBAT_PERIOD_MS) return;
    last_ms_ = now;
    mv_ = 0.8f * mv_ + 0.2f * sampleMv();      // exponential moving average
    low_count_ = (mv_ < VBAT_CUTOFF_MV) ? low_count_ + 1 : 0;
    if (low_count_ >= VBAT_LOW_COUNT) low_ = true;   // latch
  }

  float volts() const { return mv_ / 1000.0f; }
  bool low() const { return low_; }

  /// Rough state of charge from a typical LiPo discharge curve (piecewise-linear).
  int percent() const {
    static const struct { float v; int pct; } curve[] = {
        {4.20f, 100}, {4.10f, 90}, {3.95f, 75}, {3.82f, 50}, {3.73f, 30}, {3.65f, 15}, {3.30f, 0}};
    float v = volts();
    if (v >= curve[0].v) return 100;
    for (size_t i = 1; i < sizeof(curve) / sizeof(curve[0]); i++) {
      if (v >= curve[i].v) {
        float f = (v - curve[i].v) / (curve[i - 1].v - curve[i].v);
        return int(curve[i].pct + f * (curve[i - 1].pct - curve[i].pct));
      }
    }
    return 0;
  }

 private:
  /// analogReadMilliVolts() applies the chip's factory ADC calibration for us.
  float sampleMv() const { return analogReadMilliVolts(pin_) * VBAT_DIVIDER * VBAT_CAL; }

  uint8_t pin_ = 0;
  float mv_ = 0;
  uint32_t last_ms_ = 0;
  uint8_t low_count_ = 0;
  bool low_ = false;
};
