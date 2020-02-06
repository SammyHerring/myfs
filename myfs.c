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

void retrieveFile(char *retrievePath, char *outputPath);
void storeFile(char *storeFilePath);
void writeFile(void);
int generateNextBlock(void);
void deleteEmptyBlocks(void);
int getFileSize(char fileName[50]);
int findNextAvailableBlock(int spaceRequired);
int checkBlockRemaining(int block);
bool checkDisk(void);
void viewDisk(void);
int fileCount(void);
void diskGenerator(void);
bool checkFile();
char *getTimeStamp(void);
char *getDateStamp(void);
char *removeExtension(const char *filename);

struct file {
    char fileName[100];
    int extent;
    int deleted;
};

int main(int argc, char *argv[]) {

    diskGenerator(); //Initialise required directories and disk manager file

    int opt;

    char *retrievePath = "";
    char *locationPath = "Output/";

    // put ':' in the starting of the
    // string so that program can
    //distinguish between '?' and ':'
    while ((opt = getopt(argc, argv, ": v r: o: s: h")) != -1) {
        switch (opt) { // NOLINT(hicpp-multiway-paths-covered)
            case 'v':
                viewDisk();
                return 0;
            case 'r':
                if (optind == 3) {
                    //Call RetrieveFile() and End
                    retrievePath = optarg;
                    retrieveFile(retrievePath, locationPath);
                    return 0;
                } else if (optind == 5) {
                    //Set Retrieve and Pass to Case 'o'
                    locationPath = optarg;
                } else {
                    LOGERROR("Script Wrapper Function Selection Error");
                }
                break;
            case 'o':
                if (optind == 3) {
                    //Only R no Output
                    LOGERRORNR("Retrieve (-r) Operation required for Output Selection Argument. For help, see man page with 'man ./myfs-help'.");
                    return 1;
                } else if (optind == 5) {
                    //Set output location path from optarg
                    //Call RetrieveFile() and End
                    retrievePath = optarg;
                    retrieveFile(retrievePath, locationPath);
                    return 0;
                } else {
                    LOGERROR("Script Wrapper Function Selection Error");
                }
                return 0;
            case 's':
                storeFile(optarg);
                return 0;
            case 'h':
                LOGINFONR("For help, see man page with 'man ./myfs-help'.");
                return 0;
            case ':':
                LOGERRORNR("Option requires a value");
                return 0;
            default:
                LOGERRORNR("Unknown arguments passed. See help with 'man ./myfs-help'");
                return 0;
        }
    }

    if (optind == 1) {
        LOGERRORNR("No argument provided. See help with 'man ./myfs-help'");
    }

    // optind is for the extra arguments
    // which are not parsed
    for (; optind < argc; optind++) {
        LOGWARNNR("Unknown additional arguments: '%s' ignored. See help with 'man ./myfs-help'.", argv[optind]);
    }

    return 0;

    //    int space = 30;
    //
    //    diskGenerator();
    //    generateNextBlock();
    //    int block = findNextAvailableBlock(space);
    //    LOGINFONR("Next Available Block with %d Bytes: BLOCK%d", space, block);
    //    deleteEmptyBlocks();
    //    viewDisk();
    //    writeFile();
    //
    //    return 0;
}

void retrieveFile(char *retrievePath, char *outputPath) {
    printf("%s\n", retrievePath);
    printf("%s\n", outputPath);
}

void storeFile(char *storeFilePath) {

    FILE* inputFile = fopen(storeFilePath, "rb");
    if (!inputFile) { LOGERROR("Cannot read store file. Exiting."); exit(EXIT_FAILURE); };

    int existingFileSize = getFileSize(storeFilePath);
    int newFileSize = existingFileSize;
    int splitCount = 0;

    LOGINFONR("%s", storeFilePath);
    LOGINFONR("%d", existingFileSize);
    LOGINFONR("%d", newFileSize);

    int block = 0;
    int remaining = 0;

    while (newFileSize > 0) {
        block = findNextAvailableBlock(1);
        remaining = checkBlockRemaining(block);
        char *newFileBlockName = "";

        LOGINFONR("Block Avaliable: %d", block);
        LOGINFONR("Block Remaining: %d", remaining);

        //Get Full New Block File Path for part of File
        char num[20];
        sprintf(num, "%d/", block);
        char name[25] = "BLOCK";
        strcat(name, num);

        char path[30] = "DISK/";
        strcat(path,name);

        newFileBlockName = removeExtension(storeFilePath);
        newFileBlockName = strrchr(newFileBlockName, '/') ? strrchr(newFileBlockName, '/') + 1 : newFileBlockName;
        strcat(path, newFileBlockName);

        char* charBuffer = path;
        int count = splitCount;
        sprintf(charBuffer, "%s%d%s",path, count, ".txt");

        newFileBlockName = charBuffer;
        LOGINFONR("BLOCK FILE NAME: %s", newFileBlockName);



        FILE* outputFile = fopen(newFileBlockName, "w");
        if (!outputFile) { LOGERROR("Cannot write store file. Exiting."); exit(EXIT_FAILURE); };

        while (remaining > 0) {

            char* buffer = (char*)malloc(512);

            size_t byteRead = fread(buffer, sizeof(char), remaining, inputFile);

            remaining -= byteRead;
            newFileSize -= byteRead;

            if (byteRead == 0) { splitCount++; break; }

            fwrite(buffer, sizeof(char), byteRead, outputFile);

        }

        fclose(outputFile);
        splitCount++;
    }

    fclose(inputFile);
    exit(EXIT_SUCCESS);
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
            LOGINFONR("%s Directory Generated.", name);
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
    int blockCount = 0;

    while (true) {
        char num[20] = "";
        sprintf(num, "%d", blockCount);
        char name[25] = "BLOCK";
        strcat(name, num);

        char path[30] = "DISK/";
        strcat(path,name);

        int byteCount = 0;

        DIR *d;
        struct dirent *dir;
        d = opendir(path);
        bool empty = false;
        if (d)
        {
            while ((dir = readdir(d)) != NULL) {
                if (dir->d_type == DT_REG) {
                    char *fileName = dir->d_name;
                    char fullPath[30] = "";
                    strcat(fullPath, path);
                    strcat(fullPath, "/");
                    strcat(fullPath, fileName);
                    int size = getFileSize(fullPath);
                    byteCount = byteCount + size;
                }
            }
            closedir(d);
            //If BLOCK empty, delete BLOCK.
            if (byteCount <= 0) {
                rmdir(path);
                LOGINFO("%s Deleted from DISK", name);
            }


        } else if (ENOENT == errno) {
            //Directory with required size does not exist. Create new block.
            //Return new block as next available.
            break;
        } else {
            //Directory detection failed. Kill script.
            LOGERROR("\'opendir()\' failed to detect %s directory. Exiting.", name);
            fprintf(stderr, "\'opendir()\' failed to detect %s directory. Exiting\n", name);
            exit(EXIT_FAILURE);
        }

        blockCount++;
    }
}

