#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h> 

#define BUZEER_PIN D5
#define LED_PCB D4
bool toogle = false;
void onrecv(uint8_t * mac_sender,uint8_t * data,uint8_t len){
    Serial.print("Recieved Msg: ");
    for(uint8_t i = 0; i < len; i++){
        Serial.print((char)data[i]);
    }
    toogle = !toogle;
    digitalWrite(BUZEER_PIN,toogle);
    digitalWrite(LED_PCB,toogle);
    Serial.print(" SignalStrength: ");
    Serial.print(WiFi.RSSI());
    Serial.println("dB");
}

void setup() {
    pinMode(BUZEER_PIN,OUTPUT);
    pinMode(LED_PCB,OUTPUT);
    Serial.begin(115200);
    WiFi.disconnect();
    WiFi.setOutputPower(20.5);
    WiFi.mode(WIFI_STA);
    if(esp_now_init() != 0){
        Serial.println("Failed to init espnow Restarting ..");
        ESP.restart();
    }
    esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
    esp_now_register_recv_cb(onrecv);
}

void loop() {

}