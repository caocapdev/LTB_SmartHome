#include "AlarmHandler.h"
#include "RelayHandler.h"

activeSensor sensor;
activeSensor status;
activeSensor thresholds;
bool alarmStatus = 0;
bool isArmed = false; // Default to armed
int currentAlarmType = 0;

void scanSensor() {
  status.fire = (digitalData[12] + digitalData[13] + digitalData[14] +
                 digitalData[15]) > 0
                    ? 1
                    : 0;
  status.smoke =
      (analogData[0] > thresholds.smoke || analogData[1] > thresholds.smoke ||
       analogData[2] > thresholds.smoke || analogData[3] > thresholds.smoke)
          ? 1
          : 0;
  status.radar = (digitalData[5] + digitalData[6] + digitalData[7]) > 0 ? 1 : 0;
  status.window =
      (digitalData[8] + digitalData[9] + digitalData[10] + digitalData[11]) ? 1
                                                                            : 0;

  // Only trigger global safety/burglar if the specific sensor type is enabled
  status.safety = ((status.fire && sensor.fire) || (status.smoke && sensor.smoke)) ? 1 : 0;
  status.burglar = ((status.window && sensor.window) || (status.radar && sensor.radar)) ? 1 : 0;
  // Serial.println(thresholds.smoke);
}

void handleAlarm() {
  // Serial.println(isArmed);
  scanSensor();
  if (!isArmed) {
    currentAlarmType = 0;
    alarmStatus = 0; // Reset alarm status when disarmed
    return;
  }

  // Latch alarmStatus and turn off relays
  if (status.safety || status.burglar) {
    alarmStatus = 1;
    turnOffAllRelays();
    statusStartTime = millis(); // Keep showing while active
  }

  // Handle high-level alarm buzzer control
  if (alarmStatus) {
    pcfTouch.digitalWrite(15, 0); // Active BUZZER
  }

  // Set specific alarm types (Once set, they latch until disarmed)
  if (status.safety) {
    if (status.smoke && currentAlarmType == 0) {
      ERa.virtualWrite(33, 1);
      currentAlarmType = 1;
      strcpy(dashboardStatus, "PHÁT HIỆN KHÍ ĐỘC");
    }
    if (status.fire && (currentAlarmType == 0 || currentAlarmType == 1)) {
      ERa.virtualWrite(33, 2);
      currentAlarmType = 2; // Fire has higher priority than smoke
      strcpy(dashboardStatus, "PHÁT HIỆN CHÁY");
    }
  } else if (status.burglar && currentAlarmType == 0) {
    ERa.virtualWrite(33, 3);
    currentAlarmType = 3;
    strcpy(dashboardStatus, "PHÁT HIỆN TRỘM");
  }
}

// ERa Virtual Pin Write Callbacks for Sensors and Doors
ERA_WRITE(V34) {
  sensor.smoke = param.getInt();
  ERa.virtualWrite(34, sensor.smoke);
}

ERA_WRITE(V35) {
  sensor.fire = param.getInt();
  ERa.virtualWrite(35, sensor.fire);
}

ERA_WRITE(V36) {
  sensor.window = param.getInt();
  ERa.virtualWrite(36, sensor.window);
}

ERA_WRITE(V37) {
  sensor.radar = param.getInt();
  ERa.virtualWrite(37, sensor.radar);
}

ERA_WRITE(V38) {
  thresholds.smoke = param.getInt();
  ERa.virtualWrite(38, thresholds.smoke);
}

ERA_WRITE(V42) {
  sensor.safety = param.getInt();
  ERa.virtualWrite(42, sensor.safety);
}

ERA_WRITE(V43) {
  sensor.burglar = param.getInt();
  ERa.virtualWrite(43, sensor.burglar);
}

void setArmedState(bool armed) {
  isArmed = armed;
  ERa.virtualWrite(51, isArmed);
  
  statusStartTime = millis();
  if (!isArmed) {
    alarmStatus = 0;
    currentAlarmType = 0;
    pcfTouch.digitalWrite(15, 1); // Turn off buzzer
    ERa.virtualWrite(33, 0);

    // Reset all sensor enable flags and their virtual pins
    sensor.smoke = 0;
    sensor.fire = 0;
    sensor.window = 0;
    sensor.radar = 0;
    sensor.safety = 0;
    sensor.burglar = 0;

    ERa.virtualWrite(34, 0);
    ERa.virtualWrite(35, 0);
    ERa.virtualWrite(36, 0);
    ERa.virtualWrite(37, 0);
    ERa.virtualWrite(42, 0);
    ERa.virtualWrite(43, 0);

    strcpy(dashboardStatus, "HỆ THỐNG TẮT");
  } else {
    // Enable all sensor flags when armed
    sensor.smoke = 1;
    sensor.fire = 1;
    sensor.window = 1;
    sensor.radar = 1;
    sensor.safety = 1;
    sensor.burglar = 1;

    ERa.virtualWrite(34, 1);
    ERa.virtualWrite(35, 1);
    ERa.virtualWrite(36, 1);
    ERa.virtualWrite(37, 1);
    ERa.virtualWrite(42, 1);
    ERa.virtualWrite(43, 1);

    strcpy(dashboardStatus, "HỆ THỐNG BẬT");
  }
}

ERA_WRITE(V51) {
  bool value = param.getInt();
  if (value != isArmed) {
    setArmedState(value);
  }
}

ERA_WRITE(49) {
  int value = param.getInt();
  if (value) {
    Serial2.println("OPEN MAINDOOR");
  } else {
    Serial2.println("CLOSE MAINDOOR");
  }
  ERa.virtualWrite(49, value);
}

ERA_WRITE(50) {
  int value = param.getInt();
  if (value) {
    Serial2.println("DOOR1 OPEN");
  } else {
    Serial2.println("DOOR1 CLOSE");
  }
  ERa.virtualWrite(50, value);
}
