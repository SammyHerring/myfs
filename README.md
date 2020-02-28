===README.txt===
##System Title: myfs, ./myfs, myFileSystem.
#Release: v1.0.0
#Author: Samuel S.D. Herring
#Author URI: https://traveller.coffee

---
For ./myfs manual use shell command --> 'man ./myfs-help'.
---

Project Structure:
Scripts:
~/(myfsDirectory)/myfs.c
              System entrypoint to be called to use myfs Script
~/(myfsDirectory)/logger.h
              Seperate script called my 'myfs' to handle all logging of events.

Documents:
~/(myfsDirectory)/Documents/letter1.txt
              Example '.txt' File - letter1.txt

~/(myfsDirectory)/Documents/letter2.txt
              Example '.txt' File - letter2.txt

~/(myfsDirectory)/Documents/letter3.txt
              Example '.txt' File - letter3.txt

~/(myfsDirectory)/(file).txt
              Actual '.txt' File that user will provide to store in file system.

~/(myfsDirectory)/myfs-help
              myfs Help Guide File

CI/CD & CMake Compile Scripts:
~/(myfsDirectory)/CMakeFiles/*
              Numerous CMAKE compile files for CI/CD
---
Library Use:
#include <stdio.h>
              //Standard Input/Output for Console
#include <stdlib.h>
              //Standard C libraru for generic function
#include <unistd.h>
              //Access POSIX
#include <dirent.h>
              //Used for directory file search methods
#include <errno.h>
              //C integer-based error output handling
#include <sys/stat.h>
              //Used to collect file stats such as file size
#include <string.h>
              //Macros for manipulating strings (lists of chars)
#include <time.h>
              //Collect and manipulate system timedate data
#include <stdbool.h>
              //Macro to allow the use of boolean logic
#include <logger.h>
              //Custom logger macro to handle logging of events
---
These libraries are accessible across the vast majority of UNIX-based systems and so is usable independantly on any
device. In the provided, compiled CMAKE format, missing library dependencies are removed.