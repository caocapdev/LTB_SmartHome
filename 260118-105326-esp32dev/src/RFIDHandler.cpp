#include "RFIDHandler.h"
#include <Wire.h>
#include <Adafruit_PN532.h>

// PN532 I2C configuration
#define PN532_IRQ   (2)
#define PN532_RESET (3)  // Not connected by default on some modules, can be -1

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

// Add your registered card UIDs here (2 cards)
// Example format: {0x12, 0x34, 0x56, 0x78}
uint8_t allowedCards[2][7] = {
  {0x71, 0xCB, 0xFE, 0x5D}, // Card 1: Replace with serial output
  {0x00, 0x00, 0x00, 0x00}  // Card 2: Replace with serial output
};
uint8_t allowedCardsLen[2] = {4, 4}; // Length of UIDs (usually 4 or 7)

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
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'success' will be true, and 'uid' will be populated with the UID
  // Timeout set to 50ms so it doesn't block the main loop
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 50);
  
  if (success) {
    // Display some basic info about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    for (uint8_t i=0; i < uidLength; i++) {
      Serial.print(" 0x");Serial.print(uid[i], HEX);
    }
    Serial.println("");

    // Check if the card is in the allowed list
    bool authenticated = false;
    for (int i = 0; i < 2; i++) {
        if (uidLength == allowedCardsLen[i]) {
            bool match = true;
            for (int j = 0; j < uidLength; j++) {
                if (uid[j] != allowedCards[i][j]) {
                    match = false;
                    break;
                }
            }
            if (match) {
                authenticated = true;
                break;
            }
        }
    }

// Door state tracking
bool door1Open = false;

    if (authenticated) {
        door1Open = !door1Open; // Toggle state
        if (door1Open) {
            Serial.println("ACCESS GRANTED - Opening Door 1");
            Serial2.println("DOOR1 OPEN");
        } else {
            Serial.println("ACCESS GRANTED - Closing Door 1");
            Serial2.println("DOOR1 CLOSE");
        }
    } else {
        Serial.println("ACCESS DENIED");
    }
    
    // Wait a bit before next scan to avoid duplicates
    delay(500);
  }
}
