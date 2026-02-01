# Smart Home Automation System (NTM26)

## Project Overview

This repository contains the source code for the NTM26 Smart Home Automation System. The project is designed to provide a robust, modular, and secure environment control system, integrating various sensors, actuators, and an IoT platform for remote monitoring and control. The system is built upon a dual-microcontroller architecture to ensure efficient task distribution and real-time performance.

## System Architecture

The system operates using two primary processing nodes:

1.  **Main Controller (ESP32)**:
    -   Handles the core logic, internet connectivity, and communication with the ERa IoT platform.
    -   Manages high-level sensor data processing, alarm logic, and user interface elements (LCD).
    -   Communicates with I/O expanders via I2C.

2.  **Peripheral Controller (LGT8F328P / Arduino Nano)**:
    -   Offloads specific hardware tasks such as servo motor control for door mechanisms.
    -   Ensures precise timing for actuator operations independent of the main network stack.

## Hardware Configuration

### Core Components
-   **Microcontrollers**: ESP32 Dev Module, LGT8F328P (Arduino Nano compatible).
-   **Communication Protocol**: I2C for peripheral expansion, WiFi/MQTT for cloud connectivity.

### Sensors and Inputs
The system monitors various environmental and security parameters:
-   **Safety and Security**: Fire detection, Smoke detection, Burglar (Security) sensors.
-   **Presence and Access**: Radar motion detection, RFID authentication (RC522).
-   **Environmental**: Window status monitoring.
-   **User Input**: 15-channel Touch Sensor interface (via PCF8575).
-   **Analog Inputs**: 7 Analog channels for various telemetry.

### Actuators and Outputs
-   **Relay Control**: 12-channel relay system for controlling lighting, appliances, and power circuits.
-   **Door Control**: Servo motor mechanisms for automated door locking/unlocking.
-   **Visual Interface**: I2C LCD with Vietnamese character support using `LCDI2C_Vietnamese`.

### I/O Expansion
To accommodate the extensive number of peripherals, the system utilizes PCF8575 I/O expanders on the I2C bus:
-   `0x20`: Touch Logic
-   `0x21`: Digital Inputs
-   `0x22`: Relay Control

## Software Modules

The codebase is organized into modular components to facilitate maintenance and scalability.

### ESP32 Application (`260118-105326-esp32dev`)
-   **AlarmHandler**: Manages system state based on sensor inputs (Fire, Smoke, Intrusion).
-   **RFIDHandler**: Process RFID tag reads for secure authentication.
-   **RelayHandler**: Controls the state of the 12 relays providing logical abstraction for device control.
-   **SensorHandler**: Aggregates data from digital and analog sensors.
-   **DisplayHandler**: Manages LCD updates and status messages.
-   **MCPHandler**: Interfaces with the MCP/PCF expansion chips.
-   **IoT Integration**: Utilizes the ERa library for connection to the EOH cloud (mqtt1.eoh.io).

### Nano Application (`260118-125248-LGT8F328P`)
-   **Arduino_Nano.cpp**: Dedicated firmware for servo positioning and secondary hardware control.

## Getting Started

### Prerequisites
-   **Visual Studio Code**: Integrated Development Environment.
-   **PlatformIO**: Extension for VS Code is required to build and manage the project dependencies.

### Installation
1.  Clone the repository to your local machine.
2.  Open the project folder in Visual Studio Code.
3.  PlatformIO should automatically detect the multiple environments (`esp32dev` and `LGT8F328P`).
4.  Allow PlatformIO to download the necessary libraries and toolchains.

### Building and Flashing
-   **ESP32**: Select the `260118-105326-esp32dev` environment and click "Upload".
-   **Nano**: Select the `260118-125248-LGT8F328P` environment and click "Upload".

Ensure the correct COM ports are selected for each device before initiating the upload process.

## Dependencies

The project relies on the following key libraries:
-   `ERa`: For IoT and MQTT connectivity.
-   `LCDI2C_Vietnamese`: For localized display output.
-   `PCF8575`: For I2C I/O expansion.

---
*Verified against codebase structure as of February 2026.*
