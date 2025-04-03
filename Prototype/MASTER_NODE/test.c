#include <stdio.h>

#define PACKET_BASE_SIZE 15
#define ESP_NOW_MAX_DATA_LEN 250
typedef struct ESP_lightnet_packet{
    unsigned char mac_source[6];
    unsigned char mac_destination[6];
    unsigned char identifier;
    unsigned char lifetime;
    unsigned char data_length;
    unsigned char data[ESP_NOW_MAX_DATA_LEN - PACKET_BASE_SIZE];
}ESP_lightnet_packet;

int main(){
    ESP_lightnet_packet packet;
    
    printf("%d",sizeof(packet));
}