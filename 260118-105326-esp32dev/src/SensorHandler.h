#ifndef SENSOR_HANDLER_H
#define SENSOR_HANDLER_H

#include "Common.h"

void handleTouchSensors();
void handleDigital();
bool requestAndReadAnalogData(HardwareSerial &serial, int *analogData,
                              int dataSize);

#endif // SENSOR_HANDLER_H
