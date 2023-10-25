#include <stdio.h>
#include <string.h>
//#include <arpa/inet.h>
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
            return 1;
    }
    return 0;
}

char *split(char *str, const char *delim)
{
    char *p = strstr(str, delim);

    if (p == NULL) return NULL;     // delimiter not found

    *p = '\0';                      // terminate string after head
    return p + strlen(delim);       // return tail substring
}

char* newline_terminator(char* buffer){
    for(int i = 0; i < strlen(buffer); i++){
        if(buffer[i] == '\n'){
            buffer[i+1] = '\0';
            break;
        }
    }

    return buffer;
}

