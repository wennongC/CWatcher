#include "CW_compiler.h"

// Do the compile work
int callCompiler(Arguments* ptr) {
    if (ptr->fileNum == 0) return 1;

    int filesLen = 0;
    for (int i = 0; i < ptr->fileNum; i++) {
        filesLen += strlen(ptr->filenames[i]);
    }

    char *cmd = (char*)malloc(sizeof(char) * (filesLen + ptr->fileNum + 4));
    if (isCpp(ptr->options, ptr->optionNum)) {
        strcpy(cmd, "g++ "); // Invoke compiler for C++ language
    } else {
        strcpy(cmd, "gcc "); // Invoke compiler for C language
    }
    
    for (int i = 0; i < ptr->fileNum; i++) {
        strcat(cmd, ptr->filenames[i]);
        strcat(cmd, " ");
    }
    
    printf("Executing command >>> %s \n", cmd);
    system(cmd);
    free(cmd);
    printf("\n==========\n\nComplie >>> Completed!\n\n==========\n\n");
    return 0;
}

// If the options contain '-cpp' return 1, otherwise 0;
int isCpp(char** options, int num) {
    for (int i = 0; i < num; i++) {
        if (strcmp(options[i], "-cpp") == 0) {
            return 1;
        }
    }
    return 0;
}