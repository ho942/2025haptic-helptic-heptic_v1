#include "HX711.h"
#include <Wire.h>
#include "Adafruit_DRV2605.h"

const int DOUT_PIN = 2;
const int SCK_PIN = 3;

HX711 scale;

float calibration_factor = -7050.0;  // Initial calibration factor
String inputString = "";
bool waitingForWeightInput = false;
bool vibrationActive = false;  // 상태 변수, 진동이 활성화된 상태인지 확인

int count = 0;  // 측정 초과 횟수 카운트
bool overThreshold = false;  // 중복 count 방지

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

  scale.set_scale();

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
  Serial.println("[w] -> Weight Calibration (Input known weight after placing it)");
  Serial.println("[a] -> Start continuous vibration pattern");
  Serial.println("[b] -> Stop all vibrations");
  Serial.println("======================================");
}

void loop() {
  // Continuously display the current measured value
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
    // Start continuous vibration
    vibrationStrength = 255;  // Max strength
    if (measured >= 1000) {
      vibrationStrength = map(measured, 1000, 50000, 0, 255);  // Increase vibration strength as weight increases
    }

    // Count logic when over 2500g
    if (measured > 2500) {
      if (!overThreshold) {
        count++;
        overThreshold = true;
        if (count > 20) {
          Serial.println("c");
          count = 0;  // Reset count after printing "c"
        }
      }
    } else {
      overThreshold = false;
    }

  } else {
    // No vibration when inactive
    vibrationStrength = 0;
  }

  // Adjust vibration pattern based on strength
  int pattern = map(vibrationStrength, 0, 255, 1, 3);
  int repeatDelay = map(vibrationStrength, 0, 255, 200, 50);

  // Execute vibration on each DRV2605 device
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

    delay(repeatDelay);  // Adjust delay based on vibration strength
  }

  // Handle user input
  if (Serial.available()) {
    char c = Serial.read();

    if (waitingForWeightInput) {
      if (c == '\n' || c == '\r') {
        float actualWeight = inputString.toFloat();

        if (actualWeight > 0) {
          float measuredWeight = scale.get_units(10);
          calibration_factor = calibration_factor * (measuredWeight / actualWeight);
          Serial.print("✔ Calibration complete. New factor: ");
          Serial.println(calibration_factor);
        } else {
          Serial.println("✖ Invalid weight. Try again.");
        }

        inputString = "";
        waitingForWeightInput = false;
      } else {
        inputString += c;
      }
    } else {
      if (c == 't') {
        Serial.println("→ Taring... Remove all weight.");
        delay(2000);
        scale.tare();
        Serial.println("✔ Tare complete. Scale is zeroed.");
      }
      else if (c == 'w') {
        Serial.println("→ Place known weight on the scale, then enter the weight in grams and press Enter.");
        inputString = "";
        waitingForWeightInput = true;
      }
      else if (c == 'a') {
        vibrationActive = true;
        count = 0;
        Serial.println("→ Continuous vibration pattern started.");
      }
      else if (c == 'b') {
        vibrationActive = false;
        Serial.print("→ Vibration stopped. Count was: ");
        Serial.println(count);
        count = 0;
        Serial.println("→ All vibrations stopped.");
      }
    }
  }

  delay(10);  // Loop delay
}
