#include <ADS1220_WE.h>

#define ADS1220_CS_PIN    7 // chip select pin
#define ADS1220_DRDY_PIN  6 // data ready pin

// Construct the ADS1220 object
ADS1220_WE ads = ADS1220_WE(ADS1220_CS_PIN, ADS1220_DRDY_PIN);

// analog temperature parameters
const float R_nominal = 10660.0; // Nominal resistance at 25.00°C
const float T_nominal = 25.00;   // Nominal temperature
const float beta = 3965.0;       // Beta value
const int R1 = 10300;           // 10K resistor

void setup() {
  Serial.begin(9600);
  Serial.println("CLEARDATA");
  Serial.println("LABEL,CLOCK,SAMPLE,TEMP"); //parallax daq excel direktifleri
  ads.init();
  ads.bypassPGA(true);
  ads.setFIRFilter(ADS1220_50HZ_60HZ);
}

int sampleCount = 0;
void loop() {
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

  Serial.print("DATA,TIME,");
  Serial.print(atemp); 
  Serial.print("\n");
  sampleCount++;
}
