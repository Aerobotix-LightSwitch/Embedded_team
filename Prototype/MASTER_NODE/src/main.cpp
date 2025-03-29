#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

#include <stdlib.h>
#include <stdio.h>

#define STATIC_CONNECTED_NODES 3
//================================================================Memory Layout========================================================================================
// Static Network

uint8_t device_mac_addr[] = {0x10,0x06,0x1C,0xB5,0x7F,0x6C}; // This device esp32
uint8_t node1_mac_addr[] = {0xE8,0xDB,0x84,0xAE,0x3E,0x1C}; // node mcu
uint8_t node2_mac_addr[] = {0x48,0x3F,0xDA,0x7B,0x2B,0x35}; // generic esp8266
uint8_t * mac_list[]= {device_mac_addr,node1_mac_addr,node2_mac_addr}; 


//============================================================================ESP_LightNet Protocol Header and Handlers========================================================================


#define ESP_LIGHTNET_PING 0x01
#define ESP_LIGHTNET_NODE_LIGHTS_ON 0x02
#define ESP_LIGHTNET_NODE_TURN_LIGHTS_OFF 0x03
#define ESP_LIGHTNET_NODE_LIGHTS_STATUS 0x04

// #define   6
// #define   5
// #define   4
// #define   3
// #define   2
// #define   1
// #define   0

typedef struct ESP_lightnet_packet{
    uint8_t *  mac_source;
    uint8_t *  mac_destination;
    uint8_t command;
    uint8_t * data;
}ESP_lightnet_packet;


uint8_t ping_node(uint8_t * mac_dest){
    if(mac_dest == NULL) return 1;
    ESP_lightnet_packet packet;
    packet.mac_source = device_mac_addr;
    packet.mac_destination = mac_dest;
    packet.command = ESP_LIGHTNET_PING;
    packet.data = NULL;
    
    if(esp_now_send(mac_dest,(uint8_t *)&packet,sizeof(packet)) != ESP_OK){
        return 1;
    }
    return 0;
}

uint8_t light_on_node(uint8_t * mac){

}
uint8_t light_off_node(uint8_t * mac){

}
uint8_t lights_status_node(uint8_t * mac){

}

//============================================================================Network Structure========================================================================

typedef struct Node{
    uint8_t * mac_addr;
    uint8_t status;
    uint8_t * lights_in_control;
    uint8_t number_of_lights;
    struct Node * next_node;
}Node;


typedef struct Network{
    int network_hosts;
    Node * List; 
}Network;

Node * node_list = NULL;
Network *  network = NULL;


void init_network(){

    network = (Network *) malloc(sizeof(Network));
    network -> network_hosts = STATIC_CONNECTED_NODES;
    network -> List = node_list;

    node_list = (Node *)malloc(sizeof(Node));
    node_list -> mac_addr = mac_list[0];
    node_list -> lights_in_control = 0;
    Node * node = node_list;
    Node * tmp = NULL;

    for(int i = 1; i < STATIC_CONNECTED_NODES;i++){
        tmp = (Node *) malloc(sizeof(Node));
        tmp -> next_node = NULL;
        tmp->mac_addr = mac_list[i];
        tmp->lights_in_control = NULL;
        tmp -> number_of_lights = 0;

        node -> next_node = tmp;
        node = tmp;
    }
}




// uint8_t send_from_node(uint8_t command,uint8_t * mac_addr){
//     switch (command){
//         case ESP_LIGHTNET_PING:
//             ping_host(mac_addr);
//             break;
//         case ESP_LIGHTNET_TURN_LIGHTS_ON:
//             light_on_node(mac_addr);
//             break;

//         case ESP_LIGHTNET_TURN_LIGHTS_OFF:
//             light_off_node(mac_addr);
//             break;

//         case ESP_LIGHTNET_TURN_LIGHTS_STATUS:
//             lights_status(mac_addr);
//             break;
//         default:
//             return -1;
//     } 
//     return 0;
// }



//=============================================================Node mapping Functions=============================================================================


// This Function takes a light Number and returns the node in control
Node * light_to_node(uint16_t light_number){
    if(light_number <= 0) return NULL;
    Node *  node = node_list;
    while(node != NULL){
        for(int i = 0; i < node -> number_of_lights;i++){
            if(node -> lights_in_control[i] == light_number){
                return node;
            }
        }
        node = node -> next_node;
    }
    return NULL;
}


// This Function takes a mac address and returns its corresponding node
Node * mac_to_node(uint8_t * mac_addr){
    if(mac_addr == NULL) return NULL;
    Node * node = node_list;
    while(node != NULL){
        if(memcmp(node -> mac_addr,mac_addr,6) == 0) {
            return node;
        }
        node = node -> next_node;
    }
    return NULL;
}

//==============================================================Call Back Functions For ESPNOW=====================================================================


void esp_now_onRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len){

} 
void esp_now_onSend(const uint8_t *mac_addr,esp_now_send_status_t  status){
    
} 

//=====================================================================Serial User Commands Handler============================================================================================
void print_network_status(){
    
}

void serial_handle_user(String msg_buffer){
    if(msg_buffer[0] == 'G'){
        // Print Network Status
        //print_network_status();
    }
    else if(msg_buffer[0] == 'P'){

        // Update NODE
    }
}



//=====================================================================Program Setup=================================================================================================================================
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


    // This Part initialize The esp32 Static Network

    esp_now_register_recv_cb(esp_now_onRecv);
    esp_now_register_send_cb(esp_now_onSend);


    init_network();
    
}
//=====================================================================Main Loop=================================================================================================================================

void loop() {
    if(Serial.available()){
        String msg_buffer = Serial.readStringUntil('\n');
        serial_handle_user(msg_buffer);
    }
}