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
  status.safety = (status.fire + status.smoke) > 0 ? 1 : 0;
  status.burglar = (status.window + status.radar) > 0 ? 1 : 0;
  // Serial.println(thresholds.smoke);
}

void handleAlarm() {
  // Serial.println(isArmed);
  scanSensor();
  if (!isArmed) {
    currentAlarmType = 0;
    return;
  }
  // Serial.println("passed");
  if (status.safety && sensor.safety) {
    pcfTouch.digitalWrite(15, 0);
    alarmStatus = 1;
    turnOffAllRelays();
    statusStartTime = millis(); // Kepp showing while active
    if (status.smoke && sensor.smoke) {
      ERa.virtualWrite(33, 1);
      currentAlarmType = 1;
      strcpy(dashboardStatus, "PHÁT HIỆN KHÍ ĐỘC");
    }
    if (status.fire && sensor.fire) {
      ERa.virtualWrite(33, 2);
      currentAlarmType = 2;
      strcpy(dashboardStatus, "PHÁT HIỆN CHÁY");
    }
  }
  if (status.burglar && sensor.burglar) {
    ERa.virtualWrite(33, 3);
    pcfTouch.digitalWrite(15, 0);
    alarmStatus = 1;
    turnOffAllRelays();
    statusStartTime = millis();
    if ((status.window && sensor.window) || (status.radar && sensor.radar)) {
      currentAlarmType = 3;
      strcpy(dashboardStatus, "PHÁT HIỆN TRỘM");
    }
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

ERA_WRITE(V51) {
  isArmed = param.getInt();
  ERa.virtualWrite(51, isArmed);
  Serial.println("v51 spam");
  ERa.virtualWrite(34, isArmed);
  ERa.virtualWrite(35, isArmed);
  ERa.virtualWrite(36, isArmed);
  ERa.virtualWrite(37, isArmed);

  ERa.virtualWrite(42, isArmed);
  ERa.virtualWrite(43, isArmed);
  sensor.smoke = isArmed;
  sensor.fire = isArmed;
  sensor.window = isArmed;
  sensor.radar = isArmed;
  sensor.safety = isArmed;
  sensor.burglar = isArmed;
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
