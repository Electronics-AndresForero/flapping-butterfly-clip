/**
 * @file tools/hwcheck/main.cpp
 * @brief First-run hardware check. Flash this BEFORE wiring anything else.
 *
 * Prints over USB serial (115200):
 *   1. Every I2C address that answers on candidate SDA/SCL pairs -> proves where the OLED is.
 *   2. Raw ADC reading of GPIO0..GPIO4 (floating pins read noise; that is normal).
 * Expected on the 0.42" OLED board: an SSD1306 at 0x3C on SDA=GPIO5, SCL=GPIO6.
 */
#include <Arduino.h>
#include <Wire.h>

static void scanPair(int sda, int scl) {
  Wire.end();
  Wire.begin(sda, scl);
  Serial.printf("I2C scan SDA=%d SCL=%d: ", sda, scl);
  bool any = false;
  for (uint8_t a = 1; a < 127; a++) {
    Wire.beginTransmission(a);
    if (Wire.endTransmission() == 0) { Serial.printf("0x%02X ", a); any = true; }
  }
  Serial.println(any ? "" : "(nothing)");
}

void setup() {
  Serial.begin(115200);
  delay(2000);                 // give the USB-CDC port time to enumerate
}

void loop() {
  Serial.println("\n=== hwcheck ===");
  scanPair(5, 6);
  scanPair(8, 9);
  scanPair(20, 21);
  for (int p = 0; p <= 4; p++) {
    analogSetPinAttenuation(p, ADC_11db);
    Serial.printf("GPIO%d ADC = %d mV\n", p, analogReadMilliVolts(p));
  }
  delay(3000);
}
