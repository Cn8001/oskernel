#include "string.h"
int strlen(const char* ptr){
    int len = 0;
    while(*ptr != 0){
        len++;
        ptr++;
    }
    return len;
}

int strnlen(const char* string,int max){
    int i=0;
    for(i=0;i<max;i++){
        if(string[i] == 0){
            break;
        }
    }
    return i;
}

bool isdigit(char c){
    return c >=48 && c<=57;
}

int tonumericdigit(char c){
    return c-48;
}

char* strcpy(char* dest, const char* src){
    char* tmp = dest;
    while(*src != 0){
        *dest = *src;
        src += 1;
        dest += 1;
    }
    *dest = 0x00;
    return tmp;
}