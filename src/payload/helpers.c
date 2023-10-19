#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

char* XOR(char* data, char* key, int datalen, int keylen) {
    char* output = (char*)malloc(sizeof(char) * datalen);
    for (int i = 0; i < datalen; ++i){
	output[i] = data[i] ^ key[i % keylen];
    }
    return output;    

}

int str_starts_with(char *a, char*b){
    for(int i = 0; i < strlen(b); i++){
        if (a[i] != b[i])
            return 0;
    }
    return 1;
}

