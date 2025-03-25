#include <Arduino.h>
#include <espnow.h>
#include <ESP8266WiFi.h>

#define LIGHTS_NUMBER 2
const uint8_t lights_input_pins[LIGHTS_NUMBER] = {D5,D6};
const uint8_t lights_output_pins[LIGHTS_NUMBER] = {D1, D2};



uint32_t timout_ms; 

void setup() {
  Serial.begin(115200);
  WiFi.setOutputPower(20.5);
  WiFi.disconnect();


  if(esp_now_init() != 0){
    Serial.print("Failed to init the espnow .. Restarting the system\n");
    ESP.restart();
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
}

void loop() {



}