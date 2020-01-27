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
#include <stdbool.h>

void directoryGenerator(void);
void writeFile(void);

struct file {
    int id;
    char fileName[100];
    int extent;
};

int main(int argc, char *argv[]) {

    directoryGenerator();

    writeFile();

    return 0;

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

void writeFile() {
    FILE *diskManagerIn;
    FILE *diskManagerOut;

    struct file writeFile = {0, "test.txt", 100};
    struct file input;

    // Check if Disk Manager File Exists
    diskManagerIn = fopen("DISK/diskManager.dat", "r");

    if (diskManagerIn == NULL) {
        fclose(diskManagerIn);

        //Disk Manager File Does Not Exist
        LOGWARN("Disk Manager Data File Does Not Exist");

        //Create and Open Disk Manager Data File
        diskManagerIn = fopen("DISK/diskManager.dat", "w");
        LOGINFO("Creating Disk Manager Data File");

        if (diskManagerIn == NULL) {
            LOGERROR("Disk Manager Data File Creation Error");
            fclose(diskManagerIn);
            exit(EXIT_FAILURE);
        } else {
            //Write file to Disk manager
            fwrite(&writeFile, sizeof(struct file), 1, diskManagerIn);
            if (&fwrite != 0) {
                LOGINFO("File Successfully Written to Disk Manager");
                fclose(diskManagerIn);
            } else {
                LOGERROR("File Failed to Write Disk Manger");
                fclose(diskManagerIn);
                exit(EXIT_FAILURE);
            }
        }

    } else {
        fclose(diskManagerIn);

        //Disk Manager Data File Exists
        diskManagerOut = fopen("DISK/diskManager.dat", "r");
        LOGINFO("Disk Manager Data File Exists");

        if (diskManagerOut == NULL) {
            LOGERROR("Disk Manager Data File Write Error");
            exit(EXIT_FAILURE);
        } else {
            LOGINFO("Disk Manager Data File Read");

            bool found = false; //Boolean to identify whether file exists
            struct file disk[1000];

            LOGINFO("--\tFiles on DISK\t--");
            LOGINFO("--\tSTART\t--");
            // Read Disk Manager Data File
            while(fread(&input, sizeof(struct file), 1, diskManagerOut)) {
                LOGINFO("File (%d) on DISK: %s | Extent: %d", input.id, input.fileName, input.extent);

                if (strcmp(input.fileName, writeFile.fileName) == 0 && input.id == writeFile.id) {
                    LOGINFO("HTTING1");
                    found = true;
                } else {
                    LOGINFO("HTTING2");
                    struct file fileInput;
                    fileInput.id =  input.id;
                    strcpy(fileInput.fileName, input.fileName);
                    fileInput.extent = input.extent;
                    disk[input.id] = fileInput;
                }
            }
            LOGINFO("--\tEND\t--");

            fclose(diskManagerOut);

            if (found) {
                //File already exists, abandon write file.
                LOGWARN("File with name '%s' already exists. Exiting.", writeFile.fileName);
                exit(EXIT_FAILURE);
            } else {
                //File does not already exist, write File
                disk[writeFile.id] = writeFile;

                diskManagerIn = fopen("DISK/diskManager.dat", "w");

                if (diskManagerIn == NULL) {
                    LOGERROR("Disk Manager Data File Write Error");
                    fclose(diskManagerIn);
                    exit(EXIT_FAILURE);
                } else {
                    //Write file to Disk manager

                    fwrite(&disk, sizeof(disk), 1, diskManagerIn);
                    if (&fwrite != 0) {
                        LOGINFO("File Successfully Written to Disk Manager");
                        fclose(diskManagerIn);
                        exit(EXIT_SUCCESS);
                    } else {
                        LOGERROR("File Failed to Write Disk Manger");
                        fclose(diskManagerIn);
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }

        fclose(diskManagerOut);
    }
}

void viewDisk() {
    FILE* diskManager;
    struct file input;

    diskManager = fopen("DISK/diskManager.dat", "r");

    if (diskManager == NULL) {
        LOGERROR("Disk Manager Data File Read Error");
        exit(EXIT_FAILURE);
    } else {
        LOGINFO("Disk Manager Data File Exists");

        LOGINFO("--\tFiles on DISK\t--");
        LOGINFO("--\tSTART\t--");
        // Read Disk Manager Data File
        while (fread(&input, sizeof(struct file), 1, diskManager)) {
            LOGINFO("File (%d) on DISK: %s | Extent: %d", input.id, input.fileName, input.extent);
        }
        LOGINFO("--\tEND\t--");
    }
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