#include <Wire.h>
#include <RTClib.h>
#include <SD.h>

const int chipSelect = 10;
RTC_DS3231 rtc;

// file setup
char filename[13];

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

  // Count the number of existing files in the SD card
  int fileCount = 0;
  File root = SD.open("/");
  while (true) {
    File entry =  root.openNextFile();
    if (!entry) {
      break;
    }
    fileCount++;
    entry.close();
  }

  // Generate the filename based on the file count
  sprintf(filename, "%08d.csv", fileCount);

  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    dataFile.print("date");
    dataFile.print(',');
    dataFile.print("time");
    dataFile.print(',');
    dataFile.print("data1");
    dataFile.print(',');
    dataFile.print("data2");
    dataFile.print('\n');
    dataFile.close();
    Serial.println("Data written to SD card.");
  } else {
    Serial.println("Error opening file.");
  }
}

void loop() {
  DateTime now = rtc.now();
  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    dataFile.print(zeroFill(now.year(), 4));
    dataFile.print('-');
    dataFile.print(zeroFill(now.month(), 2));
    dataFile.print('-');
    dataFile.print(zeroFill(now.day(), 2));
    dataFile.print(',');
    dataFile.print(zeroFill(now.hour(), 2));
    dataFile.print(':');
    dataFile.print(zeroFill(now.minute(), 2));
    dataFile.print(':');
    dataFile.print(zeroFill(now.second(), 2));
    dataFile.print(',');
    dataFile.print("Sample Data 1");
    dataFile.print(',');
    dataFile.print("Sample Data 2");
    dataFile.print('\n');
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
