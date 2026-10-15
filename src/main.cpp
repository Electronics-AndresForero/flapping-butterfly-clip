/**
 * @file main.cpp
 * @brief Flapping Butterfly Clip - application logic (a small state machine).
 *
 *   IDLE      wings parked, waiting for a touch
 *   FLAPPING  wings flap continuously; a touch returns to IDLE
 *   LOW_BATT  battery below cutoff: wings parked, touch ignored (wearable build only)
 *
 * Hardware access lives in the headers in include/. This file only decides WHAT happens.
 */
#include <Arduino.h>
#include "config.h"
#include "Battery.h"
#include "StatusDisplay.h"
#include "TouchButton.h"
#include "WingServo.h"

enum class State : uint8_t { Idle, Flapping, LowBatt };

static StatusDisplay display;
static TouchButton   touch;
static WingServo     wings;
static Battery       battery;
static State         state = State::Idle;
static uint32_t      lastDraw = 0;

static void draw() {
  char l3[16], l4[16];
  const char* l2 = state == State::Flapping ? "FLAPPING"
                 : state == State::LowBatt  ? "LOW BATT"
                                            : "IDLE";
  if (HAS_BATTERY_SENSE) {
    snprintf(l3, sizeof l3, "%.2fV %d%%", battery.volts(), battery.percent());
  } else {
    snprintf(l3, sizeof l3, "USB power");
  }
  snprintf(l4, sizeof l4, "%s  %3d\xB0", BUILD_TAG, int(wings.angle()));
  display.show("BUTTERFLY", l2, l3, l4);
}

void setup() {
  Serial.begin(115200);
  display.begin();
  touch.begin(PIN_TOUCH);
  wings.begin(PIN_SERVO);
  if (HAS_BATTERY_SENSE) battery.begin(PIN_VBAT);
  draw();
  Serial.printf("Butterfly clip ready (%s build)\n", BUILD_TAG);
}

void loop() {
  uint32_t now = millis();

  if (HAS_BATTERY_SENSE) {
    battery.update(now);
    if (battery.low() && state != State::LowBatt) {
      state = State::LowBatt;            // latched: only a reboot / recharge clears it
      wings.setRunning(false);
      Serial.println("Low battery: wings stopped");
    }
  }

  if (touch.pressed(now) && state != State::LowBatt) {
    state = (state == State::Idle) ? State::Flapping : State::Idle;
    wings.setRunning(state == State::Flapping);
  }

  wings.update(now);

  if (now - lastDraw >= DISPLAY_PERIOD_MS) {
    lastDraw = now;
    draw();
  }
}
