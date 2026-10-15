/**
 * @file StatusDisplay.h
 * @brief Four short text lines on the 72x40 OLED (about 12 characters per line).
 *
 * The U8g2 driver "SSD1306_72X40_ER" already knows this panel is a 72x40 window inside a
 * 128x64 controller RAM, so no manual offset is needed. "F" = full frame buffer: we draw
 * everything into RAM and then push it once with sendBuffer().
 */
#pragma once
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include "config.h"

class StatusDisplay {
 public:
  void begin() {
    Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL);
    u8g2_.begin();
    u8g2_.setFont(u8g2_font_6x10_tf);
  }

  void show(const char* l1, const char* l2, const char* l3, const char* l4) {
    u8g2_.clearBuffer();
    u8g2_.drawStr(0, 9, l1);
    u8g2_.drawStr(0, 19, l2);
    u8g2_.drawStr(0, 29, l3);
    u8g2_.drawStr(0, 39, l4);
    u8g2_.sendBuffer();
  }

 private:
  U8G2_SSD1306_72X40_ER_F_HW_I2C u8g2_{U8G2_R0, U8X8_PIN_NONE, PIN_OLED_SCL, PIN_OLED_SDA};
};
