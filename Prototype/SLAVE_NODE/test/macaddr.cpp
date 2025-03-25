#include <Arduino.h>
#include <espnow.h>
#include <ESP8266WiFi.h>


void setup() {
    Serial.begin(115200);
    WiFi.setOutputPower(20.5);
    WiFi.mode(WIFI_STA);

    Serial.print("ESP8266 with mac : ");
    Serial.println(WiFi.macAddress());
}

void loop() {

}