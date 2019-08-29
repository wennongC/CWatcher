/*
*   Program Name: CWatcher
*   Author: Wennong Cai
*   Created At: August 26th, 2019
*   
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char **filenames;
    int fileNum;
    char **options;
    int optionNum;
    int argc;
    char **argv;
} Arguments;

void help();
void getArguments(Arguments*);
void printArguments(Arguments*);
int detectModify();

int main(int argc, char **argv) {
    Arguments bundle;
    bundle.argc = argc;
    bundle.argv = argv;
    getArguments(&bundle);
    printArguments(&bundle);

    // Free memory that been allocated inside "getArguments" function
    free(bundle.filenames);
    free(bundle.options);
    return 0;
}

// Display the help documentation in the console 
void help() {

}

// Get all arguments from the input
// Will insert all inputs into a struct Arguments by handling the pointer
void getArguments(Arguments* ptr) {
    ptr->fileNum = 0; 
    ptr->optionNum = 0;
    char **filenames = (char**)malloc(sizeof(char*) * (ptr->argc-1));
    char **options = (char**)malloc(sizeof(char*) * (ptr->argc-1));

    // extract the arguments into 2 temporary arrays
    // The first argument is the program name, hence could be skipped
    for (int i = 1; i < ptr->argc; i++) {
        char *arg = ptr->argv[i];
        int len = strlen(arg);
        if(len == 0) continue;
        if(arg[0] == '-') {
            options[ptr->optionNum] = arg;
            ptr->optionNum++;
        } else {
            filenames[ptr->fileNum] = arg;
            ptr->fileNum++;
        }
    }

    // copy the filenames to the struct Arguments
    if (ptr->fileNum != 0) {
        ptr->filenames = (char**)malloc(sizeof(char*) * ptr->fileNum);
        for (int i = 0; i < ptr->fileNum; i++) {
            ptr->filenames[i] = filenames[i];
        }
    }
    // copy the options to the struct Arguments
    if (ptr->optionNum != 0) {
        ptr->options = (char**)malloc(sizeof(char*) * ptr->optionNum);
        for (int i = 0; i < ptr->optionNum; i++) {
            ptr->options[i] = options[i];
        }
    }

    // free the allocated temporary memory
    free(filenames);
    free(options);
}

// Print out all the arguments
void printArguments(Arguments* ptr) {
    printf("The filenames arguments contains: \n");
    for(int i = 0; i < ptr->fileNum; i++) {
        printf("| %s |", ptr->filenames[i]);
    }

    printf("\nThe options arguments contains: \n");
    for(int i = 0; i < ptr->optionNum; i++) {
        printf("| %s |", ptr->options[i]);
    }
    printf("\n");
}

// Detect if the file has been modified.
// Return 1 if the file did be modified, otherwise 0. 
int detectModify() {
    return 0;
}