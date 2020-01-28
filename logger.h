/*
* Name: 	MCOMD3PST ASSIGNMENT 2 | logger.h
* Purpose:	File System Simulation
* Author:	Samuel Steven David Herring
* UserID:	sh1042
* Creation Date: 23/01/2020
*/

#ifndef MYFS_LOGGER_H
#define MYFS_LOGGER_H

#include <stdio.h>

//Date Time Format Function
char *getTimeStamp(void);

char *getDateStamp(void);

//Remove path from Filename
#define __SHORT_FILE__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

//Logger Macro
//Prints to console and appends to log file
#define __LOG__(format, loglevel, ...) ({\
printf("%s %-5s\t[%s]\t\t[%s:%d]\t\t" format "\n", getTimeStamp(), loglevel, __func__, __SHORT_FILE__, __LINE__, ## __VA_ARGS__); \
time_t t = time(NULL);\
static char fullPath[30] = "Logs/";\
static char* time;\
time = getDateStamp();\
strcat(time, "_Log.txt");\
strcat(fullPath, time);\
FILE *out = fopen(fullPath, "a"); \
fprintf(out, "%s %-5s\t[%s]\t\t[%s:%d]\t\t" format "\n", getTimeStamp(), loglevel, __func__, __SHORT_FILE__, __LINE__, ## __VA_ARGS__); \
fclose(out);\
})

//Prints to console but no record (NR)
#define __LOGNR__(format, loglevel, ...) ({\
printf("%s %-5s\t[%s]\t\t[%s:%d]\t\t" format "\n", getTimeStamp(), loglevel, __func__, __SHORT_FILE__, __LINE__, ## __VA_ARGS__); \
})


//Specified Log Level Macro Types
#define LOGDEBUG(format, ...) __LOG__(format, "DEBUG", ## __VA_ARGS__)
#define LOGDEBUGNR(format, ...) __LOGNR__(format, "DEBUG", ## __VA_ARGS__)

#define LOGWARN(format, ...) __LOG__(format, "WARN", ## __VA_ARGS__)
#define LOGWARNNR(format, ...) __LOGNR__(format, "WARN", ## __VA_ARGS__)

#define LOGERROR(format, ...) __LOG__(format, "ERROR", ## __VA_ARGS__)
#define LOGERRORNR(format, ...) __LOGNR__(format, "ERROR", ## __VA_ARGS__)

#define LOGINFO(format, ...) __LOG__(format, "INFO", ## __VA_ARGS__)
#define LOGINFONR(format, ...) __LOGNR__(format, "INFO", ## __VA_ARGS__)

#endif //MYFS_LOGGER_H