/* Define MQTT host */
#define ERA_DEBUG
#define REQUEST_ANALOG "REQUEST_ANALOG"
#define DEFAULT_MQTT_HOST "mqtt1.eoh.io"
// You should get Auth Token in the ERa App or ERa Dashboard
#define ERA_AUTH_TOKEN "0c6e9a90-3d5b-4199-9880-02c9598f978e"
#include <LCDI2C_Multilingual.h>
#include <ERa.hpp>      // ERa IoT platform library
#include <ERa/ERaTimer.hpp> // ERa timer utility
#include <PCF8575.h>    // PCF8575 library
LCDI2C_Vietnamese lcd(0x27, 20, 4);

// Wi-Fi Credentials
const char ssid[] = "1111";
const char pass[] = "11111111";

// I²C Addresses
#define PCF8575_RELAY_ADDRESS 0x22  // Address of the PCF8575 on the relay PCB
#define PCF8575_DIGITAL_ADDRESS 0x21  // Address of the PCF8575 on the relay PCB
#define PCF8575_TOUCH_ADDRESS 0x20  // Address of the PCF8575 on the control PCB

// Number of Relays and Touch Sensors
#define NUM_RELAYS 12
#define NUM_TOUCH_SENSORS 15
#define NUM_ANALOG 7

// PCF8575 Objects
PCF8575 pcfRelay(PCF8575_RELAY_ADDRESS);  // For relay control
PCF8575 pcfTouch(PCF8575_TOUCH_ADDRESS);  // For touch sensor reading
PCF8575 pcfDigital(PCF8575_DIGITAL_ADDRESS);  // For touch sensor reading


// Relay and Touch Sensor States
bool relayStates[NUM_RELAYS] = {true};  // Tracks the state of each relay
bool touchStates[NUM_TOUCH_SENSORS] = {false};  // Tracks the state of each touch sensor
int analogData[8] = {0};
int analogDataMap[8] = {0};
bool digitalData[16];
bool isRelayUpdated=0;
bool alarmStatus=0;


// ERa Timer
ERaTimer timer;
ERaTimer timer100;

struct activeSensor {
  short fire;    // Indicates if a fire is detected
  short smoke;   // Indicates if smoke is detected
  short radar;   // Indicates if radar is triggered
  short window;  // Indicates if the window is open
  short safety;  // Indicates if the window is open
  short burglar;  // Indicates if the window is open
};

  activeSensor sensor;
  activeSensor status;
  activeSensor thresholds;
// Setup Function
void setup() {
  // Initialize Serial for debugging
  sensor.fire = true;   // Fire detected
  sensor.smoke = false; // No smoke
  sensor.radar = true;  // Radar triggered
  sensor.window = true; // Window closed
  sensor.burglar = true;
  sensor.safety = true;
  thresholds.smoke=75;

  lcd.init();
  lcd.backlight();
  lcd.print("NHÀ THÔNG MINH");
  lcd.setCursor(0, 1);
  lcd.print("THCS LÊ TẤN BÊ");
  lcd.setCursor(0, 2);
  lcd.print("ĐANG KHỞI ĐỘNG");
  lcd.setCursor(0, 3);
  lcd.print("ĐANG KẾT NỐI WIFI");
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);

  // Initialize I²C
  //Wire.begin();
  for (int pin = 0; pin < 16; pin++) { // PCF8575 has 16 I/O pins
    pcfRelay.pinMode(pin, OUTPUT);    // Set pin as OUTPUT
    pcfDigital.pinMode(pin, INPUT);    // Set pin as OUTPUT
    pcfRelay.digitalWrite(pin, LOW);         // Initialize all relays to OFF
  }

  // Configure all pins on PCF8575 for touch sensors as INPUT
  for (int pin = 0; pin < 15; pin++) { // PCF8575 has 16 I/O pins
    pcfTouch.pinMode(pin, INPUT);     // Set pin as INPUT
  }
  pcfTouch.pinMode(15,OUTPUT);
  // Initialize PCF8575 for relays and touch sensors
  pcfRelay.begin();
  pcfDigital.begin();
  pcfTouch.begin();
  pcfTouch.digitalWrite(15,1);

  // Connect to ERa IoT platform
  ERa.begin(ssid, pass);
  ERa.virtualWrite(33,0);
  // Set up a timer to periodically update states
  ERa.addInterval(1000L, timerEvent);
  ERa.addInterval(100L, timer100Event);
}

// Loop Function
void loop() {
  // Run ERa and timer
  ERa.run();
  timer.run();
  timer100.run();
  handleAlarm();

  // Check touch sensor states and control relays
}

// Timer Event Function
void timerEvent() {
  // Upload relay states to ERa periodically
  uploadRelayStates();
  if(!alarmStatus) updateLCD(sensor);
  handleDigital();
  if (requestAndReadAnalogData(Serial2, analogData, 8)) {
  // Success: Access analogData array
} else {
  // Failed to receive data
  Serial.println("No response from Arduino Nano.");
}
}
void timer100Event() {
  handleTouchSensors();
}

