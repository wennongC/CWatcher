# CWatcher
A MacOS &amp; Linux Compatible program that automatically compile C/C++ program file when modified

# Compile
Compile the all the .c files and .h files into an executable program

# Introduction
Place the CWatcher in the same directory with the files to be monitored. 
Start the CWatcher from the terminal, and provide the filenames as arguments. The CWatcher will check the last modified date of the provided file for each 2 seconds, if something is changed, it will start GNU compiler to re-compile those files, and generate a 'a.out' executable program. The default compiler is 'gcc', but you could change it to 'g++' by providing '-cpp' when start CWatcher from the command line.

# Notice
- In the current version, CWatcher can not find related header files automatically, so you need put all the files as arguments to ensure the normal compile process. 
- By default, GNU compiler will generate the output file as 'a.out', so please ensure there is no same name file in the same directory, otherwise the older file will be OVERWROTE!
- The current version is still unstable, without reliable user input error prevention.