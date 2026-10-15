/**
 * @file WingServo.h
 * @brief Non-blocking sine-eased flapping of a hobby servo.
 *
 * CONCEPT - why easing: driving a servo straight to a new angle makes it snap at full
 * speed, which looks mechanical and shocks the gears. We instead move along a cosine curve
 *     e(t) = 0.5 - 0.5*cos(pi*t),  t in [0,1]
 * which starts slowly, speeds up, then slows down ("ease in-out"), like a real wing.
 *
 * CONCEPT - non-blocking: update() computes the angle from the clock (millis) instead of
 * using delay(), so the main loop stays free to read the touch input and refresh the OLED.
 *
 * CONCEPT - detach when idle: a powered servo holds its position and draws current.
 * At rest we stop sending pulses (detach), which saves battery and removes jitter.
 */
#pragma once
#include <Arduino.h>
#include <ESP32Servo.h>
#include "config.h"

class WingServo {
 public:
  void begin(uint8_t pin) { pin_ = pin; angle_ = ANGLE_OPEN_DEG; }

  /// true = keep flapping, false = finish the current stroke, go to rest and stop.
  void setRunning(bool run) { running_ = run; }
  bool running() const { return running_ || moving_; }
  float angle() const { return angle_; }

  /// Call every loop() with millis().
  void update(uint32_t now) {
    if (!moving_) {
      if (running_) {
        // Start a new stroke towards the opposite end of the previous one.
        startStroke(now, toClosed_ ? ANGLE_CLOSED_DEG : ANGLE_OPEN_DEG);
        toClosed_ = !toClosed_;
      } else if (angle_ != ANGLE_OPEN_DEG) {
        startStroke(now, ANGLE_OPEN_DEG);     // park the wings open
        toClosed_ = true;
      }
      return;
    }

    float t = float(now - t0_) / float(FLAP_MS);
    if (t >= 1.0f) {                          // stroke finished
      angle_ = to_;
      write(angle_);
      moving_ = false;
      if (!running_ && angle_ == ANGLE_OPEN_DEG) servo_.detach();   // idle: stop pulsing
      return;
    }
    float e = 0.5f - 0.5f * cosf(PI * t);     // sine ease in-out
    angle_ = from_ + (to_ - from_) * e;
    write(angle_);
  }

 private:
  void startStroke(uint32_t now, float to) {
    if (!servo_.attached()) {
      servo_.setPeriodHertz(50);              // standard 50 Hz servo frame
      servo_.attach(pin_, SERVO_MIN_US, SERVO_MAX_US);
    }
    from_ = angle_;
    to_ = to;
    t0_ = now;
    moving_ = true;
  }
  void write(float deg) { servo_.write(int(deg + 0.5f)); }

  Servo servo_;
  uint8_t pin_ = 0;
  float angle_ = 0, from_ = 0, to_ = 0;
  uint32_t t0_ = 0;
  bool running_ = false, moving_ = false, toClosed_ = true;
};
