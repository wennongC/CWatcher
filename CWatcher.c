/*
*   Program Name: CWatcher
*   Author: Wennong Cai
*   Created At: August 26th, 2019
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

typedef struct {
    char *filename;
    char *info;
    char *lastModified;
} FileItem;

// Record the Operating System. 1 for Linux, 2 for Darwin(MacOS); 0 for unknown
int OS = 0;

void help();
void getArguments(Arguments*);  // Analyse the arguments read from the user command line input
void freeArgMemory(Arguments*); // clean out the memory allocation
void printArguments(Arguments*); // For debuging use
int detectOS(); // check the OS name
int getFileInfo(FileItem*); // find the file info in 'ls -l' command
int detectModify(FileItem*);
int callCompiler(Arguments*);

int main(int argc, char **argv) {
    // pre-process
    Arguments bundle;
    bundle.argc = argc;
    bundle.argv = argv;
    getArguments(&bundle);

    if (bundle.fileNum > 0) {
        printArguments(&bundle);
    
        if (detectOS()) {
            // Failed to recognise the OS name
            freeArgMemory(&bundle);
            return 1;
        }

        // Need to decide how many files will be watched

        FileItem *files = (FileItem*)malloc(sizeof(FileItem) * bundle.fileNum);
        for (int i = 0; i < bundle.fileNum; i++) {
            files[i].filename = bundle.filenames[i];
            getFileInfo(&files[i]);
        }
        free(files);

        callCompiler(&bundle);
    } else {
        help();
    }

    // Free memory that been allocated inside "getArguments" function
    freeArgMemory(&bundle);
    return 0;
}

// Display the help documentation in the console 
void help() {
    // Wait to be coded
    printf("Thanks for using the CWatcher program\n");
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

void freeArgMemory(Arguments* ptr) {
    if (ptr->fileNum != 0) free(ptr->filenames);
    if (ptr->optionNum != 0) free(ptr->options);
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

// This function is only used by detectOS()
void detectOSErrorMsg() {
    fprintf(stderr, "ERROR >>> Unable to get the operating system name\n");
    fprintf(stderr, "ERROR >>> Please note that this program only support Darwin(MacOS) and Linux currently.\n");
}
// Test the Operating System
// Return 1 means error occurred.
int detectOS() {
    const int BUFSIZE = 32;
    char buf[BUFSIZE];
    FILE *fp;
    if ((fp = popen("uname", "r")) == NULL) {
        detectOSErrorMsg();
        return 1;
    }

    if (fgets(buf, BUFSIZE, fp) != NULL) {
        int length = strlen(buf);
        printf("Operating System Detected >>> %s ", buf);
        if ( length>=5 &&buf[0]=='L'&&buf[1]=='i'&&buf[2]=='n'&&buf[3]=='u'&&buf[4]=='x') {
            OS = 1;
            printf("CWatcher pre-process setting to >>> Linux\n");
        } else if ( length>=6 &&buf[0]=='D'&&buf[1]=='a'&&buf[2]=='r'&&buf[3]=='w'&&buf[4]=='i'&&buf[5]=='n') {
            OS = 2;
            printf("CWatcher pre-process setting to >>> Darwin\n");
        } else {
            detectOSErrorMsg();
            pclose(fp);
            return 1;
        }
    }

    if(pclose(fp))  {
        fprintf(stderr, "WARNING >>> File closing is encountering some problems\n");
        return 1;
    }
    return 0;
}

// Get information of a specified file
// Return -1 means error. 0 for success.
int getFileInfo(FileItem* item_ptr) {
    const int BUFSIZE = 128;
    char buf[BUFSIZE];
    FILE *fp;
    char *filename = item_ptr->filename;

    // Try to get the FileSystem list first by using command 'ls'
    if ((fp = popen("ls -l", "r")) == NULL) {
        fprintf(stderr, "ERROR >>> Unable to access file system\n");
        return -1;
    }

    int fileFoundFlag = 0;
    while (fgets(buf, BUFSIZE, fp) != NULL) {
        // First check if the "buf" includes the filename or not
        if (strstr(buf, filename) == NULL) continue;
        else {
            fileFoundFlag = 1; // Set it to 1 Temporarily, then do further check
            int totalLength = strlen(buf) - 1;
            int fileLength = strlen(filename);
            /*
                Debug part:
                printf("OK >>> buf = %s\n", buf); printf("   >>> totalLength = %d\n", totalLength);
                printf("OK >>> file = %s\n", filename); printf("   >>> fileLength = %d\n", fileLength);
            */
            for (int i = 0; i < fileLength; i++) {
                if(buf[totalLength-fileLength+i] != filename[i]) {
                    // if they are not exactly same filename
                    fileFoundFlag = 0;
                    break;
                }
            }
        }
        // If the exact same name file is found, terminate the while loop
        if (fileFoundFlag) {
            item_ptr->info = buf;
            break;
        };
    }

    if (!fileFoundFlag) {
        fprintf(stderr, "ERROR >>> Unable to find the file: %s\n", filename);
        pclose(fp);
        return -1;
    }

    if(pclose(fp))  {
        fprintf(stderr, "WARNING >>> Failed to close file\n");
        return -1;
    }

    return 0;
}

// Detect if the file has been made modifed
int detectModify(FileItem* items) {



    return 0;
}

// Do the compile work
int callCompiler(Arguments* ptr) {
    if (ptr->fileNum == 0) return 1;

    int filesLen = 0;
    for (int i = 0; i < ptr->fileNum; i++) {
        filesLen += strlen(ptr->filenames[i]);
    }

    char *cmd = (char*)malloc(sizeof(char) * (filesLen + ptr->fileNum + 4));
    strcpy(cmd, "gcc ");
    
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