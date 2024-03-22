#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ADS1220_WE.h>
#include <MOVG.h>

#define ONE_WIRE_BUS 5 // Data wire is plugged into port 9 on the Arduino
#define precision 12 // OneWire precision Dallas Sensor
#define ADS1220_CS_PIN    7 // chip select pin
#define ADS1220_DRDY_PIN  6 // data ready pin

// analog temperature parameters
const float R_nominal = 10660.0; // Nominal resistance at 25.00°C
const float T_nominal = 25.00;   // Nominal temperature
const float beta = 3965.0;       // Beta value
const int R1 = 10300;           // 10K resistor

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
MOVG movg_PH(128, 2);

void setup(void) {
  Serial.begin(9600);
  //parallax daq excel directives
  Serial.println("CLEARDATA");
  Serial.println("LABEL,CLOCK,SAMPLE,pH,T1,T2,TEMP");
  sensors.begin();
  sensors.setResolution(T1, precision);
  sensors.setResolution(T2, precision);
  ads.init();
  ads.bypassPGA(true);
  ads.setFIRFilter(ADS1220_50HZ_60HZ);
}

int sampleCount = 0;
void loop(void) {
  // PH
  float PH;
  float m_PH;
  ads.setCompareChannels(ADS1220_MUX_0_1);

  // DS18B20
  float temp1;
  float temp2;
  float m_temp1;
  float m_temp2;

  // compute moving average
  for(int i=0; i<32; i++){
    sensors.requestTemperatures();
    temp1 = sensors.getTempC(T1);
    temp2 = sensors.getTempC(T2);
    m_temp1 = movg_T1.computeMOVG(temp1);
    m_temp2 = movg_T2.computeMOVG(temp2);
    // PH
    for(int i=0; i<4; i++){
      PH = ads.getVoltage_mV();
      m_PH = movg_PH.computeMOVG(PH);
    }
  }

  // Analog Temperature
  ads.setCompareChannels(ADS1220_MUX_2_AVSS);
  float Vout = ads.getVoltage_mV();  // Voltage drop across NTC
  ads.setCompareChannels(ADS1220_MUX_AVDD_M_AVSS_4);
  float Vin = ads.getVoltage_mV() * 4.0;  // Total voltage across R1
  // Read NTC resistance
  float R_ntc = R1 * ((Vin - Vout) / Vout);
  // Calculate NTC temperature using Beta model
  float T_ntc = 1.0 / ((1.0 / (273.15 + T_nominal)) + (1.0 / beta) * log(R_ntc / R_nominal));
  float atemp = T_ntc - 273.15; // Convert back to Celsius

  // Serial.print
  Serial.print("DATA,DATE TIME,");
  Serial.print(sampleCount);
  Serial.print(",");
  Serial.print(m_PH);
  Serial.print(",");
  Serial.print(m_temp1);
  Serial.print(",");
  Serial.print(m_temp2);
  Serial.print(',');
  Serial.print(atemp); 
  Serial.print("\n");
  sampleCount++;
}
