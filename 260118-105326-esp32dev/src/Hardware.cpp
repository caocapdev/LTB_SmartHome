#include "Hardware.h"

LCDI2C_Vietnamese lcd(0x27, 20, 4);
PCF8575 pcfRelay(PCF8575_RELAY_ADDRESS);
PCF8575 pcfTouch(PCF8575_TOUCH_ADDRESS);
PCF8575 pcfDigital(PCF8575_DIGITAL_ADDRESS);

void initHardware() {
  lcd.init();
  lcd.backlight();
  lcd.print("NHÀ THÔNG MINH");
  lcd.setCursor(0, 1);
  lcd.print("THCS LÊ TẤN BÊ");
  lcd.setCursor(0, 2);
  lcd.print("ĐANG KHỞI ĐỘNG");
  lcd.setCursor(0, 3);
  lcd.print("ĐANG KẾT NỐI WIFI");

  Serial2.begin(9600, SERIAL_8N1, 16, 17);

  for (int pin = 0; pin < 16; pin++) {
    pcfRelay.pinMode(pin, OUTPUT);
    pcfDigital.pinMode(pin, INPUT);
    pcfRelay.digitalWrite(pin, LOW);
  }

  for (int pin = 0; pin < 15; pin++) {
    pcfTouch.pinMode(pin, INPUT);
  }
  pcfTouch.pinMode(15, OUTPUT);

  pcfRelay.begin();
  pcfDigital.begin();
  pcfTouch.begin();
  pcfTouch.digitalWrite(15, 1);
}
