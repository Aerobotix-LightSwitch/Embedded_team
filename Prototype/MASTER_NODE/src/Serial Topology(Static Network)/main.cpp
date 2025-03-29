#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

#include <stdlib.h>
#include <stdio.h>

#define STATIC_CONNECTED_NODES 2


//============================================================================ESP-LightNet Protocol Header========================================================================


#define READ_WRITE_FLAG_BIT  7
// #define   6
// #define   5
// #define   4
// #define   3
// #define   2
// #define   1
// #define   0

//================================================================Memory Layout============================================================================
// Static Network
const uint8_t node1_mac_addr[] = {0xE8,0xDB,0x84,0xAE,0x3E,0x1C}; // node mcu
const uint8_t node2_mac_addr[] = {0x48,0x3F,0xDA,0x7B,0x2B,0x35}; // generic esp8266
const uint8_t * mac_list[]= {node1_mac_addr,node2_mac_addr}; 

//============================================================================Network Structure========================================================================

typedef struct Node{
    uint8_t * mac_addr;
    uint8_t status;
    struct Node * next_Node;
}Node;

void node_send(){

}

typedef struct Network{
    int connected_nodes;
    Node * List; 
}Network;

void init_static_network(){
    Network *  network = (Network *) malloc(sizeof(Network));
    network -> connected_nodes = STATIC_CONNECTED_NODES;
    for(int i = 0; i < STATIC_CONNECTED_NODES;i++){
        Node * node = (Node *) malloc(sizeof(Node));
        node->mac_addr =  
    }
}
void print_network_status(){

}
//==============================================================Call Back Functions For ESPNOW=====================================================================


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

    // This Part initialize The esp32 Static Network
    
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