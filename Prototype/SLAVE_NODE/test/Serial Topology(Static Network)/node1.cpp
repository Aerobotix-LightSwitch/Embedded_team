#include <Arduino.h>
#include <espnow.h>
#include <ESP8266WiFi.h>


//=====================================================PINOUT_DEFINTION============================================================================ 

#define LIGHTS_NUMBER 2
const uint8_t lights_input_pins_def[LIGHTS_NUMBER] = {D5,D6};
const uint8_t lights_output_pins_def[LIGHTS_NUMBER] = {D1, D2};
uint8_t lights_output_pins_state[LIGHTS_NUMBER] = {0, 0};


//=====================================================MAC_ADDR_DEFINTION==============================================================================

const uint8_t device_mac[] = {0xE8,0xDB,0x84,0xAE,0x3E,0x1C};
const uint8_t  peer_behind[] = {0x10,0x06,0x1C,0xB5,0x7F,0x6C};
const uint8_t  peer_after[] = {0x48,0x3F,0xDA,0x7B,0x2B,0x35};



uint32_t timout_ms; 

void onSend(uint8_t * macaddr,uint8_t status_code){

}

void onRecv(uint8_t * macaddr,uint8_t * data,uint8_t len){

}


void setup() {
    Serial.begin(115200);
    for(uint8_t i = 0; i < LIGHTS_NUMBER; i++){
        pinMode(lights_input_pins_def[i],INPUT);
        pinMode(lights_output_pins_def[i],OUTPUT);
        digitalWrite(lights_output_pins_def[i],0);
        digitalWrite(lights_input_pins_def[i],0);
    }

    WiFi.setOutputPower(20.5);
    WiFi.mode(WIFI_STA);      
    WiFi.disconnect();


    if(esp_now_init() != 0){
        Serial.print("Failed to init the espnow .. Restarting the system\n");
        ESP.restart();
    }
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    esp_now_register_recv_cb(onRecv);
    esp_now_register_send_cb(onSend);


    esp_now_add_peer((unsigned char *)peer_behind, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
    esp_now_add_peer((unsigned char *)peer_after, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
}

void loop() {



}