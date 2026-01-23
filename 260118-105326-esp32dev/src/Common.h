#ifndef COMMON_H
#define COMMON_H

// ERa Configuration (MUST be before includes)
#define ERA_AUTH_TOKEN "0c6e9a90-3d5b-4199-9880-02c9598f978e"
#define ERA_LOCATION_VN

// #include <Arduino.h>
#if !defined(ERA_MAIN)
#define NO_GLOBAL_ERA
#endif
#include <ERa.hpp>
#include <ERa/ERaTimer.hpp>
#include <LCDI2C_Vietnamese.h>
#include <PCF8575.h>

// Constants and Macros
#define REQUEST_ANALOG "REQUEST_ANALOG"
#define DEFAULT_MQTT_HOST "mqtt1.eoh.io"

// I²C Addresses
#define PCF8575_RELAY_ADDRESS 0x22
#define PCF8575_DIGITAL_ADDRESS 0x21
#define PCF8575_TOUCH_ADDRESS 0x20

// Number of Relays and Sensors
#define NUM_RELAYS 12
#define NUM_TOUCH_SENSORS 15
#define NUM_ANALOG 7

// Shared Data Structures
struct activeSensor {
  short fire;    // Indicates if a fire is detected
  short smoke;   // Indicates if smoke is detected
  short radar;   // Indicates if radar is triggered
  short window;  // Indicates if the window is open
  short safety;  // Indicates if the window is open
  short burglar; // Indicates if the window is open
};

// Extern Declarations for Global Objects
extern LCDI2C_Vietnamese lcd;
extern PCF8575 pcfRelay;
extern PCF8575 pcfTouch;
extern PCF8575 pcfDigital;

// Extern Declarations for Global Variables
extern bool relayStates[NUM_RELAYS];
extern bool touchStates[NUM_TOUCH_SENSORS];
extern int analogData[8];
extern int analogDataMap[8];
extern bool digitalData[16];
extern bool alarmStatus;
extern bool isArmed;
extern int currentAlarmType;
extern char dashboardStatus[64];
extern unsigned long statusStartTime;

extern activeSensor sensor;
extern activeSensor status;
extern activeSensor thresholds;

void initSensors();

#endif // COMMON_H
