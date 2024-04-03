/* ESP32 AWS IoT
 *  
 * Author: Anthony Elder 
 * License: Apache License v2
 * Also see https://github.com/earlephilhower/bearssl-esp8266  for contributions
 * https://github.com/Ameba8195/Arduino/blob/master/hardware_v2/libraries/MQTTClient/examples/amazon_awsiot_basic/amazon_awsiot_basic.ino
 * Sketch Modified by Stephen Borsay for www.udemy.com and AWS IoT Live Workshops
 * https://github.com/sborsay
 * Add in Char buffer utilizing sprintf to dispatch JSON data to AWS IoT Core
 * Use and replace your own SID, PW, AWS Account Endpoint, Client cert, private cert, x.509 CA root Cert
 */
#include <WiFiClientSecure.h>
#include <PubSubClient.h> // install with Library Manager, I used v2.6.0

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
  Serial.begin(115200); delay(50); Serial.println();
  Serial.println("ESP32 AWS IoT Example");
  Serial.printf("SDK version: %s\n", ESP.getSdkVersion());

  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  WiFi.waitForConnectResult();
  Serial.print(", WiFi connected, IP address: "); Serial.println(WiFi.localIP());

  wiFiClient.setCACert(rootCA);
  wiFiClient.setCertificate(certificate_pem_crt);
  wiFiClient.setPrivateKey(private_pem_key);
}

unsigned long lastPublish;
int msgCount;

static int sample = 1;
void loop() {
  pubSubCheckConnect();

  static unsigned long lastPublish = 0;

  char fakeData[128];

  float dt1 = random(2500, 4500) / 100.0; // Digital Temperature 1 (DT1)
  float dt2 = random(2500, 4500) / 100.0; // Digital Temperature 2 (DT2)
  float at_mv = random(4500, 15000) / 100.0; // Analog Temperature (AT_mV)
  float ph_mv = random(4500, 15000) / 100.0; // pH_mV
  float tds_mv = random(4500, 15000) / 100.0; // TDS_mV

  sprintf(fakeData, "{\"SAMPLE\":%d,\"DT1\":%.2f,\"DT2\":%.2f,\"AT_mV\":%.2f,\"pH_mV\":%.2f,\"TDS_mV\":%.2f}",
          sample, dt1, dt2, at_mv, ph_mv, tds_mv);

  if (millis() - lastPublish > 10000) {
    boolean rc = pubSubClient.publish("YFMI", fakeData);
    Serial.print("Published, rc="); Serial.print( (rc ? "OK: " : "FAILED: ") );
    Serial.println(fakeData);
    lastPublish = millis();
    sample++;
  }
}

void msgReceived(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on "); Serial.print(topic); Serial.print(": ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void pubSubCheckConnect() {
  if ( ! pubSubClient.connected()) {
    Serial.print("PubSubClient connecting to: "); Serial.print(awsEndpoint);
    while ( ! pubSubClient.connected()) {
      Serial.print(".");
      pubSubClient.connect("ESPthingXXXX");
      delay(1000);
    }
    Serial.println(" connected");
    pubSubClient.subscribe("inTopic");
  }
  pubSubClient.loop();
}