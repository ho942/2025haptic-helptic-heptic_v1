/// helptic hardware User Manual  ///
- code version: helptic_v1

## 1. Overview

This system measures weight using an HX711 load cell amplifier and provides real-time haptic feedback through two DRV2605L vibration drivers connected via a TCA9548A I2C multiplexer.

Each component must be wired and configured properly for stable operation.

## 2. Components

| No. | Component             | Quantity | Description |
|:--:|:----------------------|:--------:|:------------|
| 1  | Arduino-compatible MCU | 1        | (e.g., Arduino Uno, Mega) |
| 2  | HX711 Module           | 1        | Load cell amplifier |
| 3  | Load Cell              | 1        | Weight sensor |
| 4  | TCA9548A I2C Multiplexer| 1        | Controls multiple I2C devices |
| 5  | Adafruit DRV2605L Board | 2        | Vibration motor driver |
| 6  | LRA (Linear Resonant Actuator) | 2 | Vibrating motor |
| 7  | Connecting wires       | -        | Jumper cables |

## 3. Guide to operate

###<1. Required Libraries>
- HX711 Arduino Library
- Adafruit DRV2605 Library

---------------------------------------

###<2. Command Action in the code>

t	Tare (Zero Calibration)

w	Weight Calibration (Input known weight after placing)

ex. write "w1000" if there is 1kg part on the loadcell

a	Start continuous vibration

b	Stop all vibrations and reset count

--------------------------------------

###<3.1 Instruction: how to connect>

1) Connect the arduino to PC

2) Open the arduino ino code and check the port, board for connection

3) Please check whether your PC download the libaries <1.Required Libaries>

4) Upload the Arduino code provided

5) Then connection end.

---------------------------------------

###<3.2 Instruction: how to use>

1)When you upload the code, then loadcell starts to sense so 
   - Press `t` to tare the scale. (for make zero)

2)Then use a known weight 
   - Press `w` to calibrate using a known weight.
ex) if a known weight is 1kg, just put onthe loadcell and then write down on the serial monitor commands like "w1000"

3)For real demo, HMD with AI will detect the hand poster. When AI detect the correct hand poster, PC sends signal for hardware to start CPR. So it starts vibration. Like this, think of 'a' as a signal from PC, but in this code it is trigger by you.
   - Press `a` to start vibration feedback.

4) After that, Pressing the CPR hardware for 30.
   When counts are over than 30, print "c" to Serial and immediately reset count to 0.

5) If you wanna stop, 
   - Press `b` to stop vibrations.
then it show how many press you did. 

---------------------------------------------

###<3.3 What you can change>
- Threshold for counting : The current code is based on 2500g(2kg). Just change the number in the part that says (measured > 2500)
- `a`,`b`,"c" signal: If you wanna integrate hardware to PC, you can use other 'keyword' for starting vibration.
