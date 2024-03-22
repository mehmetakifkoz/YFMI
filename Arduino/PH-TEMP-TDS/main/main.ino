#include <OneWire.h>
#include <DallasTemperature.h>
#include <ADS1220_WE.h>
#include <MOVG.h>

#define ONE_WIRE_BUS 5 // Data wire is plugged into port 9 on the Arduino
#define precision 12 // OneWire precision Dallas Sensor
#define ADS1220_CS_PIN 7 // chip select pin
#define ADS1220_DRDY_PIN 6 // data ready pin

// Pass oneWire reference to Dallas Temperature
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress DT1Address = {0x28, 0x31, 0x30, 0x94, 0x97, 0x03, 0x03, 0x4A};
DeviceAddress DT2Address = {0x28, 0x03, 0x2C, 0x94, 0x97, 0x04, 0x03, 0xFA};

// Construct the ADS1220 object
ADS1220_WE ads = ADS1220_WE(ADS1220_CS_PIN, ADS1220_DRDY_PIN);

// moving average filter
MOVG movg_DT1(32, 2);
MOVG movg_DT2(32, 2);
MOVG movg_pH_mV(128, 2);

void setup(void) {
  Serial.begin(9600);
  //parallax daq excel directives
  Serial.println("CLEARDATA");
  Serial.println("LABEL,CLOCK,SAMPLE,DT1,DT2,pH_mV,AT_mV");
  // sensors setup
  sensors.begin();
  sensors.setResolution(DT1Address, precision);
  sensors.setResolution(DT2Address, precision);
  ads.init();
  ads.bypassPGA(true);
  ads.setFIRFilter(ADS1220_50HZ_60HZ);
}

int sampleCount = 0;
void loop(void) {
  // pH_mV
  float pH_mV;
  ads.setCompareChannels(ADS1220_MUX_0_1);

  // DS18B20
  float DT1;
  float DT2;

  // compute moving average
  for(int i=0; i<32; i++){
    sensors.requestTemperatures();
    DT1 = movg_DT1.computeMOVG(sensors.getTempC(DT1Address));
    DT2 = movg_DT2.computeMOVG(sensors.getTempC(DT2Address));
    // PH
    for(int i=0; i<4; i++){
      pH_mV = movg_pH_mV.computeMOVG(pH_mV = ads.getVoltage_mV());
    }
  }

  // Analog Temperature
  ads.setCompareChannels(ADS1220_MUX_2_AVSS);
  float Vout = ads.getVoltage_mV();  // Voltage drop across NTC
  ads.setCompareChannels(ADS1220_MUX_AVDD_M_AVSS_4);
  float Vin = ads.getVoltage_mV() * 4.0;  // Total voltage across R1
  // Read NTC resistance
  float AT_mV = (Vin - Vout) / Vout;

  // Serial.print
  Serial.print("DATA,DATE TIME,");
  Serial.print(sampleCount);
  Serial.print(",");
  Serial.print(DT1);
  Serial.print(",");
  Serial.print(DT2);
  Serial.print(",");
  Serial.print(pH_mV);
  Serial.print(",");
  Serial.print(AT_mV); 
  Serial.print("\n");
  sampleCount++;
}
