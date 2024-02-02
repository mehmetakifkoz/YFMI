#include <WiFi.h>
#include <HTTPClient.h>
#include <regex>

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

std::vector<int> parsePageVersions(const String& content) {
  // Define the regex pattern for extracting version numbers
  std::regex pattern("\\b(\\d+)\\.version\\b");

  // Convert the input string to a std::string
  std::string contentStdString = content.c_str();

  // Container to store extracted version numbers
  std::vector<int> versionNumbers;

  // Search for matches in the content
  std::sregex_iterator it(contentStdString.begin(), contentStdString.end(), pattern);
  std::sregex_iterator end;

  while (it != end) {
    std::smatch match = *it;
    int version = std::stoi(match.str());
    versionNumbers.push_back(version);
    ++it;
  }

  return versionNumbers;
}

void setup() {
  Serial.begin(115200);
  connectToWiFi();

  // Your additional setup code here

  // Call the renderPage function with the desired URL
  String pageContent = renderPage("http://your-server-ip/fota/");

  // Call the parsePageVersions function to extract version numbers
  std::vector<int> versions = parsePageVersions(pageContent);

  // Print the extracted version numbers
  Serial.println("Extracted Version Numbers:");
  for (int version : versions) {
    Serial.println(version);
  }
}

void loop() {
  // Your loop code here
}