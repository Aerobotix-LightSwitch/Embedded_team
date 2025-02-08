#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h> 

bool toogle = false;
uint32_t count_bytes = 0;
uint32_t second_ms = 0;
float speed_kbs = 0;
void onrecv(uint8_t * mac,uint8_t * data,uint8_t len){
    count_bytes = count_bytes + len;
}

void setup() {
    Serial.begin(115200);
    WiFi.disconnect();
    WiFi.setOutputPower(20.5);
    WiFi.mode(WIFI_STA);
    if(esp_now_init() != 0){
        Serial.println("Failed to init espnow Restarting ..");
        ESP.restart();
    }
    esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
    esp_now_register_recv_cb(onrecv);
    second_ms = millis();
}
#define PRINT_TIME_MS 1000
void loop() {
    if(millis() - second_ms > PRINT_TIME_MS){
        speed_kbs = count_bytes / 1024.0;  
        Serial.print("Speed is :");
        Serial.print(speed_kbs);
        Serial.println("KB/s");
        count_bytes = 0;
        second_ms = millis();      
      }
}

