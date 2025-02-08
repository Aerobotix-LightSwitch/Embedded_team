#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>


uint8_t nodemcu_mac[] = {0xE8,0xDB,0x84,0xAE,0x3E,0x1C};


char * esp_now_random_string(uint8_t len){
    char str[len];
    uint8_t i = 0;
    for(; i < len - 1; i++){
        str [i] = (char)random(127);
    }
    str[i] = '\0';
    return str;
}


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
    uint8_t random_len = random(250);
    const char * msg = esp_now_random_string(random_len);
    esp_now_send(nodemcu_mac,(uint8_t * )msg,strlen(msg));
 }

