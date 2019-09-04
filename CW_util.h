#ifndef CW_UTIL_H_
#define CW_UTIL_H_

#include <string.h>
#include <stdlib.h>

typedef struct {
    char **filenames;
    int fileNum;
    char **options;
    int optionNum;
    int argc;
    char **argv;
} Arguments;

typedef struct {
    char *filename;
    char *info;
    char *lastModified;
} FileItem;

char* getLastModifyOnMac(char* string);

char* getLastModifyOnLinux(char* string);

char* stringCopy(char* origin, int startIndex, int length);

void initFileItems(FileItem* items, int length);

#endif