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


