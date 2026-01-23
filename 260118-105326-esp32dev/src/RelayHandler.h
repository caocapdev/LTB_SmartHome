#ifndef RELAY_HANDLER_H
#define RELAY_HANDLER_H

#include "Common.h"

void initRelay();
void toggleRelay(int relayIndex);
void uploadRelayStates();
void turnOffAllRelays();


#endif // RELAY_HANDLER_H
