#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define KEY_SIZE_IN_BYTES 16
#define KEY_NAME "ESP_LIGHTNET_ENCRYPTION_KEY"

int main(){

    srand(time(NULL));

    unsigned char random_byte = 0;
    unsigned char i;
    printf("\n#define %s  {0x%x",KEY_NAME,random_byte);
    
    for(i = 1;i < KEY_SIZE_IN_BYTES;i++){
        random_byte = 0;
        printf(",0x%x",random_byte);
    }
    printf("}\n");
    return 0;
}