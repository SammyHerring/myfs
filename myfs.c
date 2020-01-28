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

void diskGenerator(void);
void viewDisk(void);
void writeFile(void);
bool checkDisk(void);
bool checkFile();
int fileCount(void);
int generateNextBlock(void);
int findNextAvaliableBlock(int spaceRequired);

struct file {
    char fileName[100];
    int extent;
    int deleted;
};

int main(int argc, char *argv[]) {

    int space = 50;

    diskGenerator();
    //generateNextBlock();
    findNextAvaliableBlock(space);
    viewDisk();
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

    FILE *diskManager;
    struct file input;

    struct file writeFile = {"test.txt", 100, 0};

    if (!checkDisk()) {
        LOGERROR("Disk Manager Data File Does Not Exist");
        exit(EXIT_FAILURE);
    } else {
        if (checkFile(writeFile)) {
            exit(EXIT_FAILURE);
        } else {
            LOGINFONR("--\tWRITING TO DISK\t--");

            FILE* data;
            if ( (data = fopen("DISK/diskManager.dat", "a")) == NULL )
            {
                printf("Error opening file\n");
                exit(EXIT_FAILURE);
            }

            fwrite(&writeFile, sizeof(struct file) * 1, 1, data);
            fclose(data);

            exit(EXIT_SUCCESS);
        }
    }
}

int generateNextBlock() {
    int blockCount = 0;
    while (true) {
        char num[20];
        sprintf(num, "%d", blockCount);
        char name[25] = "BLOCK";
        strcat(name, num);

        char path[30] = "DISK/";
        strcat(path,name);

        DIR *directory = opendir(path);
        if (directory) {
            //Directory exists. Increase count and loop.
            closedir(directory);
            blockCount++;
        } else if (ENOENT == errno) {
            //Directory does not exist. Create 'BLOCKX' directory.
            mkdir(path, 0777);
            LOGINFO("%s Directory Generated", name);
            return blockCount;
        } else {
            //Directory detection failed. Kill script.
            LOGERROR("\'opendir()\' failed to detect %s directory. Exiting.", name);
            fprintf(stderr, "\'opendir()\' failed to detect %s directory. Exiting\n", name);
            exit(EXIT_FAILURE);
        }
    }
}

void deleteEmptyBlocks() {

}

int getFileSize(char fileName[50]) {
    FILE* data;
    data = fopen(fileName, "r");

    struct stat st;
    stat(fileName, &st);
    int size = st.st_size;

    fclose(data);

    return size;
}

int findNextAvaliableBlock(int spaceRequired) {

    int blockCount = 0;

    while (true) {
        char num[20] = "";
        sprintf(num, "%d", blockCount);
        char name[25] = "BLOCK";
        strcat(name, num);

        char path[30] = "DISK/";
        strcat(path,name);

        DIR *d;
        struct dirent *dir;
        d = opendir(path);
        if (d) {
            while ((dir = readdir(d)) != NULL) {
                if (dir->d_type == DT_REG) {
                    char *fileName = dir->d_name;
                    char fullPath[30] = "";
                    strcat(fullPath, path);
                    strcat(fullPath, "/");
                    strcat(fullPath, fileName);
                    int size = getFileSize(fullPath);
                    LOGINFONR("File: %s\t|\tExtent: %d",fullPath, size);
                }
            }
            closedir(d);
        }

        blockCount++;
        break;
    }
    return 1;
}

bool checkDisk() {
    FILE* diskManager = fopen("DISK/diskManager.dat", "r");
    if (diskManager == NULL) {
        fclose(diskManager);
        return false;
    } else {
        fclose(diskManager);
        return true;
    }
}

void viewDisk() {

    if (!checkDisk()) {
        LOGERROR("Disk Manager Data File Does Not Exist");
    } else {
        LOGINFO("Disk Manager Data File Exists");

        FILE* diskManager = fopen("DISK/diskManager.dat", "r");

        int fileCount = 0;
        struct file input;

        LOGINFONR("--\tFiles on DISK\t--");
        LOGINFONR("--\tSTART DIR\t--");
        // Read Disk Manager Data File
        while (fread(&input, sizeof(struct file), 1, diskManager)) {
            LOGINFONR("File (%d) on DISK: %s\t|\tExtent: %d", fileCount, input.fileName, input.extent);
            fileCount++;
        }
        if (fileCount == 0) LOGINFONR("NO FILES FOUND");
        LOGINFONR("--\tEND DIR\t\t--\t|\tFile Count: %d", fileCount);
        fclose(diskManager);
    }
}

int fileCount() {
    int fileCount = 0;

    if (!checkDisk()) {
        LOGERROR("Disk Manager Data File Does Not Exist. Exiting.");
        exit(EXIT_FAILURE);
    } else {
        FILE* diskManager;
        diskManager = fopen("DISK/diskManager.dat", "r");

        struct file input;

        // Read Disk Manager Data File
        while (fread(&input, sizeof(struct file), 1, diskManager)) {
            fileCount++;
        }

        return fileCount;
    }
}

void diskGenerator() {

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

    //Ensure Output Directory exists
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

    // Check if Disk Manager Data File Exists
    FILE* diskManager;
    diskManager = fopen("DISK/diskManager.dat", "r");

    if (diskManager == NULL) {
        fclose(diskManager);

        //Disk Manager File Does Not Exist
        LOGWARN("Disk Manager Data File Does Not Exist");

        //Create Disk Manager Data File
        diskManager = fopen("DISK/diskManager.dat", "w");

        if (diskManager == NULL) {
            LOGERROR("Disk Manager Data File Creation Error");
            fclose(diskManager);
            exit(EXIT_FAILURE);
        } else {
            LOGINFO("Disk Manager Data File Generated");
            fclose(diskManager);
        }
    }
}

bool checkFile(struct file writeFile) {
    bool found = false;

    if (checkDisk()) {

        FILE* diskManager = fopen("DISK/diskManager.dat", "r");

        struct file input;

        // Read Disk Manager Data File
        while (fread(&input, sizeof(struct file), 1, diskManager)) {

            //Check if File with Filename or ID Already Exists
            if (strcmp(input.fileName, writeFile.fileName) == 0) {
                LOGWARN("File with Filename '%s' already exists.", writeFile.fileName);
                found = true;
            }
        }

        fclose(diskManager);

    } else {
        LOGERROR("Disk Manager Data File Does Not Exist. Exiting.");
        exit(EXIT_FAILURE);
    }

    return found;
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