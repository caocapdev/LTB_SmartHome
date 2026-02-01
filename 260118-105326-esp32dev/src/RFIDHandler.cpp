#include "RFIDHandler.h"
#include <Wire.h>
#include <Adafruit_PN532.h>
#include "Common.h"

// PN532 I2C configuration
#define PN532_IRQ   (2)
#define PN532_RESET (3)  // Not connected by default on some modules, can be -1

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

// Door state and debouncing
bool doorStates[3] = {false, false, false};
uint8_t lastUid[7] = {0};
uint8_t lastUidLen = 0;
unsigned long lastScanTime = 0;

// Registered Cards
uint8_t allowedCards[3][7] = {
  {0x71, 0xCB, 0xFE, 0x5D},
  {0xE1, 0x81, 0x6B, 0xA2},
  {0xD1, 0xFB, 0xC2, 0xA3}
};
uint8_t allowedCardsLen[3] = {4, 4, 4};

void initRFID() {
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  nfc.SAMConfig();
  
  Serial.println("Waiting for an ISO14443A Card ...");
}

void handleRFID() {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;
    
  // Scan with 50ms timeout
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 50);
  
  if (success) {
    int authenticatedIndex = -1;
    for (int i = 0; i < 3; i++) {
        if (uidLength == allowedCardsLen[i]) {
            bool match = true;
            for (int j = 0; j < uidLength; j++) {
                if (uid[j] != allowedCards[i][j]) {
                    match = false;
                    break;
                }
            }
            if (match) {
                authenticatedIndex = i;
                break;
            }
        }
    }

    if (authenticatedIndex != -1) {
        // Debounce 0.5s
        if (millis() - lastScanTime > 500) {
            lastScanTime = millis();
            
            doorStates[authenticatedIndex] = !doorStates[authenticatedIndex];
            int doorNumber = authenticatedIndex + 1;
            
            // Short Beep
            pcfTouch.digitalWrite(15, 0); 
            delay(100);
            pcfTouch.digitalWrite(15, 1);

            if (doorStates[authenticatedIndex]) {
                Serial.print("ACCESS GRANTED - Opening Door "); Serial.println(doorNumber);
                Serial2.print("DOOR"); Serial2.print(doorNumber); Serial2.println(" OPEN");
            } else {
                Serial.print("ACCESS GRANTED - Closing Door "); Serial.println(doorNumber);
                Serial2.print("DOOR"); Serial2.print(doorNumber); Serial2.println(" CLOSE");
            }
        }
    } else {
        // Log unauthorized card (throttled)
        if (millis() - lastScanTime > 500) {
            lastScanTime = millis();
            
            // Double Beep
            pcfTouch.digitalWrite(15, 0); delay(100);
            pcfTouch.digitalWrite(15, 1); delay(100);
            pcfTouch.digitalWrite(15, 0); delay(100);
            pcfTouch.digitalWrite(15, 1);

            Serial.print("ACCESS DENIED - Card UID:");
            for (uint8_t i=0; i < uidLength; i++) {
              Serial.print(" 0x"); Serial.print(uid[i], HEX);
            }
            Serial.println("");
        }
    }
  }
}
