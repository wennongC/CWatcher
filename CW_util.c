#include "CW_util.h"
#include <assert.h>

#include<stdio.h>

// designed for MacOS stat command
// The second date argument is the LastModified date
/*
 * Invokers need to remember free the allocated memory manually
 */
char* getLastModifyOnMac(char* string) {
    int spaceFlag = 0;
    int countFlag = 0;
    int startIndex = 0;
    int endIndex = 0;
    
    for (int i = 0; i < strlen(string); i++) {
        if (countFlag == 2) {
            if (string[i] == '"') {
                endIndex = i-1;
                break;
            }
        } else if (spaceFlag && string[i] == '"') {
            countFlag++;
            if (countFlag == 2) startIndex = i+1;
        } else if (string[i] == ' ') {
            spaceFlag = 1;
        } else {
            spaceFlag = 0;
        }
    }

    if (startIndex != 0 && endIndex != 0) {
        int length = endIndex - startIndex + 1;
        char* lastModify = stringCopy(string, startIndex, length);
        return lastModify;
    } else return "ERROR";
}

char* getLastModifyOnLinux(char* string) {
    return "";
}

// copy part of the origin string into a new string.
/*
 * Invokers need to remember free the allocated memory manually
 */
char* stringCopy(char* origin, int startIndex, int length) {
    assert(strlen(origin) >= length); // check if the length is valid

    char* dest = (char*)malloc(sizeof(char)*(length+1));
    for (int i = 0; i < length; i++) {
        dest[i] = origin[startIndex+i];
    }
    dest[length] = '\0';
    return dest;
}