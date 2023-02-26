#ifndef STRING_H
#define STRING_H
#include <stdbool.h>

int strlen(const char* ptr);
bool isdigit(char c);
int tonumericdigit(char c);
int strnlen(const char* string,int max);
#endif