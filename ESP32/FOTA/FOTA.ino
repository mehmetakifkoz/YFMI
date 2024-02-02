#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "your-ssid";
const char* password = "your-password";

void connectToWiFi() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
    delay(1000);
  }

  Serial.println("Connected to WiFi");
}

String renderPage(const char* url) {
  WiFiClient client;
  HTTPClient http;

  http.begin(client, url);
  int httpResponseCode = http.GET();

  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);

  String payload = "";

  if (httpResponseCode > 0) {
    payload = http.getString();
    Serial.print("HTTP Response payload: ");
    Serial.println(payload);
  } else {
    Serial.println("Error in HTTP request");
  }

  http.end();
  return payload;
}

void setup() {
  Serial.begin(115200);
  connectToWiFi();

  // Your additional setup code here

  // Call the renderPage function with the desired URL
  String pageContent = renderPage("http://your-server-ip/fota/");
}

void loop() {
  // Your loop code here
}