void handleAlarm(){
  scanSensor();
  if(status.safety&&sensor.safety){
    pcfTouch.digitalWrite(15,0);
    alarmStatus=1;
    if(status.smoke&&sensor.smoke){
      ERa.virtualWrite(33,1);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("PHÁT HIỆN KHÍ ĐỘC");
      lcd.setCursor(0,1);
      lcd.print("VUI LÒNG KIỂM TRA");
      lcd.setCursor(0,2);
      lcd.print("ĐÃ NGẮT ĐIỆN");
    }
    if(status.fire&&sensor.fire){
      ERa.virtualWrite(33,2);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("PHÁT HIỆN CHÁY");
      lcd.setCursor(0,1);
      lcd.print("VUI LÒNG KIỂM TRA");
      lcd.setCursor(0,2);
      lcd.print("ĐÃ NGẮT ĐIỆN");
    }
  }
  if(status.burglar&&sensor.burglar){
    ERa.virtualWrite(33,3);
    pcfTouch.digitalWrite(15,0);
    alarmStatus=1;
    if((status.window&&sensor.window)||(status.radar&&sensor.radar)){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("PHÁT HIỆN TRỘM");
      lcd.setCursor(0,1);
      lcd.print("ĐÃ NGẮT ĐIỆN");
      lcd.setCursor(0,2);
      lcd.print("ĐÃ KHOÁ CỬA");
    }
  }
}

// Handle Touch Sensors
void handleTouchSensors() {
  for (int i = 0; i < NUM_TOUCH_SENSORS; i++) {
    bool touchState = pcfTouch.digitalRead(i);  // Read the touch sensor state
    if (touchState != touchStates[i]) {  // If the state changed
      Serial.printf("Touch sensor %d changed its state \r ",i);
      touchStates[i] = touchState;       // Update the state               // If the touch sensor is pressed
      toggleRelay(i % NUM_RELAYS);     // Toggle the corresponding relay
    }
  }
}

void handleDigital(){
  for (int i = 0; i < 16; i++) {
    digitalData[i] = !pcfDigital.digitalRead(i);
  }
  Serial.print(digitalData[11]);
  ERa.virtualWrite(12,digitalData[8]);
  ERa.virtualWrite(13,digitalData[9]);
  ERa.virtualWrite(14,digitalData[10]);
  ERa.virtualWrite(15,digitalData[11]);
  ERa.virtualWrite(16,digitalData[12]);
  ERa.virtualWrite(17,digitalData[13]);
  ERa.virtualWrite(18,digitalData[14]);
  ERa.virtualWrite(19,digitalData[15]);
  ERa.virtualWrite(20,digitalData[5]);
  ERa.virtualWrite(21,digitalData[6]);
  ERa.virtualWrite(22,digitalData[7]);
}

// Toggle Relay
void toggleRelay(int relayIndex) {
  relayStates[relayIndex] = !relayStates[relayIndex];  // Toggle the relay state
  pcfRelay.digitalWrite(relayIndex, relayStates[relayIndex]); // Update the relay state on the PCF8575
  Serial.printf("Relay %d turned \r ",relayIndex);
  Serial.println(relayStates[relayIndex]==1?"OFF":"ON");

}

// Upload Relay States to ERa
void uploadRelayStates() {
  for (int i = 0; i < NUM_RELAYS; i++) {
    ERa.virtualWrite(i, relayStates[i]); // Send each relay state to ERa
  }
}

bool requestAndReadAnalogData(HardwareSerial& serial, int* analogData, int dataSize) {

  // Send the REQUEST_ANALOG command
  serial.println(REQUEST_ANALOG);

  // Wait for response
  unsigned long startTime = millis();
  while (!serial.available() && millis() - startTime < 1000); // Wait for 1 second

  if (serial.available()) {
    String response = serial.readStringUntil('\n'); // Read the entire response
    response.trim(); // Remove extra whitespace

    // Parse the comma-separated analog values
    int index = 0;
    int start = 0;
    int end = response.indexOf(',');

    while (end != -1 && index < dataSize) {
      analogData[index] = map(response.substring(start, end).toInt(),1,1024,1,100); // Convert to int
      start = end + 1;
      end = response.indexOf(',', start);
      index++;
    }
    // Parse the last value
    if (index < dataSize) {
      analogData[index] = map(response.substring(start, end).toInt(),1,1024,1,100);
    }
    for (int i = 0; i < NUM_ANALOG; i++) {
      ERa.virtualWrite(i+23, analogData[i]); // Send each relay state to ERa
    }
    return true; // Data received and parsed successfully
  }

  return false; // Failed to receive data
}

