#include "ancillary_funcs.h"



void checkCorrectFlag(const char *myStr, char *verboseChar, char *deletedChar, char *linksChar, bool *verbose, bool*deleted, bool *links) {
    if(!strcmp(myStr, verboseChar))
        *verbose = true;
    else if(!strcmp(myStr, deletedChar))
        *deleted = true;
    else if(!strcmp(myStr, linksChar))
        *links = true;
    else {
        printf("The only flags that we can use are '%s', '%s', '%s'\n", verboseChar, deletedChar, linksChar);
        exit(EXIT_FAILURE);
    }
}


void checkCommandArguments(int argc, const char **argv, bool *verbose, bool *deleted, bool *links, char *origindir, char *destdir) {
    char *verboseChar = "-v";
    char *deletedChar = "-d";
    char *linksChar = "-l";
    struct stat info1;

    // Check if the usage is correct
    if(argc < 3 || argc > 6) {
        perror("Usage = ./quic -v -d -l origindir destdir (with or without the flags)\n");
        exit(EXIT_FAILURE);
    }

    // Check the flags
    int numOfFlags = argc - 3;

    if(numOfFlags == 1) {
        checkCorrectFlag(argv[1], verboseChar, deletedChar, linksChar, verbose, deleted, links);
    }
    else if(numOfFlags == 2) {
        if(!strcmp(argv[1], argv[2])) {     // If the flags are the same then exit
            printf("You entered the same flag\n");
            exit(EXIT_FAILURE);
        }
        else {
            checkCorrectFlag(argv[1], verboseChar, deletedChar, linksChar, verbose, deleted, links);
            checkCorrectFlag(argv[2], verboseChar, deletedChar, linksChar, verbose, deleted, links);         
        }
    }
    else if(numOfFlags == 3) {
        if(!strcmp(argv[1], argv[2]) || !strcmp(argv[2], argv[3]) || !strcmp(argv[1], argv[3])) {     // If the flags are the same then exit
            printf("You entered the same flag\n");
            exit(EXIT_FAILURE);
        }
        else {
            checkCorrectFlag(argv[1], verboseChar, deletedChar, linksChar, verbose, deleted, links);
            checkCorrectFlag(argv[2], verboseChar, deletedChar, linksChar, verbose, deleted, links);         
            checkCorrectFlag(argv[3], verboseChar, deletedChar, linksChar, verbose, deleted, links);         
        }
    }

    // Check the inodes
    if ( stat(argv[argc-2], &info1 ) == -1) {
        perror (" Failed to get file status "); 
        exit(EXIT_FAILURE);
    }

    // Check if the source path name is directory
    if (( info1.st_mode & S_IFMT ) != S_IFDIR ) {
        printf ("%s is not a directory\n", argv[argc-2]);
        exit(EXIT_FAILURE);
    }


    // Command line arguments were correct
    strcpy(origindir, argv[argc-2]);
    strcpy(destdir, argv[argc-1]);
}


bool checkSimilarity(struct stat sourceStat,struct stat  destStat, bool *sourceIsFile, bool *destIsFile) {
    // Check what if the elements are file or directory
    if (( sourceStat.st_mode & S_IFMT ) != S_IFDIR )    // Source is a file
        *sourceIsFile = true;
    else
        *sourceIsFile = false;

    if (( destStat.st_mode & S_IFMT ) != S_IFDIR )    // Source is a file
        *destIsFile = true;
    else
        *destIsFile = false;

    // 1. If the elements are different type then they are not the same
    if(*sourceIsFile != *destIsFile)
        return false;
    else if(*sourceIsFile == false)     // 2.If they are both directories then they are the same
        return true;
    else {                              // Reaching this point means that both elements are files
        if((int)sourceStat.st_size != (int)destStat.st_size) {     // 3.If they have different size then they are not similar
            // printf(" SIZE 1 IS %d SIZE 2 IS %d\n", (int)sourceStat.st_size, (int)destStat.st_size);
            return false;
        }
        else if((int)sourceStat.st_mtime > (int)destStat.st_mtime) {   // 4. They are the same size but source file has been modified ?
            // printf(" time 1 is %.12s and time 2 is %.12s\n", ctime(&sourceStat.st_mtime)+4, ctime(&destStat.st_mtime)+4);
            return false;
        }
    }
    return true;    // Reaching this point means that the files are the same
}


char *createPathStr(char *str1, char *str2) {
    char *destName = malloc(1024*sizeof(char));
    strcpy(destName, str1);
    strcat(destName, "/");
    strcat(destName, str2);

    return destName;
}


void copyFile(char *sourceName, char *destName) {
    int from, to, n;
    char buf[BUFFSIZE];
    mode_t filePermitions;
    struct stat sourceStat;

    if (( from = open(sourceName, O_RDONLY)) < 0 ){
        perror("error opening the source file\n"); exit(1);
    }

    if (stat(sourceName, &sourceStat) < 0) {
        perror("Failed getting the status\n");
        exit(EXIT_FAILURE);
    }

    // Then create the source file
    if (( to = open(destName, O_WRONLY | O_CREAT | O_APPEND , BASEMODE)) < 0 ){
        perror("error opening the destination file\n"); exit(1);
    }
    while ((n=read(from, buf, sizeof(buf))) > 0 )
        write(to, buf ,n );

    close(from);
    // Copy the permitions of the source file to the destination file
    filePermitions =  sourceStat.st_mode;
    filePermitions = filePermitions & MASK; //Mask
    fchmod(to, filePermitions);
    close(to); 
}


void printLogs(int countAll, int countCopied, int countBytes, double runTime) {
    double bytesPerSec;
    bytesPerSec = (runTime == 0.0) ? 0.0 : (double)countBytes/runTime;
    printf("\nThere are %d files/directories in the hierarchy\n", countAll);
    printf("Number of entities copied is %d\n", countCopied);
    printf("Copied %d bytes in %.4lfsec at %.2lf bytes/sec\n", countBytes, runTime, bytesPerSec);
}