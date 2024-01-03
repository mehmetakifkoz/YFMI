#include <Wire.h>
#include <RTClib.h>
#include <SD.h>

const int chipSelect = 10;
RTC_DS3231 rtc;

void setup() {
  Serial.begin(9600);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  // Uncomment the following lines to set the RTC time manually
  // DateTime setTime = DateTime(2024, 1, 3, 10, 46, 22);
  // rtc.adjust(setTime);

  if (!SD.begin(chipSelect)) {
    Serial.println("Couldn't find SD card");
    while (1);
  }
}

void loop() {
  DateTime now = rtc.now();
  char filename[15];
  sprintf(filename, "%04d%02d%02d.csv", now.year(), now.month(), now.day());
  File dataFile = SD.open(filename, FILE_WRITE);

  if (dataFile) {
    dataFile.print(zeroFill(now.year(), 4));
    dataFile.print('/');
    dataFile.print(zeroFill(now.month(), 2));
    dataFile.print('/');
    dataFile.print(zeroFill(now.day(), 2));
    dataFile.print(',');
    dataFile.print(zeroFill(now.hour(), 2));
    dataFile.print(':');
    dataFile.print(zeroFill(now.minute(), 2));
    dataFile.print(':');
    dataFile.print(zeroFill(now.second(), 2));
    dataFile.println();
    
    dataFile.println("Sample Data 1");
    dataFile.println("Sample Data 2");
    
    dataFile.close();

    Serial.println("Data written to SD card.");
  } else {
    Serial.println("Error opening file.");
  }

  delay(5000);
}

String zeroFill(int value, int length) {
  String result = String(value);
  while (result.length() < length) {
    result = "0" + result;
  }
  return result;
}
