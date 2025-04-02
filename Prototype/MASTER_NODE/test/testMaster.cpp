#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>




uint8_t nodemcu_mac_addr[] = {0xE8,0xDB,0x84,0xAE,0x3E,0x1C};

void onSend(const uint8_t *mac_addr, esp_now_send_status_t status){

}

void onRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len){
    Serial.printf("Recieved Data: %s\n", data);
}


void setup() {
    Serial.begin(115200);

    WiFi.mode(WIFI_STA);      
    WiFi.disconnect();

    if(esp_now_init() != 0){
        Serial.print("Failed to init the espnow .. Restarting the system\n");
        ESP.restart();
    }
    esp_now_register_recv_cb(onRecv);
    esp_now_register_send_cb(onSend);

    esp_now_peer_info_t peer;
    peer.channel = 1;
    peer.encrypt = 0;
    peer.ifidx = WIFI_IF_STA;
    peer.priv = NULL;
    memcpy(peer.peer_addr, nodemcu_mac_addr,6);
    esp_now_add_peer(&peer);
    Serial.println("ESP_NOW INIT Sucessfull");
}


#define DELAY_TIME_DELAY_MS 1000

void loop() {
    uint8_t  msg[] = "Hello From Esp32!";
    Serial.printf("Sending %s : %s\n",msg,esp_now_send(nodemcu_mac_addr,msg,sizeof(msg)) == 0 ? "Sucess":"Failed");
    delay(DELAY_TIME_DELAY_MS);
}