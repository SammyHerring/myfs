/*
 * Name: 	MCOMD3PST ASSIGNMENT 2 | logger.h
 * Purpose:	File System Simulation
 * Author:	Samuel Steven David Herring
 * UserID:	sh1042
 * Creation Date: 23/01/2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <logger.h>

void directoryGenerator(void);

struct file {
    int id;
    char *fileName;
    int extent;
    int blocks[100];
};

int main(int argc, char *argv[]) {
    directoryGenerator();

    FILE *diskManager;

    // Open Disk Manager Data File
    diskManager = fopen("DISK/diskManager.dat", "w");
    if (diskManager == NULL) {
        LOGERROR("Disk Manager Data File Error");
        return 1;
    }

    struct file testFile = {1, "test.txt", 100, {1}};

    // Write testFile to Disk Manager
    fwrite(&testFile, sizeof(struct file), 1, diskManager);

    if (&fwrite != 0)
        LOGINFO("File written to Disk Manager");
    else
        LOGERROR("File not written to Disk Maanger");

    fclose(diskManager);

    int opt;

    // put ':' in the starting of the 
    // string so that program can  
    //distinguish between '?' and ':'  
    while ((opt = getopt(argc, argv, ":if:lrx")) != -1) {
        switch (opt) { // NOLINT(hicpp-multiway-paths-covered)
            case 'i':
            case 'l':
            case 'r':
                printf("Option: %c\n", opt);
                break;
            case 'f':
                printf("Filename: %s\n", optarg);
                break;
            case ':':
                printf("Option requires a value\n");
                break;
            case '?':
                printf("Unknown Option: %c\n", optopt);
                break;
        }
    }

    // optind is for the extra arguments 
    // which are not parsed
    for (; optind < argc; optind++) {
        printf("extra arguments: %s\n", argv[optind]);
    }

    return 0;
}

void directoryGenerator() {

    //Ensure Logs Directory exists
    DIR *log = opendir("Logs");
    if (log) {
        //Directory exists. Proceed with log creation.
        closedir(log);
    } else if (ENOENT == errno) {
        //Directory does not exist. Create logs directory.
        mkdir("Logs", 0777);
        LOGINFO("Logs Directory Generated");
    } else {
        //Directory detection failed. Kill script.
        LOGERROR("\'opendir()\' failed to detect Logs directory. Exiting.");
        fprintf(stderr, "\'opendir()\' failed to detect Logs directory. Exiting.\n");
        exit(EXIT_FAILURE);
    }

    //Ensure DISK Directory exists
    DIR *disk = opendir("DISK");
    if (disk) {
        //Directory exists. Proceed with log creation.
        closedir(disk);
    } else if (ENOENT == errno) {
        //Directory does not exist. Create logs directory.
        mkdir("DISK", 0777);
        LOGINFO("DISK Directory Generated");
    } else {
        //Directory detection failed. Kill script.
        LOGERROR("\'opendir()\' failed to detect DISK directory. Exiting.");
        fprintf(stderr, "\'opendir()\' failed to detect DISK directory. Exiting.\n");
        exit(EXIT_FAILURE);
    }

    //Ensure DISK Directory exists
    DIR *output = opendir("Output");
    if (output) {
        //Directory exists. Proceed with log creation.
        closedir(output);
    } else if (ENOENT == errno) {
        //Directory does not exist. Create logs directory.
        mkdir("Output", 0777);
        LOGINFO("Output Directory Generated");
    } else {
        //Directory detection failed. Kill script.
        LOGERROR("\'opendir()\' failed to detect Output directory. Exiting.");
        fprintf(stderr, "\'opendir()\' failed to detect Output directory. Exiting.\n");
        exit(EXIT_FAILURE);
    }
}

char *getTimeStamp(void) {
    time_t t = time(NULL);

    static char result[21];
    strftime(result, sizeof(result), "[%d/%m/%Y %T]", localtime(&t));

    return result;
}

char *getDateStamp(void) {
    time_t t = time(NULL);

    static char result[20];
    strftime(result, sizeof(result), "%Y-%m-%d", localtime(&t));

    return result;
}