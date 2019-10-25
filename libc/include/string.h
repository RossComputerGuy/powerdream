#pragma once

#ifndef NULL
#define NULL ((void*)0)
#endif

#ifndef __TYPE_SIZE_T
#define __TYPE_SIZE_T
typedef unsigned int size_t;
#endif

size_t strlen(const char* str);
