#include "RelayHandler.h"

bool relayStates[NUM_RELAYS] = {true};

void initRelay(){
  for(int i=0; i < NUM_RELAYS; i++){
    relayStates[i]=1;
    pcfRelay.digitalWrite(i, relayStates[i]);
  }
}

void toggleRelay(int relayIndex) {
  if (relayIndex < 0 || relayIndex >= NUM_RELAYS)
    return;
  relayStates[relayIndex] = !relayStates[relayIndex];
  pcfRelay.digitalWrite(relayIndex, relayStates[relayIndex]);
  Serial.printf("Relay %d turned %s\r\n", relayIndex,
                relayStates[relayIndex] == 1 ? "OFF" : "ON");
  
  statusStartTime = millis();
  if (relayStates[relayIndex] == 1) {
    snprintf(dashboardStatus, sizeof(dashboardStatus), "ĐÈN %d TẮT", relayIndex + 1);
  } else {
    snprintf(dashboardStatus, sizeof(dashboardStatus), "ĐÈN %d BẬT", relayIndex + 1);
  }
}

void uploadRelayStates() {
  for (int i = 0; i < NUM_RELAYS; i++) {
    ERa.virtualWrite(i, relayStates[i]);
  }
}

void turnOffAllRelays() {
  for (int i = 0; i < NUM_RELAYS; i++) {
    relayStates[i] = 1; // 1 is OFF for these relays
    pcfRelay.digitalWrite(i, 1);
    ERa.virtualWrite(i, 1);
  }
  // snprintf(dashboardStatus, sizeof(dashboardStatus), "TẤT CẢ ĐÈN ĐÃ TẮT");
  statusStartTime = millis();
}

void setRelay(int relayIndex, bool state) {
  if (relayIndex < 0 || relayIndex >= NUM_RELAYS) return;
  relayStates[relayIndex] = state;
  pcfRelay.digitalWrite(relayIndex, state);
  ERa.virtualWrite(relayIndex, state);
}

// ERa Virtual Pin Write Callbacks for Relays
ERA_WRITE(V0) {
  int value = param.getInt();
  relayStates[0] = value;
  pcfRelay.digitalWrite(0, value);
  ERa.virtualWrite(V0, value);
}

ERA_WRITE(V1) {
  int value = param.getInt();
  relayStates[1] = value;
  pcfRelay.digitalWrite(1, value);
  ERa.virtualWrite(V1, value);
}

ERA_WRITE(V2) {
  int value = param.getInt();
  relayStates[2] = value;
  pcfRelay.digitalWrite(2, value);
  ERa.virtualWrite(V2, value);
}

ERA_WRITE(V3) {
  int value = param.getInt();
  relayStates[3] = value;
  pcfRelay.digitalWrite(3, value);
  ERa.virtualWrite(V3, value);
}

ERA_WRITE(V4) {
  int value = param.getInt();
  relayStates[4] = value;
  pcfRelay.digitalWrite(4, value);
  ERa.virtualWrite(V4, value);
}

ERA_WRITE(V5) {
  int value = param.getInt();
  relayStates[5] = value;
  pcfRelay.digitalWrite(5, value);
  ERa.virtualWrite(V5, value);
}

ERA_WRITE(V6) {
  int value = param.getInt();
  relayStates[6] = value;
  pcfRelay.digitalWrite(6, value);
  ERa.virtualWrite(V6, value);
}

ERA_WRITE(V7) {
  int value = param.getInt();
  relayStates[7] = value;
  pcfRelay.digitalWrite(7, value);
  ERa.virtualWrite(V7, value);
}

ERA_WRITE(V8) {
  int value = param.getInt();
  relayStates[8] = value;
  pcfRelay.digitalWrite(8, value);
  ERa.virtualWrite(V8, value);
}

ERA_WRITE(V9) {
  int value = param.getInt();
  relayStates[9] = value;
  pcfRelay.digitalWrite(9, value);
  ERa.virtualWrite(V9, value);
}

ERA_WRITE(V10) {
  int value = param.getInt();
  relayStates[10] = value;
  pcfRelay.digitalWrite(10, value);
  ERa.virtualWrite(V10, value);
}

ERA_WRITE(V11) {
  int value = param.getInt();
  relayStates[11] = value;
  pcfRelay.digitalWrite(11, value);
  ERa.virtualWrite(V11, value);
}

ERA_WRITE(47) {
  int value = param.getInt();
  for (int i = 0; i < 12; i++) {
    pcfRelay.digitalWrite(i, value);
    relayStates[i] = value;
  }
  ERa.virtualWrite(47, !value);
}
