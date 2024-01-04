#include <ADS1220_WE.h>
#include <MOVG.h>

#define ADS1220_CS_PIN    7 // chip select pin
#define ADS1220_DRDY_PIN  6 // data ready pin

// Construct the ADS1220 object
ADS1220_WE ads = ADS1220_WE(ADS1220_CS_PIN, ADS1220_DRDY_PIN);

// moving average filter
MOVG movg_TDS(32, 2);

void setup() {
  Serial.begin(9600);
  ads.init();
  ads.bypassPGA(true);
  ads.setFIRFilter(ADS1220_50HZ_60HZ);
}

void loop() {
  // TDS
  ads.setCompareChannels(ADS1220_MUX_3_AVSS);
  float tds = ads.getVoltage_mV();
  float m_tds = movg_TDS.computeMOVG(tds);
  Serial.print(tds); 
  Serial.print(" "); 
  Serial.print(m_tds);
  Serial.print("\n");
}
