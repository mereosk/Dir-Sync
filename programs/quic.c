#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <ftw.h>

#include "ancillary_funcs.h"


bool verboseGlobal;


int decimalToOctal(int decimalnum)
{
    int octalnum = 0, temp = 1;

    while (decimalnum != 0)
    {
    	octalnum = octalnum + (decimalnum % 8) * temp;
    	decimalnum = decimalnum / 8;
        temp = temp * 10;
    }

    return octalnum;
}

int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    int rv = remove(fpath);
    if(verboseGlobal)
        printf("Deleted %s\n", fpath);
    if (rv)
        perror(fpath);

    return rv;
}

int rmrf(char *path) {
    return nftw(path, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
}


void deleteDirOrFiles(char *origindir, char *destdir, bool verbose) {
    struct dirent *destinationDirent;   
    DIR *sourceDir, *destinationDir;
    struct stat destStat, sourceStat;

    // Open the directories
    if ((sourceDir = opendir(origindir)) == NULL )
        fprintf(stderr," cannot open %s \n", origindir);    
    
    if ((destinationDir = opendir(destdir)) == NULL )
        fprintf(stderr," cannot open %s \n", destdir);

    while ((destinationDirent = readdir(destinationDir)) != NULL) {
        if (destinationDirent->d_ino == 0) 
            printf("There is something deleted here\n");
        
        if(!strcmp(destinationDirent->d_name, ".") || !strcmp(destinationDirent->d_name, ".."))
            continue;

        //Fix the path string of destination
        char *destName = createPathStr(destdir, destinationDirent->d_name);

        //Fix the path string of source
        char *sourceName = createPathStr(origindir, destinationDirent->d_name);

        // Check the path of the source directory to see if it containes this file/directory
        if (stat(sourceName, &sourceStat) < 0) {
            if (errno != ENOENT) {
                ;// Some other error has occured, handle appropriately
            }
            else {
                stat(destName, &destStat);
                if (( destStat.st_mode & S_IFMT ) != S_IFDIR ) {
                    remove(destName);
                    printf("Deleted file %s\n", destName);
                }
                else {
                    rmrf(destName);
                }
            }
        }
        else {
            if (( sourceStat.st_mode & S_IFMT ) != S_IFDIR ) {
                free(destName);
                free(sourceName);
                continue;
            }
            else {
                deleteDirOrFiles(sourceName, destName, verbose);
            }
        } 
        free(destName);
        free(sourceName);
    }
    closedir(sourceDir);
    closedir(destinationDir);
}


    


void copyDir(char *origindir, char *destdir, bool delete, bool verbose, int *countAll, int *countCopied, int *countBytes) {
    DIR *sourceDir, *destinationDir;
    struct dirent *originDirent;
    struct stat destStat, sourceStat;
    mode_t dirPermitions;
    int from, to, n;
    char buf[BUFFSIZE];
    bool sourceIsFile, destIsFile, same;

    // Open the directories
    if ((sourceDir = opendir(origindir)) == NULL )
        fprintf(stderr," cannot open %s \n", origindir);    
    
    if ((destinationDir = opendir(destdir)) == NULL ) {
        if(errno != ENOENT) {
            fprintf(stderr," cannot open %s \n", destdir);
        }
        else {
            mkdir(destdir, BASEMODE);       // If folder doesn't exist create one 
            printf("Created directory %s\n", destdir);
        }
    }

    while ((originDirent = readdir(sourceDir)) != NULL) {
        if (originDirent->d_ino == 0) 
            printf("There is something deleted here\n");
        
        if(!strcmp(originDirent->d_name, ".") || !strcmp(originDirent->d_name, ".."))
            continue;

        //Fix the path string of destination
        char *destName = createPathStr(destdir, originDirent->d_name);

        //Fix the path string of source
        char *sourceName = createPathStr(origindir, originDirent->d_name);

        if( stat(sourceName, &sourceStat ) == -1)
            perror("Failed to get file status");

        // Check the path of the second directory to see if it containes this file/directory
        if (stat(destName, &destStat) < 0) {
            if (errno != ENOENT) {
                perror("Failed to get file status"); // Some other error has occured, handle appropriately
            }
            else {
                // File pointed to by path doesn't exist

                //It has to be created. First check what type it is
                if (( sourceStat.st_mode & S_IFMT ) != S_IFDIR ) {

                    // Copy the file from the source to the destination
                    // First open the source file
                    copyFile(sourceName, destName);
                    
                    *countAll+=1;
                    *countCopied+=1;
                    if(verbose)
                        printf("Created file %s\n", destName);

                    if (stat(destName, &destStat) < 0)
                        perror("Failed getting the status");
                    
                    *countBytes+=(int)destStat.st_size;
                }       
                else {      // It is a direcotory
                    
                    dirPermitions = sourceStat.st_mode;
                    dirPermitions = dirPermitions & MASK;   // Mask  
                    mkdir(destName, dirPermitions);

                    *countAll+=1;
                    *countCopied+=1;
                    if(verbose)
                        printf("Created directory %s\n", destName);

                    if (stat(destName, &destStat) < 0)
                        perror("Failed getting the status");
                    
                    *countBytes+=(int)destStat.st_size;

                    // Do recursion and go deeper into the file
                    copyDir(sourceName, destName, delete, verbose, countAll, countCopied, countBytes);
                }
            }
        }
        else {     // File/directory with this name exists in destination directory

            // Now I have to check if they are the same
            same = checkSimilarity(sourceStat, destStat, &sourceIsFile, &destIsFile);
            if(!same) {      // If they are not the same then
                if(sourceIsFile != destIsFile) {            // They are differest elements
                    // If the source is a directory, we need to delete the file from 
                    // the destination make the dir with all of its content
                    if(!sourceIsFile) {
                        if (remove(destName) == 0){
                            // *countAll-=1;
                            if(verbose)
                                printf("Deleted file %s\n", destName);
                        }
                        else {
                            printf("Unable to delete the file\n");  
                        }

                        // Then copy the directory        
                        dirPermitions = sourceStat.st_mode;
                        dirPermitions = dirPermitions & MASK;   // Mask  
                        mkdir(destName, dirPermitions);

                        *countAll+=1;
                        *countCopied+=1;
                        if(verbose)
                            printf("Created directory %s %d\n", destName, *countAll);
                        
                    
                        *countBytes+=(int)destStat.st_size;

                        // Do recursion and go deeper into the file
                        copyDir(sourceName, destName, delete, verbose, countAll, countCopied, countBytes);
                    }
                    else {   // If the source is a file, we need to delete the directory from the destination and copy the file
                        rmrf(destName);

                        copyFile(sourceName, destName);

                        *countAll+=1;
                        *countCopied+=1;
                        if(verbose)
                            printf("Created file %s\n", destName);
                        
                    
                        *countBytes+=(int)destStat.st_size;
                    }
                }
                else {          // They are files but file from source dir has to be copied to dest dir
                    // Copy the file from the source to the destination
                    // First open the source file
                    if (( from = open(sourceName, O_RDONLY)) < 0 ){
                        perror("error opening the source file\n"); exit(1);
                    }

                    // Then create the source file
                    if (( to = open(destName, O_WRONLY | O_TRUNC | O_APPEND)) < 0 ){
                        perror("error opening the destination file\n"); exit(1);
                    }
                    while ((n=read(from, buf, sizeof(buf))) > 0 )
                        write(to, buf ,n );

                    close(from);
                    close(to);

                    *countAll+=1;
                    *countCopied+=1;
                    if(verbose)
                        printf("Created file %s\n", destName); 
                    
                    *countBytes+=(int)destStat.st_size;
                } 
            }
            else {           // If they are the same then continue
                if(!sourceIsFile) {       // If they are both directories do the recursion in the directories
                    *countAll+=1;
                    copyDir(sourceName, destName, delete, verbose, countAll, countCopied, countBytes);
                }
                else {                     // They are the same so continue
                    free(destName);
                    free(sourceName);
                    *countAll+=1;
                    continue;
                }
            }
        }

        free(destName);
        free(sourceName);
    }

    closedir(sourceDir);
    closedir(destinationDir);
}


int main(int argc, char const *argv[]) {
    char origindir[256];
    char destdir[256]; 
    bool verbose = false;
    bool deleted = false;
    bool links = false;
    double t1, t2;
    struct tms tb1 , tb2;
    double ticspersec;
    int countAll = 0, countCopied = 0, countBytes = 0;

    ticspersec = (double)sysconf(_SC_CLK_TCK);

    // Checks that the the conditions are satisfied
    checkCommandArguments(argc, argv, &verbose, &deleted, &links, origindir, destdir);
    verboseGlobal = verbose;

    t1 = (double)times(&tb1);

    copyDir(origindir, destdir, deleted, verbose, &countAll, &countCopied, &countBytes);

    if(deleted)
        deleteDirOrFiles(origindir, destdir, verbose);

    t2 = (double)times(&tb2);

    printLogs(countAll, countCopied, countBytes, (t2-t1)/ticspersec);

    return 0;
}
