#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_AHTX0.h>

Adafruit_AHTX0 aht_0x39;
Adafruit_AHTX0 aht_0x38;

#define ONE_WIRE_BUS 9 // Data wire is plugged into port 9 on the Arduino
#define precision 12 // OneWire precision Dallas Sensor
 
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.

DeviceAddress T1 = {0x28, 0x86, 0x39, 0x94, 0x97, 0x03, 0x03, 0x3B};
DeviceAddress T2 = {0x28, 0x3B, 0xF6, 0x81, 0xE3, 0xE1, 0x3C, 0xE8};
DeviceAddress T3 = {0x28, 0x18, 0x61, 0x96, 0xF0, 0x01, 0x3C, 0x84};
DeviceAddress T4 = {0x28, 0x45, 0x54, 0x94, 0x97, 0x13, 0x03, 0xEC};
DeviceAddress T5 = {0x28, 0xA4, 0x69, 0x94, 0x97, 0xFF, 0x03, 0xA2};
DeviceAddress T6 = {0x28, 0xBF, 0x4F, 0x94, 0x97, 0x13, 0x03, 0x5D};
DeviceAddress T7 = {0x28, 0x98, 0x21, 0x94, 0x97, 0x03, 0x03, 0x31};
DeviceAddress T8 = {0x28, 0x03, 0x57, 0x94, 0x97, 0x13, 0x03, 0x65};
DeviceAddress T9 = {0x28, 0xF1, 0x0F, 0x94, 0x97, 0x04, 0x03, 0x50};
DeviceAddress T10 = {0x28, 0xF3, 0x0A, 0x94, 0x97, 0x04, 0x03, 0xEC};

void setup(void) {
  Serial.begin(9600); //Start serial port
  Wire.begin();
  
  // Initialize AHT sensor at address 0x39
  aht_0x39.begin(&Wire, 0, 0x39);
  aht_0x38.begin(&Wire, 0, 0x38);
  
  //parallax daq excel direktifleri
  Serial.println("CLEARDATA");
  Serial.print("LABEL,CLOCK,SAMPLE");
  Serial.print(",T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T_out1,T_out2,H1,H2");
  Serial.print("\n");

  // Start up the library
  sensors.begin();

  // set the resolution to 12 bit per device
  sensors.setResolution(T1, precision);
  sensors.setResolution(T2, precision);
  sensors.setResolution(T3, precision);
  sensors.setResolution(T4, precision);
  sensors.setResolution(T5, precision);
  sensors.setResolution(T6, precision);
  sensors.setResolution(T7, precision);
  sensors.setResolution(T8, precision);
  sensors.setResolution(T9, precision);
  sensors.setResolution(T10, precision);
}

int sample_count = 0;
void loop(void) {
  // call sensors.requestTemperatures() to issue a global temperature request to all devices on the bus
  sensors.requestTemperatures();
  float temp1 = sensors.getTempC(T1);
  float temp2 = sensors.getTempC(T2);
  float temp3 = sensors.getTempC(T3);
  float temp4 = sensors.getTempC(T4);
  float temp5 = sensors.getTempC(T5);
  float temp6 = sensors.getTempC(T6);
  float temp7 = sensors.getTempC(T7);
  float temp8 = sensors.getTempC(T8);
  float temp9 = sensors.getTempC(T9);
  float temp10 = sensors.getTempC(T10);

  // Read and print data from sensor at address 0x39
  sensors_event_t humidity_0x39, temp_0x39;
  aht_0x39.getEvent(&humidity_0x39, &temp_0x39);

  // Read and print data from sensor at address 0x38
  sensors_event_t humidity_0x38, temp_0x38;
  aht_0x38.getEvent(&humidity_0x38, &temp_0x38);

  // Assign adafruit sensors
  float temp_out1 = temp_0x39.temperature;
  float temp_out2 = temp_0x38.temperature;
  float humidity1 = humidity_0x39.relative_humidity;
  float humidity2 = humidity_0x38.relative_humidity;

  Serial.print("DATA,TIME,");
  Serial.print(sample_count);
  Serial.print(",");
  Serial.print(temp1);
  Serial.print(",");
  Serial.print(temp2);
  Serial.print(",");
  Serial.print(temp3);
  Serial.print(",");
  Serial.print(temp4);
  Serial.print(",");
  Serial.print(temp5);
  Serial.print(",");
  Serial.print(temp6);
  Serial.print(",");
  Serial.print(temp7);
  Serial.print(",");
  Serial.print(temp8);
  Serial.print(",");
  Serial.print(temp9);
  Serial.print(",");
  Serial.print(temp10);
  Serial.print(",");
  Serial.print(temp_out1);
  Serial.print(",");
  Serial.print(temp_out2);
  Serial.print(",");
  Serial.print(humidity1);
  Serial.print(",");
  Serial.print(humidity2);
  Serial.print("\n");
  delay(10000);

  // increment the sample count
  sample_count++;
}