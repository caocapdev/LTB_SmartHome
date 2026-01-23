#include "DisplayHandler.h"

uint32_t lastLCDUpdate = 0;
char dashboardStatus[64] = "Hệ thống OK";
unsigned long statusStartTime = 0;

void updateLCD(const activeSensor &sensor) {
  if (millis() - lastLCDUpdate < 1000) return;
  lastLCDUpdate = millis();

  if (alarmStatus && currentAlarmType > 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    if (currentAlarmType == 1) {
      lcd.print("PHÁT HIỆN KHÍ ĐỘC");
      lcd.setCursor(0, 1);
      lcd.print("VUI LÒNG KIỂM TRA");
      lcd.setCursor(0, 2);
      lcd.print("ĐÃ NGẮT ĐIỆN");
    } else if (currentAlarmType == 2) {
      lcd.print("PHÁT HIỆN CHÁY");
      lcd.setCursor(0, 1);
      lcd.print("VUI LÒNG KIỂM TRA");
      lcd.setCursor(0, 2);
      lcd.print("ĐÃ NGẮT ĐIỆN");
    } else if (currentAlarmType == 3) {
      lcd.print("PHÁT HIỆN TRỘM");
      lcd.setCursor(0, 1);
      lcd.print("ĐÃ NGẮT ĐIỆN");
      lcd.setCursor(0, 2);
      lcd.print("ĐÃ KHOÁ CỬA");
    }
    return;
  }

  // Reset status to default after 3 seconds
  if (statusStartTime > 0 && (millis() - statusStartTime > 3000)) {
    strcpy(dashboardStatus, "Hệ thống OK");
    statusStartTime = 0;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Onl - ");
  lcd.print(dashboardStatus);

  lcd.setCursor(0, 1);
  lcd.print("Khói:");
  lcd.print(sensor.smoke ? "ON " : "OFF");
  lcd.setCursor(8, 1);
  lcd.print("Radar:");
  lcd.print(sensor.radar ? "ON" : "OFF");

  lcd.setCursor(0, 2);
  lcd.print("Lửa:");
  lcd.print(sensor.fire ? "ON " : "OFF");
  lcd.setCursor(8, 2);
  lcd.print("An toàn:");
  lcd.print(sensor.safety ? "ON" : "OFF");

  lcd.setCursor(0, 3);
  lcd.print("CSổ:");
  lcd.print(sensor.window ? "ON " : "OFF");
  lcd.setCursor(8, 3);
  lcd.print("Báo trộm:");
  lcd.print(sensor.burglar ? "ON" : "OFF");
}
