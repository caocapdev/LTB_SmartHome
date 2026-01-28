#include "RFIDHandler.h"
#include <Wire.h>
#include <Adafruit_PN532.h>
#include "Common.h"

// PN532 I2C configuration
#define PN532_IRQ   (2)
#define PN532_RESET (3)  // Not connected by default on some modules, can be -1

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

// Door state and debouncing tracking
bool doorStates[3] = {false, false, false}; // State for Door 1, 2, and 3
uint8_t lastUid[7] = {0};
uint8_t lastUidLen = 0;
unsigned long lastScanTime = 0;

// Add your registered card UIDs here (2 cards)
// Example format: {0x12, 0x34, 0x56, 0x78}
// 0xE1 0x81 0x6B 0xA2
uint8_t allowedCards[3][7] = {
  {0x71, 0xCB, 0xFE, 0x5D}, // Card 1: Replace with serial output
  {0xE1, 0x81, 0x6B, 0xA2},  // Card 2: Replace with serial output
  {0xD1, 0xFB, 0xC2, 0xA3}  // Card 3: Replace with serial output
};
uint8_t allowedCardsLen[3] = {4, 4, 4}; // Length of UIDs (usually 4 or 7)

void initRFID() {
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // Configure board to read RFID tags
  nfc.SAMConfig();
  
  Serial.println("Waiting for an ISO14443A Card ...");
}

void handleRFID() {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID
    
  // Scan for card. Timeout 50ms.
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 50);
  
  if (success) {
    // Check if the card is in the allowed list
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
        // Strict lockout: only toggle if 0.5 seconds have passed since last action
        if (millis() - lastScanTime > 500) {
            lastScanTime = millis();
            
            // Toggle the specific door state
            doorStates[authenticatedIndex] = !doorStates[authenticatedIndex];
            int doorNumber = authenticatedIndex + 1;
            
            // Buzzer FeedBack: Short Beep
            pcfTouch.digitalWrite(15, 0); // ON
            delay(100);
            pcfTouch.digitalWrite(15, 1); // OFF

            if (doorStates[authenticatedIndex]) {
                Serial.print("ACCESS GRANTED - Opening Door "); Serial.println(doorNumber);
                Serial2.print("DOOR"); Serial2.print(doorNumber); Serial2.println(" OPEN");
            } else {
                Serial.print("ACCESS GRANTED - Closing Door "); Serial.println(doorNumber);
                Serial2.print("DOOR"); Serial2.print(doorNumber); Serial2.println(" CLOSE");
            }
        }
    } else {
        // For unauthorized cards, print UID once every 3 seconds to avoid log spam
        if (millis() - lastScanTime > 500) {
            lastScanTime = millis();
            
            // Buzzer FeedBack: Double Beep for Denied
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
