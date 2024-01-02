#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 5 // Data wire is plugged into port 9 on the Arduino
#define precision 12 // OneWire precision Dallas Sensor

// Pass oneWire reference to Dallas Temperature
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

DeviceAddress T1 = {0x28, 0x31, 0x30, 0x94, 0x97, 0x03, 0x03, 0x4A};
DeviceAddress T2 = {0x28, 0x03, 0x2C, 0x94, 0x97, 0x04, 0x03, 0xFA};

void setup(void) {
  Serial.begin(9600);
  sensors.begin();
  sensors.setResolution(T1, precision);
  sensors.setResolution(T2, precision);
}

void loop(void) {
  sensors.requestTemperatures();
  float temp1 = sensors.getTempC(T1);
  float temp2 = sensors.getTempC(T2);
  Serial.print(temp1);
  Serial.print(" ");
  Serial.print(temp2);
  Serial.print("\n");
}