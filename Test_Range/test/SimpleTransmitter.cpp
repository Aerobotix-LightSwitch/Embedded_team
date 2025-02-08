#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>


uint8_t nodemcu_mac[] = {0xE8,0xDB,0x84,0xAE,0x3E,0x1C};
void onSent(uint8_t * reciver_mac,uint8_t msgStatus){
    Serial.print("Code Sending: ");
    Serial.println(msgStatus == 0 ? "Sucess" : "Failed");
}

void setup() {
    Serial.begin(115200);
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.setOutputPower(20.5);
    if(esp_now_init() != 0){
        Serial.println("FailedToInit");
        ESP.restart();
    }
    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
    esp_now_register_send_cb(onSent);
    Serial.println("Peering to Slave");
    while(esp_now_add_peer(nodemcu_mac,ESP_NOW_ROLE_SLAVE,1,NULL,0) != 0){
        Serial.print(".");
        delay(1000);
    } 
    Serial.println("\nPeered To Slave");
}

void loop() {
    char * msg = "UP";
    esp_now_send(nodemcu_mac,(uint8_t * )msg,strlen(msg));
    delay(1000);
 }