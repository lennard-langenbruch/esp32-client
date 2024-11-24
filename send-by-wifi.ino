#include <WiFi.h>
#include <HTTPClient.h>

const char *ssid = "mSsid";
const char *password = "mPassword";
const char *url = "mUrl"; 

int btnGPIO = 0;
int btnState = false;

void setup() {
  Serial.begin(115200);
  delay(10);

  // Set GPIO0 Boot button as input
  pinMode(btnGPIO, INPUT);

  // Start connecting to WiFi
  Serial.println();
  Serial.print("[WiFi] Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int tryDelay = 500;
  int numberOfTries = 20;

  while (true) {
    switch (WiFi.status()) {
      case WL_NO_SSID_AVAIL:
        Serial.println("[WiFi] SSID not found");
        break;
      case WL_CONNECT_FAILED:
        Serial.print("[WiFi] Failed - WiFi not connected! Reason: ");
        return;
        break;
      case WL_CONNECTION_LOST:
        Serial.println("[WiFi] Connection was lost");
        break;
      case WL_SCAN_COMPLETED:
        Serial.println("[WiFi] Scan is completed");
        break;
      case WL_DISCONNECTED:
        Serial.println("[WiFi] WiFi is disconnected");
        break;
      case WL_CONNECTED:
        Serial.println("[WiFi] WiFi is connected!");
        Serial.print("[WiFi] IP address: ");
        Serial.println(WiFi.localIP());

        performGETRequest();

        return;
        break;
      default:
        Serial.print("[WiFi] WiFi Status: ");
        Serial.println(WiFi.status());
        break;
    }
    delay(tryDelay);

    if (numberOfTries <= 0) {
      Serial.print("[WiFi] Failed to connect to WiFi!");
      WiFi.disconnect();
      return;
    } else {
      numberOfTries--;
    }
  }
}

void loop() {
  // Read the button state
  btnState = digitalRead(btnGPIO);

  if (btnState == LOW) {
    // Disconnect from WiFi
    Serial.println("[WiFi] Disconnecting from WiFi!");
    if (WiFi.disconnect(true, false)) {
      Serial.println("[WiFi] Disconnected from WiFi!");
    }
    delay(1000);
  }
}

void performGETRequest() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    Serial.print("[HTTP] Starting GET request to: ");
    Serial.println(url);
    http.begin(url); 
    int httpResponseCode = http.GET(); // send request

    if (httpResponseCode > 0) {
      Serial.print("[HTTP] Response code: ");
      Serial.println(httpResponseCode);
      Serial.print("[HTTP] Response body: ");
      Serial.println(http.getString());
    } else {
      Serial.print("[HTTP] GET request failed, error: ");
      Serial.println(http.errorToString(httpResponseCode));
    }

    http.end(); // free resources
  } else {
    Serial.println("[HTTP] Not connected to WiFi");
  }
}
