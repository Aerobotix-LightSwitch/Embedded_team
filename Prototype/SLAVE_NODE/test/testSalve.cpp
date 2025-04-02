#include <Arduino.h>
#include <espnow.h>
#include <ESP8266WiFi.h>




uint8_t esp32_mac_addr[] = {0x10,0x06,0x1C,0xB5,0x7F,0x6C};

void onSend(uint8_t * macaddr,uint8_t status_code){

}

void onRecv(uint8_t * macaddr,uint8_t * data,uint8_t len){
    Serial.printf("Recieved Data: %s\n", data);
}


void setup() {
    Serial.begin(115200);

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

    esp_now_add_peer(esp32_mac_addr, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
    Serial.println("ESP_NOW INIT Sucessfull");
}


#define DELAY_TIME_DELAY_MS 1000

void loop() {
    uint8_t  msg[] = "Hello From Nodemcu!";
    Serial.printf("Sending %s : %s\n",msg,esp_now_send(esp32_mac_addr,msg,sizeof(msg)) == 0 ? "Sucess":"Failed");
    delay(DELAY_TIME_DELAY_MS);
}