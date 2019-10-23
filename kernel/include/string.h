#pragma once

#include <kernel/types.h>

#ifndef NULL
#define NULL ((void*)0)
#endif

void* memset(void* buffptr, int value, size_t size);
void* memcpy(void* dest, const void* src, size_t n);

int strcmp(const char* s1, const char* s2);  
char* strcpy(char* dest, const char* src);
size_t strlen(const char* str);
char* strncpy(char* dest, const char* src, size_t n);
char* strtok(char* str, const char* delim);
