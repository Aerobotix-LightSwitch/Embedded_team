#include <Arduino.h>
#include <espnow.h>
#include <ESP8266WiFi.h>


//=====================================================Node Ligths============================================================================ 

uint8_t light_number = 2;


uint8_t lights_input_pin[] = {D5,D6};
uint8_t lights_input_state[] = {0, 0};

uint8_t lights_output_pin[] = {D1, D2};
uint8_t lights_state[] = {0 , 0};

// This Function Update the light state of a node if something changes from external input pin it return 1  
uint8_t node_update_lights(uint8_t * lights_input,uint8_t * lights_ouput,uint8_t * input_state,uint8_t * lights_state,uint8_t light_number){
    uint8_t flag = 0;
    for(uint8_t i = 0; i < light_number;i++){
        uint8_t state = digitalRead(lights_input[i]);

        // This For external user Toggling Updates
        if(state != input_state[i]){
            input_state[i] = state;
            lights_state[i] = (lights_state[i] & 0x7F) | (state << 7); // set msb to the state
            digitalWrite(lights_ouput[i],state);
            flag = 1;
        }
    }
    return flag;
}

//=====================================================MAC_ADDR_DEFINTION==============================================================================

uint8_t device_mac_addr[] = {0xE8,0xDB,0x84,0xAE,0x3E,0x1C};
uint8_t node_behind_mac_addr[] = {0x10,0x06,0x1C,0xB5,0x7F,0x6C};
uint8_t node_after_mac_addr[] = {0x48,0x3F,0xDA,0x7B,0x2B,0x35};


//============================================================================ESP_LightNet Protocol Header========================================================================


#define ESP_LIGHTNET_REQ_FAILURE 0x00
#define ESP_LIGHTNET_REQ_PING 0x01
#define ESP_LIGHTNET_RES_PING_SUCCESS 0x02

#define ESP_LIGHTNET_REQ_LIGHT_UPDATE 0x03
#define ESP_LIGHTNET_RES_LIGHT_UPDATE 0x04

#define ESP_LIGHTNET_REQ_LIGHTS_STATUS 0x05
#define ESP_LIGHTNET_RES_LIGHTS_STATUS 0x06

#define ESP_LIGHTNET_PACKET_INITAL_LIFETIME 10

typedef struct ESP_lightnet_packet{
    uint8_t mac_source[6];
    uint8_t mac_destination[6];
    uint8_t identifier;
    uint8_t lifetime;
    uint8_t data_length;
    uint8_t * data;
}ESP_lightnet_packet;


//=================================================================ESP_LightNet Node Request Functions=============================================================================================


//=================================================================ESP_LightNet Node Response Functions=============================================================================================

// This Function Response a ping from a master

uint8_t esp_lightnet_node_response_ping(uint8_t * dest_mac){
    if(dest_mac == NULL) return 1;
    ESP_lightnet_packet packet;
    memcpy(packet.mac_source,device_mac_addr,6);
    memcpy(packet.mac_destination,dest_mac,6);
    packet.identifier = ESP_LIGHTNET_RES_PING_SUCCESS;
    packet.data_length = 0;
    packet.data = NULL;

    return esp_now_send((unsigned char *)node_behind_mac_addr,(uint8_t *)&packet,sizeof(packet));
}   

// This Function updates the local light state of a node and Responses the master with success

uint8_t esp_lightnet_node_response_light_update(uint8_t * dest_mac,ESP_lightnet_packet request_packet){
    if(dest_mac == NULL) return 1;
    // update the light_state
    uint8_t light_number = request_packet.data[0];
    uint8_t state = (light_number & 0x80) != 0; // extract state
    light_number = light_number & 0x7F; // extract local id 
    digitalWrite(lights_output_pin[light_number],state);
    
    // Send response with update Successful 
    ESP_lightnet_packet response_packet;
    response_packet.identifier = ESP_LIGHTNET_RES_LIGHT_UPDATE;
    memcpy(response_packet.mac_source,device_mac_addr,6);
    memcpy(response_packet.mac_destination,dest_mac,6);
    response_packet.lifetime = 0;

    response_packet.data = NULL;
    response_packet.data_length = 0;
    return esp_now_send(node_behind_mac_addr,(uint8_t *)&response_packet,sizeof(response_packet));
}

