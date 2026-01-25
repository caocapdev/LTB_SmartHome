#include "SensorHandler.h"
#include "RelayHandler.h"

bool touchStates[NUM_TOUCH_SENSORS] = {false};
int analogData[8] = {0};
bool digitalData[16];

void initSensors() {
  for (int i = 0; i < NUM_TOUCH_SENSORS; i++) {
    touchStates[i] = pcfTouch.digitalRead(i);
  }
}

void handleTouchSensors() {
  for (int i = 0; i < NUM_TOUCH_SENSORS; i++) {
    bool touchState = pcfTouch.digitalRead(i);
    if (touchState != touchStates[i]) {
      touchStates[i] = touchState;
      
      // Original logic: toggle on any change (press and release)
      if (i < 12) {
        toggleRelay(i);
      } else if (i == 12) { // Label 13
        Serial2.println("OPEN MAINDOOR");
        statusStartTime = millis();
        strcpy(dashboardStatus, "NÂNG GARAGE");
      } else if (i == 13) { // Label 14
        Serial2.println("CLOSE MAINDOOR");
        statusStartTime = millis();
        strcpy(dashboardStatus, "HẠ GARAGE");
      } else if (i == 14) { // Label 15
        isArmed = !isArmed;
        statusStartTime = millis();
        if (!isArmed) {
          alarmStatus = 0;
          pcfTouch.digitalWrite(15, 1);
          ERa.virtualWrite(33, 0);

          ERa.virtualWrite(34, 0);
          ERa.virtualWrite(35, 0);
          ERa.virtualWrite(36, 0);
          ERa.virtualWrite(37, 0);

          ERa.virtualWrite(42, 0);
          ERa.virtualWrite(43, 0);

          ERa.virtualWrite(51, 0);
          strcpy(dashboardStatus, "HỆ THỐNG TẮT");
        } else {
          strcpy(dashboardStatus, "HỆ THỐNG BẬT");
          ERa.virtualWrite(34, 1);
          ERa.virtualWrite(35, 1);
          ERa.virtualWrite(36, 1);
          ERa.virtualWrite(37, 1);

          ERa.virtualWrite(42, 1);
          ERa.virtualWrite(43, 1);

          ERa.virtualWrite(51, 1);
        }
        ERa.virtualWrite(43, isArmed);
      }
    }
  }
}

void handleDigital() {
  for (int i = 0; i < 16; i++) {
    digitalData[i] = !pcfDigital.digitalRead(i);
    Serial.print(i);
    Serial.print(": ");
    Serial.print(digitalData[i]);
    Serial.print(" ");
  }
  Serial.println();
  ERa.virtualWrite(12, digitalData[8]);
  ERa.virtualWrite(13, digitalData[9]);
  ERa.virtualWrite(14, digitalData[10]);
  ERa.virtualWrite(15, digitalData[11]);
  ERa.virtualWrite(16, digitalData[12]);
  ERa.virtualWrite(17, digitalData[13]);
  ERa.virtualWrite(18, digitalData[14]);
  ERa.virtualWrite(19, digitalData[15]);
  ERa.virtualWrite(20, digitalData[5]);
  ERa.virtualWrite(21, digitalData[6]);
  ERa.virtualWrite(22, digitalData[7]);
}

bool requestAndReadAnalogData(HardwareSerial &serial, int *analogData,
                              int dataSize) {
  serial.println(REQUEST_ANALOG);

  unsigned long startTime = millis();
  while (!serial.available() && millis() - startTime < 50)
    ;

  if (serial.available()) {
    String response = serial.readStringUntil('\n');
    Serial.println(response);
    response.trim();

    int index = 0;
    int start = 0;
    int end = response.indexOf(',');

    while (end != -1 && index < dataSize) {
      analogData[index] =
          map(response.substring(start, end).toInt(), 1, 1024, 1, 100);
      start = end + 1;
      end = response.indexOf(',', start);
      index++;
    }
    if (index < dataSize) {
      analogData[index] =
          map(response.substring(start).toInt(), 1, 1024, 1, 100);
    }
    for (int i = 0; i < NUM_ANALOG; i++) {
      ERa.virtualWrite(i + 23, analogData[i]);
    }
    return true;
  }
  return false;
}
