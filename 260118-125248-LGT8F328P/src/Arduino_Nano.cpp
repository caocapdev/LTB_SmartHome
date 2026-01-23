#include <Servo.h>
#include <AccelStepper.h>
#define REQUEST_ANALOG "REQUEST_ANALOG" // Command to request analog data
#define pin1 4
#define pin2 2
#define pinPWM 9
AccelStepper myStepper(1, 11, 12);

Servo doorServo;

void setup() {
  // Initialize Serial communication
  Serial.begin(9600);
  doorServo.attach(5);
  myStepper.setMaxSpeed(1000);
	myStepper.setAcceleration(200);
	myStepper.setSpeed(1000);
  pinMode(4,OUTPUT);
  pinMode(2,OUTPUT);
  pinMode(9,OUTPUT);
}

void loop() {
  myStepper.run();
  // Check if data is available in the Serial buffer
  if (Serial.available()) {
    // Read the incoming message
    String message = Serial.readStringUntil('\n'); // Read until newline
    message.trim(); // Remove any extra whitespace

    // Check if the message is REQUEST_ANALOG
    if (message == REQUEST_ANALOG) {
      // Read analog sensor data (e.g., from A0 to A5)
      int analogValues[8];
      for (int i = 0; i < 8; i++) {
        analogValues[i] = analogRead(i); // Read analog pins A0 to A5
      }

      // Send analog data back to the Serial Monitor
      for (int i = 0; i < 8; i++) {
        Serial.print(analogValues[i]);
        if (i < 7) {
          Serial.print(","); // Separate values with a comma
        }
      }
      Serial.println(); // End the message with a newline
    } else  if (message == "DOOR1 OPEN") {
    for(int i=0;i<140;i++){
      
      doorServo.write(i); // Turn servo to 90 degrees
    delay(1);
    }
    Serial.println("DOOR1 OPENED");
  } else if (message == "DOOR1 CLOSE") {
    doorServo.write(0); // Turn servo to 0 degrees
    Serial.println("DOOR1 CLOSED");
  } else if(message == "LIFT GARAGE") {
	  myStepper.moveTo(15000);
    
  } else if(message == "DROP GARAGE"){
	  myStepper.moveTo(0);

  } else if(message == "CURTAIN OPEN"){
    // Set the standby pin to HIGH to enable the motor
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, HIGH);
    // Set the motor speed using PWM
    analogWrite(pinPWM, 256);
    delay(2000);
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);

  }
  }
}