int getFileSize(char fileName[50]) {
    FILE* data;
    data = fopen(fileName, "r");

    struct stat st;
    stat(fileName, &st);
    int size = st.st_size;

    if (size < 0) {
        LOGERROR("File State Size Read Error. Exiting.");
        exit(EXIT_FAILURE);
    }

    fclose(data);

    return size;
}

int findNextAvailableBlock(int spaceRequired) {

    int blockCount = 0;

    while (true) {
        char num[20] = "";
        sprintf(num, "%d", blockCount);
        char name[25] = "BLOCK";
        strcat(name, num);

        char path[30] = "DISK/";
        strcat(path,name);

        int byteCount = 0;

        DIR *d;
        struct dirent *dir;
        d = opendir(path);
        if (d)
        {
            while ((dir = readdir(d)) != NULL) {
                if (dir->d_type == DT_REG) {
                    char *fileName = dir->d_name;
                    char fullPath[30] = "";
                    strcat(fullPath, path);
                    strcat(fullPath, "/");
                    strcat(fullPath, fileName);
                    int size = getFileSize(fullPath);
                    byteCount = byteCount + size;
//                    LOGDEBUGNR("File: %s\t|\tExtent: %d",fullPath, size);
                }
            }
            closedir(d);
            LOGINFONR("%s Total Bytes: %d", name, byteCount);

            if ((spaceRequired <= (512-byteCount)) && (byteCount < 512)) {
                return blockCount;
            }

        } else if (ENOENT == errno) {
            //Directory with required size does not exist. Create new block.
            //Return new block as next available.
            generateNextBlock();
            return blockCount;
        } else {
            //Directory detection failed. Kill script.
            LOGERROR("\'opendir()\' failed to detect %s directory. Exiting.", name);
            fprintf(stderr, "\'opendir()\' failed to detect %s directory. Exiting\n", name);
            exit(EXIT_FAILURE);
        }

        blockCount++;
    }
}

int checkBlockRemaining(int block) {
    char num[20] = "";
    sprintf(num, "%d", block);
    char name[25] = "BLOCK";
    strcat(name, num);

    char path[30] = "DISK/";
    strcat(path,name);

    int byteCount = 0;

    DIR *d;
    struct dirent *dir;
    d = opendir(path);
    if (d)
    {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_REG) {
                char *fileName = dir->d_name;
                char fullPath[30] = "";
                strcat(fullPath, path);
                strcat(fullPath, "/");
                strcat(fullPath, fileName);
                int size = getFileSize(fullPath);
                byteCount = byteCount + size;
            }
        }
        closedir(d);

        return 512-byteCount;

    } else if (ENOENT == errno) {
        //Directory with required size does not exist. Create new block.
        LOGERROR("Block%d Directly Could Not Be Found. Exiting.", block);
        exit(EXIT_FAILURE);
    } else {
        //Directory detection failed. Kill script.
        LOGERROR("\'opendir()\' failed to detect %s directory. Exiting.", name);
        fprintf(stderr, "\'opendir()\' failed to detect %s directory. Exiting\n", name);
        exit(EXIT_FAILURE);
    }
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

char *removeExtension(const char *filename) {
    size_t len = strlen(filename);
    char *newfilename = malloc(len-2);
    if (!newfilename) {
        LOGERROR("Error Removing Extension from New Block Filename. Exiting");
        exit(EXIT_FAILURE);
    }
    memcpy(newfilename, filename, len-4);
    newfilename[len - 3] = 0;
    return newfilename;
}