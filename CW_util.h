#ifndef CW_UTIL_H_
#define CW_UTIL_H_

#include <string.h>
#include <stdlib.h>

char* getLastModifyOnMac(char* string);

char* getLastModifyOnLinux(char* string);

char* stringCopy(char* origin, int startIndex, int length);

#endif