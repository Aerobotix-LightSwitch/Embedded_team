#include <Arduino.h>
#include <espnow.h>
#include <ESP8266WiFi.h>


//=====================================================PINOUT_DEFINTION============================================================================================================

#define LIGHTS_NUMBER 2
const uint8_t lights_input_pins_def[LIGHTS_NUMBER] = {0,2};
const uint8_t lights_output_pins_def[LIGHTS_NUMBER] = {1, D2};
uint8_t lights_output_pins_state[LIGHTS_NUMBER] = {0, 0};


//=====================================================MAC_ADDR_DEFINTION==========================================================================================================

uint8_t  device_mac[] = {0x48,0x3F,0xDA,0x7B,0x2B,0x35};
uint8_t  node_behind[] = {0xE8,0xDB,0x84,0xAE,0x3E,0x1C};
uint8_t * node_after = NULL;

//============================================================================ESP_LightNet Protocol Header=========================================================================


#define ESP_LIGHTNET_REQ_PING 0x01
#define ESP_LIGHTNET_RES_PING 0x02

#define ESP_LIGHTNET_REQ_LIGHT_UPDATE 0x03
#define ESP_LIGHTNET_RES_LIGHT_UPDATE 0x04

#define ESP_LIGHTNET_REQ_LIGHTS_STATUS 0x05
#define ESP_LIGHTNET_RES_LIGHTS_STATUS 0x06

#define ESP_LIGHTNET_PACKET_INITAL_LIFETIME 10
typedef struct ESP_lightnet_packet{
    uint8_t *  mac_source;
    uint8_t *  mac_destination;
    uint8_t identifier;
    uint8_t lifetime;
    uint8_t data_length;
    uint8_t * data;

}ESP_lightnet_packet;


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
    //esp_now_add_peer(peer_after, ESP_NOW_ROLE_COMBO, 1, NULL, 0); That s the last 
}

void loop() {



}