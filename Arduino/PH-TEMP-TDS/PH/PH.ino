#include <ADS1220_WE.h>

#define ADS1220_CS_PIN    7 // chip select pin
#define ADS1220_DRDY_PIN  6 // data ready pin

// Construct the ADS1220 object
ADS1220_WE ads = ADS1220_WE(ADS1220_CS_PIN, ADS1220_DRDY_PIN);

void setup() {
  Serial.begin(9600);
  ads.init();
  ads.bypassPGA(true);
  ads.setFIRFilter(ADS1220_50HZ_60HZ);
}

void loop() {
  ads.setCompareChannels(ADS1220_MUX_0_1);
  float PH = ads.getVoltage_mV();
  Serial.print(PH);
  Serial.print('\n');
}
