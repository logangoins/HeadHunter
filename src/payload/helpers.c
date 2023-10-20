#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char*
XOR(char* data, char* key, int datalen, int keylen)
{
    char* output = (char*)malloc(sizeof(char) * datalen);

    for (int i = 0; i < datalen; ++i) {
        output[i] = data[i] ^ key[i % keylen];
    }

    return output;
}
