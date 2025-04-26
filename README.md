# 2025haptic-helptic-heptic_v1
this code for control helptic device

Real-Time Weight Measurement and Vibration Feedback System
(using HX711 + TCA9548A + DRV2605)

This project utilizes an HX711 load cell amplifier and two Adafruit DRV2605L haptic drivers to
adjust vibration strength and patterns according to the measured weight.
An I2C multiplexer (TCA9548A) is used to control multiple DRV2605 devices independently.

<br>
Features
Read weight data from HX711 load cell.

Control 2 DRV2605 vibration motors independently via TCA9548A multiplexer.

Dynamically adjust vibration strength and pattern based on weight.

Count and trigger events when exceeding a weight threshold.

Interactive calibration and control via Serial commands.

<br>
Serial Commands

Command	Action
t	Tare (Zero Calibration)
w	Weight Calibration (Input known weight after placing)
a	Start continuous vibration
b	Stop all vibrations and reset count
<br>
Project Flow Diagram
Here’s the high-level process flow:

mermaid
복사
편집
flowchart TD
    A[Power ON] --> B[Initialize HX711 and DRV2605s via TCA9548A]
    B --> C{Serial Input?}
    C -- "t: Tare" --> D[Tare the scale]
    C -- "w: Weight Calibration" --> E[Input known weight -> Adjust calibration factor]
    C -- "a: Start Vibration" --> F[Enable continuous vibration based on weight]
    C -- "b: Stop Vibration" --> G[Disable all vibration, reset count]
    F --> H[Measure weight continuously]
    H --> I{Weight > 2500g?}
    I -- Yes --> J[Increase count]
    J --> K{Count > 20?}
    K -- Yes --> L[Print "c" to Serial and reset count]
    K -- No --> H
    I -- No --> H
    G --> H
(Diagram generated using Mermaid.js)

<br>
Hardware Setup

Module	Connection Pin
HX711 DOUT	Pin 2
HX711 SCK	Pin 3
TCA9548A	I2C (Shared SCL/SDA)
Devices:

HX711 Load Cell Amplifier

TCA9548A I2C Multiplexer

DRV2605L Haptic Motor Driver ×2

LRA (Linear Resonant Actuator) motors

<br>
Required Libraries
HX711 Arduino Library

Adafruit DRV2605 Library

Arduino built-in Wire.h

<br>
Core Logic
cpp
복사
편집
// Read the current weight
float measured = scale.get_units(5);

// Map weight to vibration strength
int vibrationStrength = map(measured, 1000, 50000, 0, 255);

// Map vibration strength to pattern
int pattern = map(vibrationStrength, 0, 255, 1, 3);

// Activate vibration accordingly
drv.setWaveform(0, pattern);
drv.go();
<br>
Notes
Calibration (calibration_factor) must be tuned according to your specific load cell.

When the weight exceeds 2500g continuously, "c" is printed after 20 times.

DRV2605L is configured for Library 6 (optimized for LRA motors).

<br>
Example System Photo
(to be inserted)

<br>
License
This project is licensed under the MIT License.

Extra: Want a README Badge Pack?
I can also quickly generate:

"Made with Arduino" badges

"Using HX711" badges

"Realtime Feedback System" badges

"MIT License" badge

Would you like me to make a badge set too? 🚀
(They make the GitHub page look even more polished!)
Or if you want, I can create a more graphical system architecture (block diagram style) too. 🎨
