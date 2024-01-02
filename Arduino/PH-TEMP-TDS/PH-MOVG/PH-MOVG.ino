#include <ADS1220_WE.h>
#include "MOVG.h"

#define ADS1220_CS_PIN    7 // chip select pin
#define ADS1220_DRDY_PIN  6 // data ready pin

// Construct the ADS1220 object
ADS1220_WE ads = ADS1220_WE(ADS1220_CS_PIN, ADS1220_DRDY_PIN);

// moving average filter
MOVG movg_PH(128, 2);

void setup() {
  Serial.begin(9600);
  ads.init();
  ads.bypassPGA(true);
  ads.setFIRFilter(ADS1220_50HZ_60HZ);
}

void loop() {
  ads.setCompareChannels(ADS1220_MUX_0_1);
  float PH = ads.getVoltage_mV();
  float m_PH = movg_PH.computeMOVG(PH);
  Serial.print(PH);
  Serial.print(" ");
  Serial.print(m_PH);
  Serial.print('\n');
}
