# Line_Follower_Robot_Project_2026

> Autonomous line-following robot with PID control algorithm  
> **DUTH — Department of Informatics | Embedded Systems | Spring 2026**

![Arduino](https://img.shields.io/badge/Arduino-C%2B%2B-blue?style=for-the-badge&logo=arduino)
![Platform](https://img.shields.io/badge/Platform-Raspberry%20Pi%20Pico-ff69b4?style=for-the-badge)
![MCU](https://img.shields.io/badge/MCU-RP2040-c51a4a?style=for-the-badge&logo=raspberrypi&logoColor=white)
![Control](https://img.shields.io/badge/Control-PID-green?style=for-the-badge)

---

##  Description

This project involves the design and implementation of a fully **autonomous robotic vehicle** that follows a black line on a white surface. The system uses TCRT5000 infrared sensors and a **PID control algorithm** to dynamically correct its path in real time, without any human intervention.

<img width="940" height="415" alt="image" src="https://github.com/user-attachments/assets/274e0358-d438-4fe6-b444-319f2e758cab" />

---

##  System Features

-  Autonomous line tracking (straight paths & curves)
-  PID controller for smooth and stable navigation
-  Search mode — automatic line recovery when lost
-  Stop condition — automatic halt at the end of the track
-  Independent motor speed control via PWM

---

##  Hardware

| Component | Description | Qty |
|---|---|---|
| Maker Pi RP2040 | Main microcontroller board (RP2040 @ 133 MHz) | 1 |
| TCRT5000 | Analog infrared sensor for line detection | 3 |
| N20 DC Motor 6V/500rpm | Micro DC gear motor | 2 |
| N20 Rubber Wheel 43mm | Rubber wheels for traction | 2 |
| Li-Po 7.4V 1200mAh | Rechargeable battery pack | 1 |
| UBEC 5V | Voltage regulator (7.4V → 5V) | 1 |
| Plywood 3mm | Chassis 12.7×12.5 cm | — |

**Total build cost: ~€40.19**

---

##  Software

### Architecture

```
┌─────────────────────────────────────────┐
│              RP2040 (133MHz)            │
│                                         │
│  TCRT5000 ──► Sensor Read               │
│  (GPIO 26,27,28)   │                    │
│                    ▼                    │
│             Error Calculation           │
│          (left / center / right)        │
│                    │                    │
│                    ▼                    │
│            PID Controller               │
│           (Kp · e + Ki · ∫e + Kd · Δe)  │
│                    │                    │
│                    ▼                    │
│          PWM Output (Motor L / R)       │
└─────────────────────────────────────────┘
```
##  Wiring Diagram

<img width="560" height="324" alt="image" src="https://github.com/user-attachments/assets/f7e2d398-04f4-4bd5-9a65-1a8cfc11e7ea" />


### Control Logic

1. **Normal operation**: Sensors read the line → position error is calculated → PID adjusts the motors via PWM.

2. **Search mode**: If the line is lost (all sensors read white) → the robot rotates toward the last known direction until the line is found again.

3. **Stop condition**: If all sensors simultaneously detect black → the robot stops completely and PID variables are reset.


### Example Code

```cpp
// PID control
float error = get_error(b_l, b_c, b_r);
set_speeds(BASE_SPEED + correction,
           BASE_SPEED - correction);

// Search mode
if (last_seen == "left")
  set_speeds(0, SEARCH_SPEED);

// Stop condition
if (all_black)
  motor_stop();
```

---

##  Repository Structure

```
line-follower/
├── src/
│   └── LineFollowerCode.ino  # Main source code (C++/ArduinoIDE)
├── LineFollowerPictures/
|   ├── CompetionStages.png #Photos from the three stages 
│   ├── Photo.jpg # Line Follower Photo
│   ├── PhotoBottomView.jpg  #Photo from the bottom view
│   ├── PhotoTopView.jpg # Photo from the top view
├── LICENSE      
└── README.md
```

---

##  Getting Started

1. Flash the code onto the Maker Pi RP2040 board (using Arduino IDE).
2. Calibrate the TCRT5000 sensors via the onboard potentiometers according to ambient lighting conditions and connect the to the corresponding pins
on the board ADC(26,27,28)
3. Place the robot on the track and power on.

---

##  Results

- Reliable line tracking on both straight segments and curves
- Effective line recovery mechanism in search mode
- Correct activation of stop condition at track end
- Attention: Re-calibration required under varying lighting conditions

### Suggested Improvements

- Increase sensor count (3 → 5+) for faster response
- Use SPI/I2C communication for reduced sensor read latency
- Implement automatic calibration at startup

---

##   Team Members
<div align="center">
<table>
<tr>

<td align="center">
  <img src="https://github.com/DimitraDern.png" width="120px;" alt="Dimitra Dern"/><br>
  <sub><b>Dimitra Dernektsi</b></sub><br><br>

[![GitHub](https://img.shields.io/badge/GitHub-DimitraDern-black?style=for-the-badge&logo=github)](https://github.com/DimitraDern)

</td>

<td align="center">
  <img src="https://github.com/vlapozidis.png" width="120px;" alt="vlapozidis"/><br>
  <sub><b>Vladimiros Pozidis</b></sub><br><br>

[![GitHub](https://img.shields.io/badge/GitHub-vlapozidis-black?style=for-the-badge&logo=github)](https://github.com/vlapozidis)

</td>

<td align="center">

<img src="https://github.com/Kaitatzi.png" width="120px;" alt="Empty"/><br>
<sub><b>Stella Kaitatzi</b></sub>

[![GitHub](https://img.shields.io/badge/GitHub-Kaitatzi-black?style=for-the-badge&logo=github)](https://github.com/Kaitatzi)

</td>

<td align="center">

<img src="https://via.placeholder.com/120x120.png?text=%2B" width="120px;" alt="Empty"/><br>
<sub><b>Coming Soon</b></sub>

</td>

</tr>
</table>
</tr>
</div>
<div align="center">
Team 2 — Lab Section 1
Department of Informatics, DUTH — Spring Semester 2026  
<br>
<sub>© 2026 Group 2 - Democritus University of Thrace. All rights reserved.</sub>
</div>
