#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define KEY_SIZE_IN_BYTES 16
#define KEY_NAME "ESP_LIGHTNET_ENCRYPTION_KEY"


char random_char(){
    int offset = 'z' - 'A' + 1;
    return rand() % offset + 'A';
}


int main(){

    srand(time(NULL));

    unsigned char i;
    char generated_key[16];
    
    
    for(i = 0;i < KEY_SIZE_IN_BYTES;i++){
        generated_key[i] = random_char();
    }

    // Hex Format
    printf("#define %s {0x%x",KEY_NAME,generated_key[0]);
    for(i = 1 ; i < KEY_SIZE_IN_BYTES;i++){
        printf(",0x%x",generated_key[i]);
    }
    printf("}\n");

    //String Format

    printf("#define %s \"",KEY_NAME);
    for(i = 0 ; i < KEY_SIZE_IN_BYTES;i++){
        printf("%c",generated_key[i]);
    }
    printf("\"\n");

    return 0;
}