uint8_t esp_lightnet_node_response_light_status(uint8_t * dest_mac){
    if(dest_mac == NULL) return 1; 
    ESP_lightnet_packet packet;
    

    return 0;
}
//=================================================================Node Debuging Functions===================================================================================================

String byte_to_str(uint8_t * data ,uint16_t data_length){
    if(data == NULL || data_length <= 0) return "";
    String str = "{";
    char buffer[2];
    for(uint16_t i = 0; i < data_length; i++){
        snprintf(buffer,sizeof(buffer),"%02x",data[i]);
        str = str + String(buffer) + ",";
    }
    str[(str.length() - 1)] = '}';
    return str;
}

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

void print_packet_content(ESP_lightnet_packet packet){
    String msg;
    Serial.println("\n-------------------------------------Packet Content------------------------------------------");
    msg = "mac_source: " + mac_to_str(packet.mac_source) + " mac_destination: " + mac_to_str(packet.mac_destination);
    msg = msg + " identifier: " + String(packet.identifier) + " lifetime: " + String(packet.lifetime);
    msg = msg + " data_length: " + String(packet.data_length) + " data: " + byte_to_str(packet.data,packet.data_length);
    Serial.println(msg);
}
uint8_t esp_lightnet_node_response_recieve_data(uint8_t * macaddr,uint8_t * data,uint8_t len){
    ESP_lightnet_packet packet;
    memcpy(&packet,data,len);
    print_packet_content(packet);
    return 0;
}

//==============================================================Node Call Back Functions For ESPNOW======================================================================================

void onSend(uint8_t * macaddr,uint8_t status_code){
    return;
}

void onRecv(uint8_t * macaddr,uint8_t * data,uint8_t len){
    ESP_lightnet_packet packet;
    memcpy(&packet,data,len);

    //print_packet_content(packet);


    // if this packet is for that node take it and use it
    if(memcmp(packet.mac_destination,device_mac_addr,6) == 0){


        esp_lightnet_node_response_recieve_data(macaddr,data,len);
        // switch(packet.identifier){
        //     case ESP_LIGHTNET_REQ_PING:
        //         esp_lightnet_node_response_ping(packet.mac_source);
        //         break;
        //     case ESP_LIGHTNET_REQ_LIGHT_UPDATE:
        //         esp_lightnet_node_response_light_update(packet.mac_source,packet);
        //         break;
        //     case ESP_LIGHTNET_REQ_LIGHTS_STATUS:
        //         break;
        //     default:
        //         break;
        // }
    }
    // OtherWise Forward it to one way
    else{
        // from node to master (Master Response)
        if(memcmp(macaddr,node_behind_mac_addr,6)){
            esp_now_send((unsigned char *)node_after_mac_addr,data,len);
        }
        // master to node (Master Request)
        else if(memcmp(macaddr,node_after_mac_addr,6)){
            esp_now_send((unsigned char *)node_behind_mac_addr,data,len);
        }
        else{
            Serial.println("Error");
        }
    }
}


//====================================================================================Node Setup===================================================================================
void setup() {
    Serial.begin(115200);

    // init the local lights state and their corresponding pins 
    for(uint8_t i = 0; i < light_number; i++){
        pinMode(lights_input_pin[i],INPUT);
        pinMode(lights_output_pin[i],OUTPUT);
        digitalWrite(lights_output_pin[i],0);
    }


    // Set Up ESPNOW for the node 

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

    esp_now_add_peer((unsigned char *)node_behind_mac_addr, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
    esp_now_add_peer((unsigned char *)node_after_mac_addr, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
}
//====================================================================================Node Main Loop===================================================================================

void loop() {

    if(node_update_lights(lights_input_pin,lights_output_pin,lights_input_state,lights_state,light_number) == 1){
        Serial.println("Something Changed in the System");
    }

}