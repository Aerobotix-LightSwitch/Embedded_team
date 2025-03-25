#include <WiFi.h>
extern "C" {
  #include "esp_wifi.h"
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);  // Set the ESP32 to station mode

  uint8_t mac[6];  
  esp_wifi_get_mac(WIFI_IF_STA, mac);  // Get the MAC address of the ESP32 in station mode
  
  Serial.print("ESP32 MAC Address: ");
  for (int i = 0; i < 6; i++) {
    if (mac[i] < 0x10) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i < 5) Serial.print(":");
  }
  Serial.println();
}

void loop() {
  // Nothing needed here
}
