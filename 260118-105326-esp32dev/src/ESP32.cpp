#define ERA_MAIN
#include "AlarmHandler.h"
#include "Common.h"
#include "DisplayHandler.h"
#include "Hardware.h"
#include "RelayHandler.h"
#include "SensorHandler.h"

// Wi-Fi Credentials
const char ssid[] = "xnxx";
const char pass[] = "11111111";

// ERa Timers
ERaTimer timer;

void timerEvent();
void debounce();

void setup() {
  Serial.begin(115200);

  // Initialize Sensor Values
  sensor.fire = true;
  sensor.smoke = false;
  sensor.radar = true;
  sensor.window = true;
  sensor.burglar = true;
  sensor.safety = true;
  thresholds.smoke = 75;

  // Initialize Hardware
  initHardware();
  initSensors();
  initRelay();

  // Connect to ERa IoT platform
  ERa.begin(ssid, pass);
  ERa.virtualWrite(33, 0);

  // Set up periodic tasks
  ERa.addInterval(1000L, timerEvent);
  ERa.addInterval(100L, debounce);
}

void loop() {
  ERa.run();
  timer.run();
}
void debounce(){
  handleTouchSensors();
  handleAlarm();
}

void timerEvent() {
  uploadRelayStates();
  updateLCD(sensor);
  handleDigital();

  if (!requestAndReadAnalogData(Serial2, analogData, 8)) {
    Serial.println("No response from Arduino Nano.");
  }
}

ERA_CONNECTED() { 
  Serial.println("ERa Connected");
}

ERA_DISCONNECTED() { 
  Serial.println("ERa Disconnected");
}