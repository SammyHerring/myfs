/*
 * Name: 	MCOMD3PST ASSIGNMENT 2 | ./myfs
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

// --- START    || GLOBALS ---
//Global Variables
int BLOCKLIMIT = 512; //Maximum permitted Block Size (Unit: Bytes)

int instanceCount;

// --- START    || FILE DEFINITIONS ---
//Main File Functions
void storeFile(char *storeFilePath);

void retrieveFile(char *retrievePath, char *outputPath);

void deleteFile(char *deleteFileName);

//Block Manager Functions
bool storeFileBlock(char *storeFilePath);

bool retrieveFileBlock(char *retrievePath, char *outputPath);

bool deleteFileBlock(char *deleteFileName);

//Utilities
//Disk Operations
void diskGenerator(void);

//Block Operations
int generateNextBlock();

bool deleteEmptyBlocks();

int getFileSize(char fileName[50]);

int findNextAvailableBlock(int spaceRequired);

int countNumberOfBlocks();

int checkBlockRemaining(int block);
//File Operations
bool checkFileExist(char *fileName);

int countFilesRecursively(char *fileName, char *basePath, bool outermost);
//Validation and Text Manipulation Operations
bool checkFileValid(char *filePath);

char *removeFilePath(char *fullName);

char *getFilenameExtension(char *filename);

char *removetxtExtension(char *filename);

char *removeElementIdentiferExtension(char *fileNameID);

//Time Stamp Operations
char *getTimeStamp(void);

char *getDateStamp(void);
// --- END      || FILE DEFINITIONS ---
// --- END      || GLOBALS ---

// --- START    || MAIN FILE FUNCTIONS ---
int main(int argc, char *argv[]) {
    diskGenerator(); //Initialise required directories and disk manager file

    int opt;

    char *retrievePath;
    char *locationPath = "Output/";

    while ((opt = getopt(argc, argv, ": r: o: s: d: h")) != -1) {
        switch (opt) { // NOLINT(hicpp-multiway-paths-covered)
            case 's':
                //Check requested file is valid
                storeFile(optarg);
                return 0;
            case 'r':
                if (argc == 3) {
                    //Call RetrieveFile() and End
                    retrievePath = optarg;
                    retrieveFile(retrievePath, locationPath);
                    return 0;
                } else if (argc == 5) {
                    //Set Retrieve and Pass to Case 'o'
                    retrievePath = optarg;
                } else {
                    LOGERROR("Script Wrapper Function Selection Error");
                }
                break;
            case 'o':
                if (argc == 2) {
                    //Only Output Function Requested - no Retrieval Operation
                    LOGERRORNR(
                            "Retrieve (-r) Operation required for Output Selection Argument. For help, see man page with 'man ./myfs-help'.");
                    return 1;
                } else if (argc == 3) {
                    //Only R no Output
                    LOGERRORNR(
                            "Retrieve (-r) Operation required for Output Selection Argument. For help, see man page with 'man ./myfs-help'.");
                    return 1;
                } else if (argc == 5) {
                    //Set output location path from optarg
                    //Call RetrieveFile() and End
                    locationPath = optarg;
                    retrieveFile(retrievePath, locationPath);
                    return 0;
                } else {
                    LOGERROR("Script Wrapper Function Selection Error");
                }
                return 0;
            case 'd':
                deleteFile(optarg);
                return 0;
            case 'h':
                LOGINFONR("For help, see man page with 'man ./myfs-help'.");
                return 0;
            case ':':
                LOGERROR("Requested File System mode requires an argument. See help with 'man ./myfs-help'");
                return 0;
            default:
                LOGERROR("Unknown arguments passed. See help with 'man ./myfs-help'");
                return 0;
        }
    }

    if (optind == 1) {
        LOGERROR("No argument provided. See help with 'man ./myfs-help'");
    }

    // Optind used to consider secondary arguments. Secondary arguments not parsed.
    for (; optind < argc; optind++) {
        LOGWARN("Unknown additional arguments: '%s' ignored. See help with 'man ./myfs-help'.", argv[optind]);
    }

    return 0;
}

//Store File Function from Menu
void storeFile(char *storeFilePath) {
    if (checkFileValid(storeFilePath)) {
        LOGINFONR("File System -- Store");
        if (storeFileBlock(storeFilePath)) {
            LOGINFO("Storing File %s Succeeded.", storeFilePath);
            exit(EXIT_SUCCESS);
        } else {
            LOGWARN("Storing File %s Failed.", storeFilePath);
            exit(EXIT_FAILURE);
        }
    } else {
        LOGWARN("Invalid File Detected. Exiting.");
    }
}

//Retrieve File Function from Menu
void retrieveFile(char *retrievePath, char *outputPath) {
    if (checkFileValid(retrievePath)) {
        LOGINFONR("File System -- Retrieve");
        if (retrieveFileBlock(retrievePath, outputPath)) {
            LOGINFO("Retrieving File %s.txt Succeeded.", removeElementIdentiferExtension(retrievePath));
            exit(EXIT_SUCCESS);
        } else {
            LOGWARN("Retrieving File %s Failed.", retrievePath);
            exit(EXIT_FAILURE);
        }
    } else {
        LOGWARN("Invalid File Detected. Exiting.");
    }
}

//Delete File Function from Menu
void deleteFile(char *deleteFileName) {
    if (checkFileValid(deleteFileName)) {
        LOGINFONR("File System -- Delete");
        if (deleteFileBlock(deleteFileName)) {
            LOGINFO("Deleting File %s Succeeded.", deleteFileName);
            exit(EXIT_SUCCESS);
        } else {
            LOGWARN("Deleting File %s Failed.", deleteFileName);
            exit(EXIT_FAILURE);
        }
    } else {
        LOGWARN("Invalid File Detected. Exiting.");
    }
}
// --- END      || MAIN FILE FUNCTIONS ---

// -- START     || BLOCK MANAGER FUNCTIONS ---
//Store File to Block
bool storeFileBlock(char *storeFilePath) {

    LOGINFO("File : %s", storeFilePath);

    //Check if file already exists in DISK and Overwrite if required
    char *fileCheck = removeFilePath(storeFilePath);

    if (checkFileExist(fileCheck)) {
        LOGINFO("Overwriting File: %s.txt", fileCheck);
        if (deleteFileBlock(fileCheck)) {
            LOGINFONR("RECORD Cleanup successful.");
        } else {
            LOGERRORNR("RECORD Cleanup not successful.");
            return false;
        }
    }

    //Add file extension if not exists
    if (strcmp(getFilenameExtension(storeFilePath), "txt") != 0) {
        strcat(storeFilePath, ".txt");
    }

    LOGINFONR("%s", storeFilePath);

    FILE *inputFile = fopen(storeFilePath, "rb");
    if (!inputFile) {
        LOGERROR("Cannot read store file. Exiting.");
        exit(EXIT_FAILURE);
    };

    //File Stats
    int existingFileSize = getFileSize(storeFilePath);
    int newFileSize = existingFileSize;
    int splitCount = 0;

    //Per Block Stats
    int block = 0;
    int remaining = 0;

    while (newFileSize > 0) {
        block = findNextAvailableBlock(512); //Find next full empty block. May be adjusted dependant on FileSystem.
        remaining = checkBlockRemaining(block);
        char *newFileBlockName;

        //Get Full New Block File Path for given part of File
        //Example: fileName-[0..n].txt
        char num[20];
        sprintf(num, "%d/", block);
        char name[25] = "BLOCK";
        strcat(name, num);

        char path[30] = "DISK/";
        strcat(path, name);

        newFileBlockName = removetxtExtension(storeFilePath);
        newFileBlockName = strrchr(newFileBlockName, '/') ? strrchr(newFileBlockName, '/') + 1 : newFileBlockName;
        strcat(path, newFileBlockName);

        char *charBuffer = path;
        int count = splitCount;
        sprintf(charBuffer, "%s-%d%s", path, count,
                ".txt"); //Adds dash to distinguish between file name and file element count number

        newFileBlockName = charBuffer;
        if (splitCount == 0) LOGINFONR("GENERATED BLOCKS:");
        LOGINFONR("\t--> BLOCK: %s", newFileBlockName);

        //Writes new partial file byte by byte until Block limit is reached
        FILE *outputFile = fopen(newFileBlockName, "w");
        if (!outputFile) {
            LOGERROR("Cannot write store file. Exiting.");
            return false;
        };

        while (remaining > 0) {

            char *buffer = (char *) malloc(BLOCKLIMIT);

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
    return true;
}

//Retrieve File from Blocks
bool retrieveFileBlock(char *retrievePath, char *outputPath) {

    //Check required file exists
    if (!checkFileExist(retrievePath)) {
        LOGWARNNR("File %s does not exist.", retrievePath);
        return false;
    }

    //Add file extension if not exists
    if (strcmp(getFilenameExtension(retrievePath), "txt") != 0) {
        strcat(retrievePath, ".txt");
    }

    //If no output selected, append file name and extension to default file path
    char *target = malloc(strlen(outputPath) + strlen(retrievePath) + 1);
    if (strcmp(outputPath, "Output/") == 0) {
        strcpy(target, outputPath);
        strcat(target, retrievePath);
    } else {
        strcat(target, outputPath);
    }

    //Open file to write to
    FILE *outputFile = fopen(target, "w");
    if (!outputFile) {
        LOGERROR("Cannot write to output path. Exiting.");
        return false;
    };

    int blockCount = 0; //Index of blocks
    int elementCount = countFilesRecursively(retrievePath, "DISK", true); //Count of file elements
    int elementIndex = 0; //Index of file elements

    if (blockCount == 0) LOGDEBUGNR("DETECTED BLOCKS:");


    while (elementIndex < elementCount) {
        LOGINFONR("Index: %d\tCount: %d", elementIndex, elementCount);

        char *fileRetrieve = retrievePath;

        //START -- FIND ELEMENT FILE

        while (true) {
            char num[20] = "";
            sprintf(num, "%d", blockCount);
            char name[25] = "BLOCK";
            strcat(name, num);

            char path[30] = "DISK/";
            strcat(path, name);

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

                        char *blockElementFileName = removetxtExtension(fileName);
                        char *requestedFileName = removetxtExtension(fileRetrieve);

                        char requestedIndex[100] = "";
                        sprintf(requestedIndex, "-%d", elementIndex);

                        requestedFileName = removeElementIdentiferExtension(requestedFileName);
                        strcat(requestedFileName, requestedIndex);

                        if (strcmp(blockElementFileName, requestedFileName) == 0) {
                            LOGDEBUGNR("\t--> DETECTED: %s", fullPath);
                            elementIndex++;
                            blockCount++;
                            break;
                        } else {
                            blockCount++;
                        }
                    }
                }
                closedir(d);
                break;
            } else if (ENOENT == errno) {
                //Directory does not exist. Block search ends.
                if (blockCount >= countFilesRecursively("BLOCK", "DISK", true)) {
                    break;
                }
                blockCount++;
                continue;
            } else {
                //Directory detection failed. Kill script.
                LOGERROR("\'opendir()\' failed to detect %s directory. Exiting.", name);
                fprintf(stderr, "\'opendir()\' failed to detect %s directory. Exiting\n", name);
                return false;
            }
        }

        //If all blocks searched for next possible element without success break loop
        if (blockCount >= countFilesRecursively("BLOCK", "DISK", true)) {
            break;
        }

        //END -- FIND ELEMENT FILE
    }

    fclose(outputFile);
    return true;
}

//Delete File from Blocks
bool deleteFileBlock(char *deleteFileName) {
    int blockCount = 0;

    if (!checkFileExist(deleteFileName)) {
        LOGWARN("File %s does not exist.", deleteFileName);
        return false;
    }

    while (true) {
        char num[20] = "";
        sprintf(num, "%d", blockCount);
        char name[25] = "BLOCK";
        strcat(name, num);

        char path[30] = "DISK/";
        strcat(path, name);

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

                    char *blockElementFileName = removeElementIdentiferExtension(fileName);
                    char *requestedFileName = removetxtExtension(deleteFileName);

                    if (blockCount == 0) LOGDEBUGNR("DELETED BLOCKS:");

                    if (strcmp(blockElementFileName, requestedFileName) == 0) {
                        remove(fullPath);
                        LOGDEBUGNR("\t--> DELETED: %s", fullPath);
                    }
                }
            }
            closedir(d);
        } else if (ENOENT == errno) {
            //Directory does not exist. Block search ends.

            if (blockCount >= countFilesRecursively("BLOCK", "DISK", true)) {
                break;
            }
            blockCount++;
            continue;
        } else {
            //Directory detection failed. Kill script.
            LOGERROR("\'opendir()\' failed to detect %s directory. Exiting.", name);
            fprintf(stderr, "\'opendir()\' failed to detect %s directory. Exiting\n", name);
            return false;
        }

        blockCount++;
    }

    if (deleteEmptyBlocks()) {
        LOGINFONR("BLOCK Cleanup successful.");
    } else {
        LOGWARNNR("BLOCK Cleanup unsuccessful.");
    }

    return true;
}
// -- END       || BLOCK MANAGER FUNCTIONS ---

// --- START    ||  UTILITIES ---
// --- START    ||  DISK OPERATIONS ---
//Generate all required directories
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
}
// --- END      ||  DISK OPERATIONS ---

// --- START    || BLOCK OPERATIONS ---
//Generate next block
int generateNextBlock() {
    int blockCount = 0;
    while (true) {
        char num[20];
        sprintf(num, "%d", blockCount);
        char name[25] = "BLOCK";
        strcat(name, num);

        char path[30] = "DISK/";
        strcat(path, name);

        DIR *directory = opendir(path);
        if (directory) {
            //Directory exists. Increase count and loop.
            closedir(directory);
            blockCount++;
        } else if (ENOENT == errno) {
            //Directory does not exist. Create 'BLOCKX' directory.
            mkdir(path, 0777);
            //LOGINFONR("%s Directory Generated.", name); //Commented for console simplicity. Use for verbose out.
            return blockCount;
        } else {
            //Directory detection failed. Kill script.
            LOGERROR("\'opendir()\' failed to detect %s directory. Exiting.", name);
            fprintf(stderr, "\'opendir()\' failed to detect %s directory. Exiting\n", name);
            exit(EXIT_FAILURE);
        }
    }
}
//Delete empty blocks
bool deleteEmptyBlocks() {
    int blockCount = 0;

    while (true) {
        char num[20] = "";
        sprintf(num, "%d", blockCount);
        char name[25] = "BLOCK";
        strcat(name, num);

        char path[30] = "DISK/";
        strcat(path, name);

        int byteCount = 0;

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
                    byteCount = byteCount + size;
                }
            }
            closedir(d);
            //If BLOCK empty, delete BLOCK.
            if (byteCount <= 0) {
                rmdir(path);
                //LOGINFONR("%s Directory Deleted", name); //Commented for console simplicity. Use for verbose out.
            }

        } else if (ENOENT == errno) {
            //Block does not exist
            if (blockCount >= countFilesRecursively("BLOCK", "DISK", true)) {
                break;
            }

        } else {
            //Directory detection failed. Kill script.
            LOGERROR("\'opendir()\' failed to detect %s directory. Exiting.", name);
            fprintf(stderr, "\'opendir()\' failed to detect %s directory. Exiting\n", name);
            return false;
        }

        blockCount++;
    }
    return true;
}

//Get a file size in bytes
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

//Find next available block with required space
int findNextAvailableBlock(int spaceRequired) {

    int blockCount = 0;

    while (true) {
        char num[20] = "";
        sprintf(num, "%d", blockCount);
        char name[25] = "BLOCK";
        strcat(name, num);

        char path[30] = "DISK/";
        strcat(path, name);

        int byteCount = 0;

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
                    byteCount = byteCount + size;
                }
            }
            closedir(d);

            if ((spaceRequired <= (BLOCKLIMIT - byteCount)) && (byteCount < BLOCKLIMIT)) {
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

//Count number of blocks
int countNumberOfBlocks() {

    int blockCount = 0;

    while (true) {
        char num[20] = "";
        sprintf(num, "%d", blockCount);
        char name[25] = "BLOCK";
        strcat(name, num);

        char path[30] = "DISK/";
        strcat(path, name);

        int byteCount = 0;

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
                    byteCount = byteCount + size;
                }
            }
            closedir(d);

        } else if (ENOENT == errno) {
            //Directory does not exist. Return current block count as number of blocks.
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

//Check bytes remaining in block
int checkBlockRemaining(int block) {
    char num[20] = "";
    sprintf(num, "%d", block);
    char name[25] = "BLOCK";
    strcat(name, num);

    char path[30] = "DISK/";
    strcat(path, name);

    int byteCount = 0;

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
                byteCount = byteCount + size;
            }
        }
        closedir(d);

        return BLOCKLIMIT - byteCount;

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
// --- END      || BLOCK OPERATIONS ---

// --- START    || FILE OPERATIONS ---
//Check if file exists in Block
bool checkFileExist(char *fileName) {
    if (countFilesRecursively(fileName, "DISK", true) > 0) {
        return true;
    }
    return false;
}

//Count files recursively
int countFilesRecursively(char *fileNameCount, char *basePath, bool outermost) {
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    if (outermost) {
        int instanceCount = 0;
    }

    //Check if item is directory
    if (!dir)
        return 0;

    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {

            char *file = removeElementIdentiferExtension(dp->d_name); //Get generic file name
            char *compareFile = removetxtExtension(fileNameCount);


            if (strcmp(file, compareFile) == 0) instanceCount++;

            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);

            countFilesRecursively(fileNameCount, path, false);
        }
    }

    closedir(dir);
    return instanceCount;
}
// --- END      || FILE OPERATIONS ---

// --- START    ||  VALIDATION OPERATIONS ---
//Check file is of type '.txt' and has no dashes in name
bool checkFileValid(char *filePath) {
    //Check file is of type '.txt'
    if (strcmp(getFilenameExtension(removeFilePath(filePath)), "txt") == 0) {
        //Check file does not have any dashes in file name
        if (!(strstr(removeFilePath(filePath), "-"))) {
            return true;
        } else {
            LOGWARN("Invalid File Name: '%s' cannot haves dashes (-) in name.", filePath);
            return false;
        }
    } else {
        LOGWARN("Invalid File Type: '%s' is not of type '.txt'.", filePath);
        return false;
    }

}

//Remove path from file name
char *removeFilePath(char *fullName) {
    return strrchr(fullName, '/') ? strrchr(fullName, '/') + 1 : fullName;
}

//Check for .txt file extension
char *getFilenameExtension(char *filename) {
    char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "";
    return dot + 1;
}

//Removes the .txt file extension of filename manipulation
char *removetxtExtension(char *fileName) {
    char *fname = fileName;
    char *end = fname + strlen(fname);

    while (end > fname && *end != '.' && *end != '\\' && *end != '/') {
        --end;
    }
    if ((end > fname && *end == '.') &&
        (*(end - 1) != '\\' && *(end - 1) != '/')) {
        *end = '\0';
    }
    return fname;
}

//Removes the file element identifier from the fileName for validation
char *removeElementIdentiferExtension(char *fileNameID) {
    char *ptr;

    ptr = strchr(fileNameID, '-');
    if (ptr != NULL) {
        *ptr = '\0';
    }

    return fileNameID;
}
// --- END      ||  VALIDATION OPERATIONS ---

// --- START    || TIME STAMP OPERATIONS ---
//Return String with Formatted Date and Time
char *getTimeStamp(void) {
    time_t t = time(NULL);

    static char result[21];
    strftime(result, sizeof(result), "[%d/%m/%Y %T]", localtime(&t));

    return result;
}

//Return String with Formatted Date
char *getDateStamp(void) {
    time_t t = time(NULL);

    static char result[20];
    strftime(result, sizeof(result), "%Y-%m-%d", localtime(&t));

    return result;
}
// --- END    || TIME STAMP OPERATIONS ---
// --- END      ||  UTILITIES ---