void scanSensor() {
  status.fire = (digitalData[12]+digitalData[13]+digitalData[14]+digitalData[15])>0 ? 1 : 0;
  status.smoke = (analogData[0] > thresholds.smoke||analogData[1] > thresholds.smoke||analogData[2] > thresholds.smoke||analogData[3] > thresholds.smoke) ? 1 : 0;
  status.radar = (digitalData[5]+digitalData[6]+digitalData[7])>0 ? 1 : 0;
  status.window = (digitalData[8]+digitalData[9]+digitalData[10]+digitalData[11]) ? 1 : 0;
  status.safety = (status.fire+status.smoke)>0 ? 1 : 0;
  status.burglar = (status.window+status.radar)>0 ? 1 : 0;
}
void updateLCD(const activeSensor& sensor) {
  // Clear the LCD screen
  lcd.clear();

  // Line 1: System status
  lcd.setCursor(0, 0);
  lcd.print("Online - Hệ thống OK");

  // Line 2: Smoke and Radar status
  lcd.setCursor(0, 1);
  lcd.print("Khói:");
  lcd.print(sensor.smoke ? "ON " : "OFF");
  lcd.setCursor(8, 1);
  lcd.print("Radar:");
  lcd.print(sensor.radar ? "ON" : "OFF");

  // Line 3: Fire and Safety status
  lcd.setCursor(0, 2);
  lcd.print("Lửa:");
  lcd.print(sensor.fire ? "ON " : "OFF");
  lcd.setCursor(8, 2);
  lcd.print("An toàn:");
  lcd.print(sensor.safety ? "ON" : "OFF");

  // Line 4: Window and Burglar status
  lcd.setCursor(0, 3);
  lcd.print("CSổ:");
  lcd.print(sensor.window ? "ON " : "OFF");
  lcd.setCursor(8, 3);
  lcd.print("Báo trộm:");
  lcd.print(sensor.burglar ? "ON" : "OFF");
}

ERA_CONNECTED() {
    Serial.println("ERa Connected");
}

ERA_DISCONNECTED() {
    Serial.println("ERa Disconnected");
}
// ERa Virtual Pin Write Callbacks
ERA_WRITE(V0) {
  int value = param.getInt();  // Get the value written to the virtual pin
  Serial.printf("Virtual pin %d get %d\r\n", pin, value);
  relayStates[0] = value;      // Update the relay state
  pcfRelay.digitalWrite(0, value);    // Update the relay on the PCF8575
}

ERA_WRITE(V1) {
  int value = param.getInt();
  relayStates[1] = value;
  pcfRelay.digitalWrite(1, value);
}

ERA_WRITE(V2) {
  int value = param.getInt();
  relayStates[2] = value;
  pcfRelay.digitalWrite(2, value);
}

ERA_WRITE(V3) {
  int value = param.getInt();
  relayStates[3] = value;
  pcfRelay.digitalWrite(3, value);
}

ERA_WRITE(V4) {
  int value = param.getInt();
  relayStates[4] = value;
  pcfRelay.digitalWrite(4, value);
}

ERA_WRITE(V5) {
  int value = param.getInt();
  relayStates[5] = value;
  pcfRelay.digitalWrite(5, value);
}

ERA_WRITE(V6) {
  int value = param.getInt();
  relayStates[6] = value;
  pcfRelay.digitalWrite(6, value);
}

ERA_WRITE(V7) {
  int value = param.getInt();
  relayStates[7] = value;
  pcfRelay.digitalWrite(7, value);
}

ERA_WRITE(V34) {
  int value = param.getInt();
  sensor.smoke=value;
  ERa.virtualWrite(34,value);
}

ERA_WRITE(V35) {
  int value = param.getInt();
  sensor.fire=value;
  ERa.virtualWrite(35,value);
}

ERA_WRITE(V36) {
  int value = param.getInt();
  sensor.window=value;
  ERa.virtualWrite(36,value);
}

ERA_WRITE(V37) {
  int value = param.getInt();
  sensor.radar=value;
  ERa.virtualWrite(37,value);
}

ERA_WRITE(V38) {
  int value = param.getInt();
  thresholds.smoke=value;
  ERa.virtualWrite(38,value);
}

ERA_WRITE(V42) {
  int value = param.getInt();
  sensor.safety=value;
  ERa.virtualWrite(42,value);
}

ERA_WRITE(V43) {
  int value = param.getInt();
  sensor.burglar=value;
  ERa.virtualWrite(43,value);
}

ERA_WRITE(47) {
  int value = param.getInt();
  for(int i=0;i<12;i++)pcfRelay.digitalWrite(i, value);
  ERa.virtualWrite(47,value);
}
ERA_WRITE(49) {
  int value = param.getInt();
  if(value){
    Serial2.println("OPEN MAINDOOR");
  } else{

    Serial2.println("CLOSE MAINDOOR");
  }
  ERa.virtualWrite(49,value);
}
ERA_WRITE(50) {
  int value = param.getInt();
  if(value){
    Serial2.println("DOOR1 OPEN");
  } else{

    Serial2.println("DOOR1 CLOSE");
  }
  ERa.virtualWrite(50,value);
}