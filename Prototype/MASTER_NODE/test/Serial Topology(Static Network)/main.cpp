#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

#include <stdlib.h>
#include <stdio.h>

//================================================================Memory Layout========================================================================================
// Static Network
#define STATIC_CONNECTED_NODES 3


uint8_t esp_lightnet_encryption_key[16] = {0x6a,0x66,0x55,0x69,0xc3,0x97,0x46,0xf5,0xb3,0xcc,0x25,0x9d,0x32,0xb3,0xb,0xf0};

#define ESP_LIGHTNET_ENCRYPTION_FLAG 0
#define ESP_LIGHTNET_CHANNEL 1 

uint8_t device_mac_addr[] = {0x10,0x06,0x1C,0xB5,0x7F,0x6C}; // This device esp32
uint8_t node1_mac_addr[] = {0xE8,0xDB,0x84,0xAE,0x3E,0x1C}; // node mcu
uint8_t node2_mac_addr[] = {0x48,0x3F,0xDA,0x7B,0x2B,0x35}; // generic esp8266
uint8_t * mac_list[]= {device_mac_addr,node1_mac_addr,node2_mac_addr}; 
uint8_t * light_index[] = { (uint8_t[]) {0,0} ,(uint8_t[]) {1,2}, (uint8_t[]) {3,4}};

//============================================================================Network Structure========================================================================
#define NODE_STATUS_CONNECTION 7
// #define   6
// #define   5
// #define   4
// #define   3
// #define   2
// #define   1
// #define   0
typedef struct Node{
    uint8_t * mac_addr;
    uint8_t status;
    uint8_t * lights_in_control; // IDS of the LIGHTS The MSB bit used for light state 
    uint8_t number_of_lights;
    struct Node * next_node;
}Node;

Node * node_list = NULL;

