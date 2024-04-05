#include <MOVG.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ADS1220_WE.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h> // install with Library Manager, I used v2.6.0

#define ONE_WIRE_BUS 20 // Data wire is plugged into port 9 on the Arduino
#define precision 12 // OneWire precision Dallas Sensor
#define ADS1220_CS_PIN 14 // chip select pin
#define ADS1220_DRDY_PIN 3 // data ready pin

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

const char* ssid = "YOUR NETWORK NAME";
const char* password = "YOUR NETWORK PASSWORD";

const char* awsEndpoint = "YOUR-AWS-IOT-ENDPOINT"; //AWS IoT Core--> Settings (Device data endpoint)

// xxxxxxxxxx-certificate.pem.crt
static const char certificate_pem_crt[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)KEY";

// xxxxxxxxxx-private.pem.key
static const char private_pem_key[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
-----END RSA PRIVATE KEY-----
)KEY";

/* root CA found at:
  https://www.amazontrust.com/repository/AmazonRootCA1.pem
  fungible between regions, I dont care if you use mine below or paste in your own.
*/
static const char rootCA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)EOF";


WiFiClientSecure wiFiClient;
void msgReceived(char* topic, byte* payload, unsigned int len);
PubSubClient pubSubClient(awsEndpoint, 8883, msgReceived, wiFiClient); 

void setup() {
  Serial.begin(9600);
  delay(5000);

  //parallax daq excel directives
  Serial.println("CLEARDATA");
  Serial.println("LABEL,CLOCK,SAMPLE,DT1,DT2,AT_mV,pH_mV,TDS_mV");

  // DS18B20
  sensors.begin();
  sensors.setResolution(DT1Address, precision);
  sensors.setResolution(DT2Address, precision);

  //ADS
  ads.init();
  ads.bypassPGA(true);
  ads.setFIRFilter(ADS1220_50HZ_60HZ);
  
  WiFi.begin(ssid, password);
  WiFi.waitForConnectResult();

  wiFiClient.setCACert(rootCA);
  wiFiClient.setCertificate(certificate_pem_crt);
  wiFiClient.setPrivateKey(private_pem_key);
}

int SAMPLE = 0;
void loop() {
  // Digital Temperature
  for (int i=0; i<32; i++) {
    sensors.requestTemperatures();
    movg_DT1.addValue(sensors.getTempC(DT1Address));
    movg_DT2.addValue(sensors.getTempC(DT2Address));
  }
  float DT1 = movg_DT1.computeMOVG();
  float DT2 = movg_DT2.computeMOVG();

  // pH
  for (int i=0; i<128; i++) {
    ads.setCompareChannels(ADS1220_MUX_0_1);
    movg_pH_mV.addValue(ads.getVoltage_mV());
  }
  float pH_mV = movg_pH_mV.computeMOVG();

  float AT_mV = random(4500, 15000) / 100.0; // Analog Temperature Voltage (AT_mV)
  float TDS_mV = random(4500, 15000) / 100.0; // TDS_mV

  // Serial.print()
  Serial.print("DATA,DATE TIME,");
  Serial.print(SAMPLE);
  Serial.print(",");
  Serial.print(DT1);
  Serial.print(",");
  Serial.print(DT2);
  Serial.print(",");
  Serial.print(AT_mV);
  Serial.print(",");
  Serial.print(pH_mV);
  Serial.print(",");
  Serial.print(TDS_mV);
  Serial.print('\n');
  SAMPLE++;

  pubSubCheckConnect();

  char data[128];

  sprintf(data, "{\"SAMPLE\":%d,\"DT1\":%.2f,\"DT2\":%.2f,\"AT_mV\":%.2f,\"pH_mV\":%.2f,\"TDS_mV\":%.2f}",
          SAMPLE, DT1, DT2, AT_mV, pH_mV, TDS_mV);

  pubSubClient.publish("YFMI", data);
}

void msgReceived(char* topic, byte* payload, unsigned int length) {
  return;
}

void pubSubCheckConnect() {
  unsigned long startTime = millis(); // Record the start time
  while (!pubSubClient.connected()) {
    // Attempt to connect
    pubSubClient.connect("ESPthingXXXX");

    // Check if the connection is successful or timeout occurs
    if (pubSubClient.connected() || (millis() - startTime >= 3000)) {
      break; // Exit the loop if connected or timeout reached
    }
    delay(1000);
  }
  pubSubClient.loop();
}