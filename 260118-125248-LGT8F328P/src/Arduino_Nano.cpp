#include <Servo.h>
#include <AccelStepper.h>
#define REQUEST_ANALOG "REQUEST_ANALOG"
#define pin1 4
#define pin2 2
#define pinPWM 9
AccelStepper myStepper(1, 11, 12);

Servo doorServo;
Servo doorServo2;
unsigned long door2StartTime = 0;
bool door2IsMoving = false;
const unsigned long door2MoveDuration = 1500;

void setup() {
  Serial.begin(9600);
  doorServo.attach(5);
  doorServo2.attach(6);
  myStepper.setMaxSpeed(600);
	myStepper.setAcceleration(500);
	myStepper.setSpeed(600);
  pinMode(4,OUTPUT);
  pinMode(2,OUTPUT);
  pinMode(9,OUTPUT);
}

void loop() {
  myStepper.run();

  if (door2IsMoving && (millis() - door2StartTime >= door2MoveDuration)) {
    doorServo2.write(90);
    door2IsMoving = false;
    Serial.println("DOOR2 STOPPED");
  }

  if (Serial.available()) {
    String message = Serial.readStringUntil('\n');
    message.trim();

    if (message == REQUEST_ANALOG) {
      int analogValues[8];
      for (int i = 0; i < 8; i++) {
        analogValues[i] = analogRead(i);
        if(i==5){
          analogValues[i]=map(analogValues[i],0,1023,1023,0);
        }
      }

      for (int i = 0; i < 8; i++) {
        Serial.print(analogValues[i]);
        if (i < 7) {
          Serial.print(",");
        }
      }
      Serial.println();
    } else  if (message == "DOOR1 OPEN") {
      for(int i=0;i<95;i++){
        doorServo.write(i);
        delay(1);
      }
      Serial.println("DOOR1 OPENED");
    } else if (message == "DOOR1 CLOSE") {
      doorServo.write(0);
      Serial.println("DOOR1 CLOSED");
    } else  if (message == "DOOR2 OPEN") {
      doorServo2.write(180);
      door2StartTime = millis();
      door2IsMoving = true;
      Serial.println("DOOR2 OPENING");
    } else if (message == "DOOR2 CLOSE") {
      doorServo2.write(0);
      door2StartTime = millis();
      door2IsMoving = true;
      Serial.println("DOOR2 CLOSING");
    } else if(message == "OPEN MAINDOOR") {
      myStepper.moveTo(-6000);
    } else if(message == "CLOSE MAINDOOR"){
      myStepper.moveTo(0);
    } else if(message == "CURTAIN OPEN"){
      // Curtain Motor Control
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, HIGH);
      analogWrite(pinPWM, 256);
      delay(2000);
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, LOW);
    }
  }
}