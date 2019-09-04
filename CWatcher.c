/*
*   Program Name: CWatcher
*   Author: Wennong Cai
*   Created At: August 26th, 2019
*/

#include <unistd.h>
#include <stdio.h>
#include "CW_util.h"

// Record the Operating System. 1 for Linux, 2 for Darwin(MacOS); 0 for unknown
int OS = 0;

void help();
void getArguments(Arguments*);  // Analyse the arguments read from the user command line input
void freeArgMemory(Arguments*); // clean out the memory allocation
void printArguments(Arguments*); // For debuging use
int detectOS(); // check the OS name
int getFileInfo(FileItem*); // find the file info in 'ls -l' command
int getLastModified(FileItem*); // find LastModified of a file in 'stat' command
int detectModify(Arguments*, FileItem*);
int callCompiler(Arguments*);

int main(int argc, char **argv) {
    // pre-process. Initialize the Arguments structure
    Arguments bundle = { 
        .filenames = NULL,
        .fileNum = 0,
        .options = NULL,
        .optionNum = 0,
        .argc = 0,
        .argv = NULL,
    };
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

        // <<< Need to decide how many files will be watched here >>>

        FileItem *files = (FileItem*)malloc(sizeof(FileItem) * bundle.fileNum);
        initFileItems(files, bundle.fileNum);
        
        while (1) {
            int result = detectModify(&bundle, files);
            printf("Detect Result >>> %d \n", result);
            fflush(stdout);
            if (result == -1) break;
            else sleep(2);
        }

        free(files);
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
        printf("Operating System Detected >>> %s", buf);
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
/*
*   <<< Consider using 'stat' in this function to extract "Last Data modification"
*   <<< In MacOS, Four date components in 'stat' are "Last access", "Last modification", "Last status change", "CreateAt" >>>
*/
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

            for (int i = 0; i < fileLength; i++) {
                if(buf[totalLength-fileLength+i] != filename[i]) {
                    // if they are not exactly same filename
                    fileFoundFlag = 0;
                    break;
                }
            }
        }
        // If the exact same name file is found, extract the information from "stat"
        // and then terminate the while loop
        if (fileFoundFlag) {
            if (item_ptr->info != NULL) free(item_ptr->info);// Clean the memory allocation first
            item_ptr->info = stringCopy(buf, 0, strlen(buf));
            int result = getLastModified(item_ptr);
            if (result == -1) return -1;
            break;
        };
    }

    if (!fileFoundFlag) {
        fprintf(stderr, "ERROR >>> Unable to find the file: %s\n", filename);
        pclose(fp);
        return -1;
    }

    pclose(fp);
    return 0;
}

// Extract the last modified information from a existing file,
// Return -1 for error
int getLastModified(FileItem* item_ptr) {
    const int BUFSIZE = 1024;
    char buf[BUFSIZE];
    FILE *fp;
    char *cmd = (char*)malloc(sizeof(char) * (strlen(item_ptr->filename) + 6));
    strcpy(cmd, "stat ");
    strcat(cmd, item_ptr->filename);
    // Try to get the file status first by using command 'stat'
    if ((fp = popen(cmd, "r")) == NULL) {
        fprintf(stderr, "ERROR >>> Unable to access file system\n");
        return -1;
    }
    free(cmd);

    while (fgets(buf, BUFSIZE, fp) != NULL) {
        if (OS == 1) { // Linux
            if (item_ptr->lastModified != NULL) free(item_ptr->lastModified);// Clean the memory allocation first
            item_ptr->lastModified = getLastModifyOnLinux(buf); // try to get lastModify information
            if (item_ptr->lastModified == NULL) continue;
            else break;
        } else if (OS == 2) { // Mac
            if (item_ptr->lastModified != NULL) free(item_ptr->lastModified);// Clean the memory allocation first
            item_ptr->lastModified = getLastModifyOnMac(buf);
            break;
        } else {
            fprintf(stderr, "ERROR >>> Unable to access file system\n");
            pclose(fp);
            return -1;
        }
    }
    pclose(fp);
    return 0;
}

/*
*   Detect if the file has been made modifed.
*   The second argument is an array containing all files to be watched.
*   This function returns 1 if there changed in file, and returns 0 for non-changed.
*       Return -1 for Error occurred.
*/
int detectModify(Arguments* ptr, FileItem* files) {
    int detected = 0; // Default value is "not changed"
    for (int i = 0; i < ptr->fileNum; i++) {
        if (files[i].filename == NULL) {
            files[i].filename = stringCopy(ptr->filenames[i], 0, strlen(ptr->filenames[i]));
            if (getFileInfo(&files[i]) == -1) return -1;
            detected = 1; // The file initial process always involves changes
        } else if (strcmp(files[i].filename, ptr->filenames[i]) == 0) {
            // save the last information into a temporary variable, then Update the file information
            int length = strlen(files[i].info);
            char *temp = stringCopy(files[i].info, 0, strlen(files[i].info));

            if (getFileInfo(&files[i]) == -1) return -1;

            if (strcmp(temp, files[i].info) != 0) {
                printf("file modification detected >>> Starting to re-compile the program...\n");
                detected = 1;
            }
            free(temp);
        } else {
            fprintf(stderr, "ERROR >>> Filenames did not match the monitor record\n");
            return -1;
        }
    }

    return detected;
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
