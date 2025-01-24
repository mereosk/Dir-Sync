#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/times.h>
#include <unistd.h>
#include <errno.h>

#define MASK 1023
#define BUFFSIZE 1024
#define BASEMODE 0755


// Checks if the command line arguments are in the correct form
// also it returns the directories than the main program wants
void checkCommandArguments(int argc, const char **argv, bool *verbose, bool *deleted, bool *links, char *origindir, char *destdir);

// This function checks the stats of two inodes and returns whether they are similar or not
// also it returns as pointers what are the the elements, directories or files
bool checkSimilarity(struct stat sourceStat,struct stat  destStat, bool *sourceIsFile, bool *destIsFile);

char *createPathStr(char *str1, char *str2);

void copyFile(char *source, char *dest);

void printLogs(int countAll, int countCopied, int countAllBytes, double runTime);