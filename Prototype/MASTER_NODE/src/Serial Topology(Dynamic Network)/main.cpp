#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
//============================================================================Header Protocol========================================================================
#define CONNECTED_FLAG 1




//============================================================================Network Structure========================================================================



typedef struct Node{
    uint8_t * mac_addr;
    uint8_t status;
    struct Node * next_Node;
}Node;

void node_send(){

}

typedef struct Network{
    int number_of_connected_nodes;
    Node * List; 
}Network;


void print_network_status(){

}

//==============================================================Call Back Functions For ESPNOW==========================================================


void esp_now_onRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len){

} 
void esp_now_onSend(const uint8_t *mac_addr,esp_now_send_status_t  status){
    
} 


void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_MODE_STA);

    WiFi.disconnect();


    Serial.print("ESP32 with mac : ");
    Serial.println(WiFi.macAddress());

    if(esp_now_init() != 0){
        Serial.println("Failed To Start ESP_NOW ... Restarting");
        ESP.restart();
    }

    esp_now_register_recv_cb(esp_now_onRecv);
    esp_now_register_send_cb(esp_now_onSend);


}

void loop() {
    if(Serial.available()){
        String msg_buffer = Serial.readStringUntil('\n');
        if(msg_buffer[0] == 'G'){
            // Print Network Status
            print_network_status();
        }
        else if(msg_buffer[0] == 'P'){
            int light  = msg_buffer.substring(1,msg_buffer.indexOf(' ')).toInt();
            msg_buffer.toUpperCase();
            bool state =  msg_buffer.indexOf('ON') != -1;

            // Update NODE
        }
    }
}