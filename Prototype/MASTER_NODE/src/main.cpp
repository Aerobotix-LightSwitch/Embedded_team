#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_MODE_STA);

    Serial.print("ESP32 with mac : ");
    Serial.println(WiFi.macAddress());
}

void loop() {

}