void esp_lightnet_init_master(){

    node_list = (Node *)malloc(sizeof(Node));
    node_list -> mac_addr = mac_list[0];
    node_list -> lights_in_control = 0;
    node_list -> number_of_lights = 0;
    node_list -> status = (1 << NODE_STATUS_CONNECTION);
    node_list -> next_node = NULL;

    Node * node = node_list;
    Node * tmp = NULL;

    for(int i = 1; i < STATIC_CONNECTED_NODES;i++){
        tmp = (Node *) malloc(sizeof(Node));
        tmp -> next_node = NULL;

         // Static Network
        
        tmp->mac_addr = mac_list[i];
        tmp -> number_of_lights = 2;
        tmp -> lights_in_control = light_index[i]; 
        tmp -> status = (0 << NODE_STATUS_CONNECTION);
        node -> next_node = tmp;
        node = tmp;
    }
    // Perring to the master's next Node
    node = node_list -> next_node;
    esp_now_peer_info_t peer;
    peer.channel = ESP_LIGHTNET_CHANNEL;
    peer.encrypt = ESP_LIGHTNET_ENCRYPTION_FLAG;
    memset(&peer.lmk,0,16);
    peer.ifidx = WIFI_IF_STA;
    peer.priv = NULL;
    if(esp_now_add_peer(&peer) != ESP_OK){
        Serial.println("Failed To peer to next Node, Restarting ...");
        ESP.restart();
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
Node * light_to_node(uint8_t light_number,Node * list){
    if(light_number <= 0) return NULL;
    if(light_number >= 128) light_number = light_number % 128; // MSB for State 
    Node *  node = list;
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
Node * mac_to_node(uint8_t * mac_addr,Node * list){
    if(mac_addr == NULL) return NULL;
    Node * node = list;
    while(node != NULL){
        if(memcmp(node -> mac_addr,mac_addr,6) == 0) {
            return node;
        }
        node = node -> next_node;
    }
    return NULL;
}


//============================================================================ESP_LightNet Protocol Header========================================================================

#define ESP_LIGHTNET_REQ_FAILURE 0x00
#define ESP_LIGHTNET_REQ_PING 0x01
#define ESP_LIGHTNET_RES_PING_SUCEESS 0x02
#define ESP_LIGHTNET_RES_PING_INCOMPLETE 0x03

#define ESP_LIGHTNET_REQ_LIGHT_UPDATE 0x04
#define ESP_LIGHTNET_RES_LIGHT_UPDATE 0x05

#define ESP_LIGHTNET_REQ_LIGHTS_STATUS 0x06
#define ESP_LIGHTNET_RES_LIGHTS_STATUS 0x07

#define ESP_LIGHTNET_PACKET_INITAL_LIFETIME 10
typedef struct ESP_lightnet_packet{
    uint8_t *  mac_source;
    uint8_t *  mac_destination;
    uint8_t identifier;
    uint8_t lifetime;
    uint8_t data_length;
    uint8_t * data;

}ESP_lightnet_packet;

//=================================================================ESP_LightNet Request Functions=============================================================================================

// This Function pings a node 

uint8_t esp_lightnet_request_ping(uint8_t * mac_dest){
    if(mac_dest == NULL) return 1;
    ESP_lightnet_packet packet;
    packet.mac_source = device_mac_addr;
    packet.mac_destination = mac_dest;
    packet.identifier = ESP_LIGHTNET_REQ_PING;
    packet.data = NULL;
    packet.data_length = 0;
    if(esp_now_send(node_list -> next_node -> mac_addr,(uint8_t *)&packet,sizeof(packet)) != ESP_OK){
        return 1;
    }
    return 0;
}

// This Function updates a light state of a node 

uint8_t esp_lightnet_request_light_update(uint8_t * mac_dest,uint8_t light_number,uint8_t state){
    if(mac_dest == NULL || light_number > 127 || light_number < 1) return 1;
    
    ESP_lightnet_packet packet;
    packet.mac_source = device_mac_addr;
    packet.mac_destination = mac_dest;
    packet.identifier = ESP_LIGHTNET_REQ_LIGHT_UPDATE;


    // Just one light state at a time :)
    packet.data_length = 1;
    state = state == 0 ? 0 : 1;
    light_number = (state << 7) | light_number; // light number (1 to 127)
    packet.data[0] = light_number;
    if(esp_now_send(node_list -> next_node -> mac_addr,(uint8_t *)&packet,sizeof(packet)) != ESP_OK){
        return 1;
    }
    return 0;
}

// This Function sends a light state of a node 

uint8_t esp_lightnet_request_lights_status(uint8_t * mac_dest){
    if(mac_dest == NULL) return 1;
    ESP_lightnet_packet packet;
    packet.mac_source = device_mac_addr;
    packet.mac_destination = mac_dest;
    packet.identifier = ESP_LIGHTNET_REQ_LIGHTS_STATUS;
    packet.data = NULL;
    packet.data_length = 0;
    if(esp_now_send(node_list -> next_node -> mac_addr,(uint8_t *)&packet,sizeof(packet)) != ESP_OK){
        return 1;
    }
    return 0;
}

uint8_t esp_lightnet_master_request_update(Node * list){
    if(list == NULL) return 1;
    else if(list -> next_node == NULL) return 1;
    
    Node * last_node = list;
    while(last_node -> next_node != NULL){
        last_node = last_node -> next_node;   
    }

    if(esp_lightnet_request_ping(last_node ->mac_addr) != 0) return 1;

    return 0;
}

//=================================================================ESP_LightNet Response Functions=============================================================================================
// This Function a node 

uint8_t esp_lightnet_response_ping(uint8_t * mac_src){
    Node * node =  mac_to_node(mac_src,node_list);
    if(mac_src == NULL || node == NULL) return 1;
    node -> status = NODE_STATUS_CONNECTION;
    return 0;
}

// This Function updates a light state of a node 

uint8_t esp_lightnet_master_response_light_update(uint8_t * mac_src,uint8_t * data,uint8_t data_length){
    return 0;
}

// This Function sends a light state of a node 

uint8_t esp_lightnet_master_response_lights_status(uint8_t * mac_src,uint8_t * data,uint8_t data_length){
    Node * node =  mac_to_node(mac_src,node_list);
    if(mac_src == NULL || node == NULL) return 1;
    node -> number_of_lights = data_length;
    for(uint8_t i = 0; i < data_length; i++){
        node -> lights_in_control[i] = data[i];
    }
    return 0;
}

//=====================================================================Serial User Commands Handler============================================================================================

String mac_to_str(uint8_t * mac){
    if(mac == NULL) return String("Unknown");
    String str_mac = "";
    uint8_t i;
    for(i = 0; i < 6;i++){
        str_mac = str_mac + String(mac[i],HEX) + ':'; 
    }
    str_mac.remove(str_mac.length() - 1);
    return str_mac;
}

// This is just two State (Connected or Not)
String status_to_str(uint8_t state){
    String state_str = state == (1 << NODE_STATUS_CONNECTION) ? "CONNECTED" :" NOT CONNECTED";
    return state_str;
}
String lights_to_str(uint8_t * lights , uint8_t n){
    return "";
}
void print_packet_content(ESP_lightnet_packet packet){
    String msg;
    Serial.println("\n-------------------------------------Packet Content------------------------------------------");
    msg = "mac_source: " + mac_to_str(packet.mac_source) + " mac_destination: " + mac_to_str(packet.mac_destination);
    msg = msg + " identifier: " + String(packet.identifier) + " lifetime: " + String(packet.lifetime);
    msg = msg + "data_length: " + String(packet.data_length);
    Serial.println(msg);
}
void print_network_status(Node * list){
    Node * node = list;
    uint16_t id = 0;
    Serial.println("\n-------------------------------------Network Status------------------------------------------");
    Serial.println("Connected Devices: " + String(STATIC_CONNECTED_NODES));
    while(node != NULL){
        String node_info_buffer;
        node_info_buffer = "id: " + String(id,10) + " mac_address: " + mac_to_str(node -> mac_addr);
        node_info_buffer = node_info_buffer + " Status: " + status_to_str(node -> status);
        node_info_buffer = node_info_buffer + " Light in Control " + String(node -> number_of_lights) + ": " + lights_to_str(node -> lights_in_control,node -> number_of_lights);
        Serial.println(node_info_buffer);

        node = node -> next_node;
        id++;
    }
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
//==============================================================Call Back Functions For ESPNOW======================================================================================


void esp_now_onRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len){
    Node * node = mac_to_node((uint8_t *)mac_addr,node_list);
    if(node == NULL) return;
    uint8_t status = (1 << NODE_STATUS_CONNECTION);

    ESP_lightnet_packet packet;
    memcpy(&packet,data,data_len);

    // Handle Response
    switch(packet.identifier){
        case ESP_LIGHTNET_RES_PING_SUCEESS:
            esp_lightnet_response_ping(packet.mac_source);
            break;
        case ESP_LIGHTNET_REQ_LIGHTS_STATUS:
            esp_lightnet_master_response_lights_status(packet.mac_source,packet.data,packet.data_length);
            break;
        case ESP_LIGHTNET_RES_LIGHT_UPDATE:
            esp_lightnet_master_response_light_update(packet.mac_source,packet.data,packet.data_length);
            break;
        default:
            break;
    }
} 
void esp_now_onSend(const uint8_t *mac_addr,esp_now_send_status_t  status){
    return;
} 



//=====================================================================Program Setup=================================================================================================================================
uint32_t master_update_time_ms;

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_MODE_STA);
    WiFi.disconnect();

    if(esp_now_init() != 0){
        Serial.println("Failed To Start ESP_NOW ... Restarting");
        ESP.restart();
    }
    Serial.println("ESPNOW started sucessfully");

    // This Part initialize The esp32 Static Network

    esp_now_register_recv_cb(esp_now_onRecv);
    esp_now_register_send_cb(esp_now_onSend);


    //esp_lightnet_init_master();



    master_update_time_ms = millis();
}
//=====================================================================Main Loop=================================================================================================================
#define MASTER_UPDATE_TIME_MS 1000
void loop() {
    if(Serial.available()){
        String msg_buffer = Serial.readStringUntil('\n');
        //serial_handle_user(msg_buffer);

        // Crude Serial Handler 
        if(msg_buffer[0] == 'G'){
            print_network_status(node_list);
        }
        else{
            uint8_t light_number = msg_buffer[0];
            uint8_t state = msg_buffer[0] == 'O' ? 1 : 0;
        }
    }
    if((millis() - master_update_time_ms) > MASTER_UPDATE_TIME_MS){
        char data = 'f';
        Serial.print("Sending ");
        if(esp_now_send(node1_mac_addr,(const uint8_t *)&data,1) == ESP_OK){
            Serial.print("Sucess");
        }
        Serial.println();
        master_update_time_ms = millis();
    }
}