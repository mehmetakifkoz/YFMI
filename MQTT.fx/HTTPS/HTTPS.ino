/* ESP32 HTTP(s) Certificate Secured to AWS IoT
 * TESTED TO WORK ON ARDUINO 1.8.16 WITH ESP32 BOARD PACKAGE VERSION 1.0.6
 * Author: Anthony Elder, Earle F. Philhower-3, Rui Santos,  various others
 * License: Apache License v2
 * Sketch Modified by Stephen Borsay for www.udemy.com and my AWS IoT Live Workshops
 * github.com/sborsay
 * Add in Char buffer utilizing sprintf to dispatch JSON data to AWS IoT Core
 * Use and replace your own SID, PW, AWS Account Endpoint, Client cert, private cert, x.509 CA root Cert
 * 
 * also
 *  github.com/espressif/arduino-esp32/blob/master/libraries/WiFiClientSecure/examples/WiFiClientSecure/WiFiClientSecure.ino
 * 
 * references:
 *docs.aws.amazon.com/iot/latest/developerguide/http.html#httpurl
 *github.com/espressif/arduino-esp32/blob/master/libraries/WiFiClientSecure/examples/WiFiClientSecure/WiFiClientSecure.ino
 *github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/BearSSL_ServerClientCert/BearSSL_ServerClientCert.ino
 */
#include <WiFiClientSecure.h>

const char* ssid = "WiFi Network name"; //your network
const char* password = "WiFi Network Password";  //your wifi password

const char* host = "aiknowkungfues-ats.iot.us-east-1.amazonaws.com"; //AWS IoT Core--> Settings (Device data endpoint)
const int Port = 8443;  //typical values are HTTPS= 443, MQTT=8883, and HTTP = 80 but AWS IoT Core uses 84443 for HTTP(s) Certificate secured

//AWS IoT Topic Subscribe to "outTopic" in MQTT test client
const char *uri = "/topics/outTopic?qos=1";  //see https://docs.aws.amazon.com/iot/latest/developerguide/http.html


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

/* root CA 
   https://www.amazontrust.com/repository/AmazonRootCA1.pem
  fungible between regions, I dont care if you use mine below or paste in your own.
*/
static const char rootCA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)EOF";

WiFiClientSecure myClient;

void setup() {
  Serial.begin(115200); delay(50); Serial.println();
  Serial.println("ESP32 AWS IoT Example");
  Serial.printf("SDK version: %s\n", ESP.getSdkVersion());

  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print(", WiFi connected, IP address: "); Serial.println(WiFi.localIP());

  myClient.setCACert(rootCA);
  myClient.setCertificate(certificate_pem_crt);
  myClient.setPrivateKey(private_pem_key);
}

static int sample = 1;
void loop() {
  
  Serial.println(host);
  myClient.setTimeout(15000); // 15 Seconds
  delay(1000);
  
  Serial.print("HTTPS Connecting to AWS IoT Core\n");
  int r=0; //retry counter
  while((!myClient.connect(host, Port)) && (r < 30)){
      delay(100);
      Serial.print(".");
      r++;
  }
  if(r==30) {
    Serial.println("Connection failed");
  }
  else {
    Serial.println("Connected");
  }
  
  
  char fakeData[256];

  float at = random(25, 45); // Analog Temperature (AT)
  float dt1 = random(25, 45); // Digital Temperature 1 (DT1)
  float dt2 = random(25, 45); // Digital Temperature 2 (DT2)
  float ph_mv = random(45, 150); // pH_mV
  float tds_mv = random(45, 150); // TDS_mV

  sprintf(fakeData, "{\"UPTIME\":%lu,\"SAMPLE\":%d,\"AT\":%.0f,\"DT1\":%.0f,\"DT2\":%.0f,\"pH_mV\":%.0f,\"TDS_mV\":%.0f}",
          millis() / 1000, sample, at, dt1, dt2, ph_mv, tds_mv);
  sample++;

  char postStr[64];
  sprintf(postStr, "POST %s HTTP/1.1", uri);  // put together the string for HTTP POST
  
  Serial.println("request is sending");
  
  myClient.println(postStr);
  myClient.print("Host: ");
  myClient.println(host);
  myClient.println("Content-Type: application/json");
  myClient.print("Content-Length: "); 
  myClient.println(strlen(fakeData));
  myClient.println();    // extra `\r\n` to separate the http header and http body
  myClient.println(fakeData);          

  Serial.println(fakeData);
  Serial.println("request sent");
  Serial.print("Heap space remaining: ");
  Serial.println(ESP.getFreeHeap());  //Libraby macro to observe remaining heap size in bytes.
  delay(10000);  // myClient.stop must be placed after some delay, not before
  myClient.stop();  //prevents heap colliding with stack, remember, no automatic garbage collection in C/C++
                    //needed for ESP32 in this sketch but not ESP8266 consistently
}