#include <OneWire.h>
#include <DallasTemperature.h>
#include <ADS1220_WE.h>
#include <MOVG.h>

#define ONE_WIRE_BUS 5 // Data wire is plugged into port 9 on the Arduino
#define precision 12 // OneWire precision Dallas Sensor
#define ADS1220_CS_PIN    7 // chip select pin
#define ADS1220_DRDY_PIN  6 // data ready pin

// Pass oneWire reference to Dallas Temperature
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress T1 = {0x28, 0x31, 0x30, 0x94, 0x97, 0x03, 0x03, 0x4A};
DeviceAddress T2 = {0x28, 0x03, 0x2C, 0x94, 0x97, 0x04, 0x03, 0xFA};

// Construct the ADS1220 object
ADS1220_WE ads = ADS1220_WE(ADS1220_CS_PIN, ADS1220_DRDY_PIN);

// moving average filter
MOVG movg_T1(32, 2);
MOVG movg_T2(32, 2);
MOVG movg_TDS(64, 2);

void setup(void) {
  Serial.begin(9600);
  //parallax daq excel directives
  Serial.println("CLEARDATA");
  Serial.println("LABEL,CLOCK,SAMPLE,T1,T2,mT1,mT2,TDS");
  sensors.begin();
  sensors.setResolution(T1, precision);
  sensors.setResolution(T2, precision);
  ads.init();
  ads.bypassPGA(true);
  ads.setFIRFilter(ADS1220_50HZ_60HZ);
}

int sampleCount = 0;
void loop(void) {
  // DS18B20
  float temp1, temp2;
  float m_temp1, m_temp2;
  sensors.requestTemperatures();
  temp1 = sensors.getTempC(T1);
  temp2 = sensors.getTempC(T2);
  m_temp1 = movg_T1.computeMOVG(temp1);
  m_temp2 = movg_T2.computeMOVG(temp2);

  // TDS
  float tds;
  float m_tds;
  ads.setCompareChannels(ADS1220_MUX_3_AVSS);
  for (int i=0; i<64; i++){
    tds = ads.getVoltage_mV();
    m_tds = movg_TDS.computeMOVG(tds);
  }

  // Serial.print
  Serial.print("DATA,TIME,");
  Serial.print(sampleCount);
  Serial.print(",");
  Serial.print(temp1);
  Serial.print(",");
  Serial.print(temp2);
  Serial.print(",");
  Serial.print(m_temp1);
  Serial.print(",");
  Serial.print(m_temp2);
  Serial.print(",");
  Serial.print(m_tds);
  Serial.print("\n");
  sampleCount++;
}

