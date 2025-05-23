#include "HX711.h"
#include <Wire.h>
#include "Adafruit_DRV2605.h"

const int DOUT_PIN = 2;
const int SCK_PIN = 3;

HX711 scale;

float calibration_factor = -7050.0;  // Initial calibration factor
bool vibrationActive = false;  // State variable

int count = 0;  // Measurement overrun count
bool overThreshold = false;  // Avoid double counts

// TCA9548A I2C address
#define TCA_ADDR 0x70
Adafruit_DRV2605 drv;

// Function to select TCA9548A channel
void tcaSelect(uint8_t channel) {
  if (channel > 7) return;
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(1 << channel);  // Enable only the selected channel
  Wire.endTransmission();
}

void setup() {
  Serial.begin(115200);
  scale.begin(DOUT_PIN, SCK_PIN);
  scale.set_gain(64); // Set default gain

  scale.set_scale(); // Initialize with no scale factor yet

  Wire.begin();

  // Initialize DRV2605 on each TCA9548A channel
  for (uint8_t i = 0; i < 2; i++) {
    tcaSelect(i);
    if (!drv.begin()) {
      Serial.print("DRV2605 not found on channel ");
      Serial.println(i);
    } else {
      Serial.print("DRV2605 initialized on channel ");
      Serial.println(i);
    }

    drv.selectLibrary(6);  // Library 6 for LRA
    drv.setMode(DRV2605_MODE_INTTRIG);  // Internal trigger
  }

  Serial.println("====== HX711 2-Step Calibration ======");
  Serial.println("[t] -> Tare (Zero Calibration)");
  Serial.println("[w####] -> Weight Calibration (Input known weight after placing it)");
  Serial.println("[a] -> Start continuous vibration pattern");
  Serial.println("[b] -> Stop all vibrations");
  Serial.println("======================================");
}

void loop() {
  scale.set_scale(calibration_factor);
  scale.set_gain(64);
  float measured = scale.get_units(5);

  Serial.print("Measured: ");
  Serial.print(measured);
  Serial.print(" g\tCal.Factor: ");
  Serial.println(calibration_factor);

  int vibrationStrength = 0;

  // Check if vibration should be active
  if (vibrationActive) {
    vibrationStrength = 255;  // Max strength
    if (measured >= 1000) {
      vibrationStrength = map(measured, 1000, 50000, 0, 255);
    }

    if (measured > 2500) {
      if (!overThreshold) {
        count++;
        overThreshold = true;
        if (count > 30) {
          Serial.println("c");
          count = 0;
        }
      }
    } else {
      overThreshold = false;
    }

  } else {
    vibrationStrength = 0;
  }

  int pattern = map(vibrationStrength, 0, 255, 1, 3);
  int repeatDelay = map(vibrationStrength, 0, 255, 200, 50);

  for (uint8_t i = 0; i < 2; i++) {
    tcaSelect(i);

    if (vibrationStrength > 0) {
      drv.setWaveform(0, pattern);
    } else {
      drv.setWaveform(0, 0);
    }

    drv.setWaveform(1, 0);
    drv.go();

    Serial.print("Channel ");
    Serial.print(i);
    Serial.print(" - Pattern executed with vibration strength: ");
    Serial.print(vibrationStrength);
    Serial.print(" and pattern: ");
    Serial.println(pattern);

    delay(repeatDelay);
  }

  // Handle user input
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();  // Remove whitespace

    if (input.startsWith("w")) {
      String weightStr = input.substring(1);
      float actualWeight = weightStr.toFloat();

      if (actualWeight > 0) {
        Serial.println("→ Calibrating with known weight: " + String(actualWeight) + " grams...");
        float measuredWeight = scale.get_units(10);
        calibration_factor = calibration_factor * (measuredWeight / actualWeight);
        Serial.print("✔ Calibration complete. New factor: ");
        Serial.println(calibration_factor);
      } else {
        Serial.println("✖ Invalid weight input after 'w'. Try again.");
      }
    }
    else if (input == "t") {
      Serial.println("→ Taring... Remove all weight.");
      delay(2000);
      scale.tare();
      Serial.println("✔ Tare complete. Scale is zeroed.");
    }
    else if (input == "a") {
      vibrationActive = true;
      count = 0;
      Serial.println("→ Continuous vibration pattern started.");
    }
    else if (input == "b") {
      vibrationActive = false;
      Serial.print("→ Vibration stopped. Count was: ");
      Serial.println(count);
      count = 0;
      Serial.println("→ All vibrations stopped.");
    }
    else {
      Serial.println("✖ Unknown command.");
    }
  }

  delay(10);  // Small loop delay
}
