//
// Created by Andreas Øverland on 12/12/2022.
//
#include <stdio.h>
#include <stdlib.h>
#include <libc.h>
#include <time.h>
#include <dirent.h>
#include "constants.h"

// Given a file levelN do the following
// Remove duplicates in the file itself
// Go through all level(N-2*n (2*n >=0 )) and remove all duplicates from the levelN file
// The resulting file should have only unique positions and no positions that exist in files from earlier levels where the same color is in the move

// 1. Mergesort of the original file
// 2. Read chunks from earlier files and remove duplicates from current file

const int MAX_READ_POSITIONS = 20000000;

// Used to sort board positions in a buffer
int cmpFunc(const void *a, const void *b) {
    return memcmp(a, b, REDUCED_COMP_BINARY_BYTE_SIZE * sizeof(unsigned char));
}

void makeTmpFolderName(char *name) {
    const char *str = "ABCDEFGHIJHLKMOPQRSTUVWXYZabcdefghijhlkmopqrstuvwxyz0123456789";
    for (int i = 0; i < 10; i++) {
        name[i] = str[rand() % 62];
    }
}

int getNumFilesInDir(char *dirname) {
    DIR *dirp = opendir(dirname);
    struct dirent *entry;
    int fileCount = 0;
    while ((entry = readdir(dirp)) != NULL) {
        if (entry->d_type == DT_REG) { /* If the entry is a regular file */
            fileCount++;
        }
    }
    closedir(dirp);
    return fileCount;
}

int main(int argc, char **argv) {
    if( argc < 3 ){
        printf("Usage: remdups filename level\nremdups output.rcbin 2\n");
        return 0;
    }

    srand((unsigned) time(NULL));

    FILE *inFile = fopen(argv[1], "r"); // open binary file reading
    if (inFile == NULL) {
        printf("File not found: %s\n", argv[1]);
        return 0;
    }

    int currentLevel = atoi( argv[2] );
    printf("Current level: %d\n", currentLevel );

    fseek(inFile, 0L, SEEK_END);
    long currentFileSize = ftell(inFile);
    printf("Num positions in file: %ld\n", currentFileSize / REDUCED_COMP_BINARY_BYTE_SIZE);
    fseek(inFile, 0, SEEK_SET);

    unsigned char *readBuffer = malloc(MAX_READ_POSITIONS * REDUCED_COMP_BINARY_BYTE_SIZE * sizeof(unsigned char));
    unsigned char *writeBuffer = malloc(MAX_READ_POSITIONS * REDUCED_COMP_BINARY_BYTE_SIZE * sizeof(unsigned char));

    // make a temp folder and put all files in there, for later merging
    char *tempFolderNameA = malloc(100);
    makeTmpFolderName(tempFolderNameA);
    mkdir(tempFolderNameA, S_IRWXU);

    int numFiles = (int) (currentFileSize / (MAX_READ_POSITIONS * REDUCED_COMP_BINARY_BYTE_SIZE));
    for (int f = 0; f <= numFiles; f++) {
        memset(readBuffer, 0x00, MAX_READ_POSITIONS * REDUCED_COMP_BINARY_BYTE_SIZE * sizeof(signed char));
        unsigned long numRead = fread(readBuffer, sizeof(unsigned char),
                                      MAX_READ_POSITIONS * REDUCED_COMP_BINARY_BYTE_SIZE, inFile);

        qsort(readBuffer, numRead / REDUCED_COMP_BINARY_BYTE_SIZE,
              REDUCED_COMP_BINARY_BYTE_SIZE * sizeof(unsigned char), cmpFunc);

        // The positions are now sorted, 34 bytes for each position
        int uniquePos = 0;
        for (int originalPos = 0; originalPos < (numRead / REDUCED_COMP_BINARY_BYTE_SIZE); originalPos++) {
            memcpy(&writeBuffer[uniquePos * REDUCED_COMP_BINARY_BYTE_SIZE],
                   &readBuffer[originalPos * REDUCED_COMP_BINARY_BYTE_SIZE], REDUCED_COMP_BINARY_BYTE_SIZE);
            if (originalPos == (numRead / REDUCED_COMP_BINARY_BYTE_SIZE) - 1) {
                break;
            }
            for (int comparePos = originalPos + 1;
                 comparePos < (numRead / REDUCED_COMP_BINARY_BYTE_SIZE); comparePos++) {
                if (memcmp(&readBuffer[originalPos * REDUCED_COMP_BINARY_BYTE_SIZE],
                           &readBuffer[comparePos * REDUCED_COMP_BINARY_BYTE_SIZE],
                           REDUCED_COMP_BINARY_BYTE_SIZE) == 0) {
                    continue;
                } else {
                    // the two buffers differ
                    // set originalPos to comparePos, then break this loop
                    originalPos = comparePos - 1;
                    uniquePos++;
                    break;
                }
            }
        }


        // write back to a temporary file
        /*char sortedOutFileFileName[100];
        sprintf(sortedOutFileFileName,"%s_%d.sorted",argv[1],f);
        FILE *sortedOutFile = fopen( sortedOutFileFileName,"w");
        fwrite(readBuffer, sizeof(unsigned char), numRead, sortedOutFile );
        fclose( sortedOutFile );*/

        char mergedOutFileName[100];
        sprintf(mergedOutFileName, "%s/%05d.rcbin", tempFolderNameA, f);
        FILE *mergedOutFile = fopen(mergedOutFileName, "w");
        fwrite(writeBuffer, sizeof(unsigned char), (uniquePos + 1) * REDUCED_COMP_BINARY_BYTE_SIZE, mergedOutFile);
        fclose(mergedOutFile);
        printf("Merged %s of %d\n", mergedOutFileName, numFiles );
    }

    fclose(inFile);

    // merge sorted temp-files, with only unique positions
    // Åpne to og to-filer, og merge de til 1 med kun unike brett
    char *tempFolderNameB = malloc(100);
    makeTmpFolderName(tempFolderNameB);
    mkdir(tempFolderNameB, S_IRWXU);

    numFiles = getNumFilesInDir(tempFolderNameA);

    if( numFiles == 1 ){
        // small batch, only one file
        // move it back to root
        char *lastFileName = malloc(100);
        sprintf(lastFileName, "%s/00000.rcbin",tempFolderNameA);
        char *finalFileName = malloc(100);
        sprintf(finalFileName, "%s.unique",argv[1]);
        printf("Moving %s to %s\n", lastFileName, finalFileName );
        rename( lastFileName, finalFileName);
        free(lastFileName);
        free(finalFileName);
        remove( tempFolderNameA );
    }
    while (numFiles > 1) {

        for (int fileId = 0; fileId < numFiles; fileId += 2) {
            char sourceFileNameA[100];
            char sourceFileNameB[100];
            char targetFileName[100];
            sprintf(sourceFileNameA, "%s/%05d.rcbin", tempFolderNameA, fileId);
            sprintf(sourceFileNameB, "%s/%05d.rcbin", tempFolderNameA, fileId + 1);
            sprintf(targetFileName, "%s/%05d.rcbin", tempFolderNameB, fileId >> 1);
            FILE *sourceFileA = fopen(sourceFileNameA, "r"); // open binary file reading
            FILE *sourceFileB = fopen(sourceFileNameB, "r"); // open binary file reading
            FILE *targetFile = fopen(targetFileName, "w");
            unsigned char *bufferA = malloc(sizeof(unsigned char) * REDUCED_COMP_BINARY_BYTE_SIZE);
            unsigned char *bufferB = malloc(sizeof(unsigned char) * REDUCED_COMP_BINARY_BYTE_SIZE);
            if (sourceFileA != NULL && sourceFileB != NULL) {

                printf("Merging files:\n%s\n%s\n", sourceFileNameA, sourceFileNameB);
                printf("Into file:\n%s\n", targetFileName );
                unsigned long numReadFromA = fread(bufferA, sizeof(unsigned char), REDUCED_COMP_BINARY_BYTE_SIZE,
                                                   sourceFileA);
                unsigned long numReadFromB = fread(bufferB, sizeof(unsigned char), REDUCED_COMP_BINARY_BYTE_SIZE,
                                                   sourceFileB);
                while (numReadFromA > 0 && numReadFromB > 0) {
                    const int res = memcmp(bufferA, bufferB, REDUCED_COMP_BINARY_BYTE_SIZE);
                    if (res == 0) {
                        fwrite(bufferA, sizeof(unsigned char), REDUCED_COMP_BINARY_BYTE_SIZE, targetFile);
                        numReadFromA = fread(bufferA, sizeof(unsigned char), REDUCED_COMP_BINARY_BYTE_SIZE,
                                             sourceFileA);
                        numReadFromB = fread(bufferB, sizeof(unsigned char), REDUCED_COMP_BINARY_BYTE_SIZE,
                                             sourceFileB);
                    } else if (res < 0) {
                        fwrite(bufferA, sizeof(unsigned char), REDUCED_COMP_BINARY_BYTE_SIZE, targetFile);
                        numReadFromA = fread(bufferA, sizeof(unsigned char), REDUCED_COMP_BINARY_BYTE_SIZE,
                                             sourceFileA);
                    } else {
                        fwrite(bufferB, sizeof(unsigned char), REDUCED_COMP_BINARY_BYTE_SIZE, targetFile);
                        numReadFromB = fread(bufferB, sizeof(unsigned char), REDUCED_COMP_BINARY_BYTE_SIZE,
                                             sourceFileB);
                    }
                }
                while (numReadFromA > 0) {
                    // just copy the remaning from A
                    fwrite(bufferA, sizeof(unsigned char), REDUCED_COMP_BINARY_BYTE_SIZE, targetFile);
                    numReadFromA = fread(bufferA, sizeof(unsigned char), REDUCED_COMP_BINARY_BYTE_SIZE, sourceFileA);
                }
                while (numReadFromB > 0) {
                    // just copy the remaning from B
                    fwrite(bufferB, sizeof(unsigned char), REDUCED_COMP_BINARY_BYTE_SIZE, targetFile);
                    numReadFromB = fread(bufferB, sizeof(unsigned char), REDUCED_COMP_BINARY_BYTE_SIZE, sourceFileB);
                }
                // read line by line
                // if lineA === lineB , write line A, advance both files
                // if lineA < lineB, write lineA, advance file A
                // if lineA > lineB, write lineB, advance file B
            } else if (sourceFileA != NULL && sourceFileB == NULL) {
                printf("Leaving file untouched:\n%s\n", sourceFileNameA);
                unsigned char *copyBuff = malloc(
                        MAX_READ_POSITIONS * REDUCED_COMP_BINARY_BYTE_SIZE * sizeof(unsigned char));
                unsigned long numRead = fread(copyBuff, sizeof(unsigned char),
                                              MAX_READ_POSITIONS * REDUCED_COMP_BINARY_BYTE_SIZE, sourceFileA);
                while (numRead > 0) {
                    fwrite(copyBuff, sizeof(unsigned char), numRead, targetFile);
                    numRead = fread(copyBuff, sizeof(unsigned char),
                                    MAX_READ_POSITIONS * REDUCED_COMP_BINARY_BYTE_SIZE,
                                    sourceFileA);
                }
            }

            remove(sourceFileNameA);
            remove(sourceFileNameB);
            fclose(sourceFileA);
            fclose(sourceFileB);
            fclose(targetFile);
        }
        remove( tempFolderNameA );
        strcpy(tempFolderNameA, tempFolderNameB);
        numFiles = getNumFilesInDir(tempFolderNameA);
        if( numFiles > 1 ){
            makeTmpFolderName(tempFolderNameB);
            mkdir(tempFolderNameB, S_IRWXU);
        }
        else {
            char *lastFileName = malloc(100);
            sprintf(lastFileName, "%s/00000.rcbin",tempFolderNameA);
            char *finalFileName = malloc(100);
            sprintf(finalFileName, "%s.unique",argv[1]);
            printf("Moving %s to %s\n", lastFileName, finalFileName );
            rename( lastFileName, finalFileName);
            free(lastFileName);
            free(finalFileName);
            remove( tempFolderNameA );
        }
    }

    // now there is only one file in tempFolderNameB called 00000.rcbin
    // move it somewhere safe

    // compare with all previous levels for same color and remove non-new positions
    for( int level = currentLevel - 2; level >= 0; level-=2){
        printf("Removing positions from file level%d.rcbin.unique\n",level);

        char *currentLevelPrunedFileName = malloc(100);
        sprintf(currentLevelPrunedFileName,"level%d.rcbin.pruned",currentLevel);
        FILE *currLevelFilePruned = fopen(currentLevelPrunedFileName, "w");

        char *currentLevelFileName = malloc(100);
        sprintf(currentLevelFileName,"level%d.rcbin.unique",currentLevel);
        FILE *currLevelFile = fopen(currentLevelFileName, "r");

        char *prevLevelFileName = malloc(100);
        sprintf(prevLevelFileName,"level%d.rcbin.unique",level);
        FILE *prevLevelFile = fopen( prevLevelFileName, "r");

        // read line by line, remember they are sorted. so we can skip one or the other when they are not equal
        unsigned char *currLevelBuff = malloc(sizeof(unsigned char) * REDUCED_COMP_BINARY_BYTE_SIZE );
        unsigned char *prevLevelBuff = malloc(sizeof(unsigned char) * REDUCED_COMP_BINARY_BYTE_SIZE );

        unsigned long currNumRead = fread(currLevelBuff, sizeof(unsigned char), REDUCED_COMP_BINARY_BYTE_SIZE, currLevelFile);
        unsigned long prevNumRead = fread(prevLevelBuff, sizeof(unsigned char), REDUCED_COMP_BINARY_BYTE_SIZE, prevLevelFile );

        int numDuplicates = 0;
        while(currNumRead > 0 && prevNumRead > 0 ){
            int res = memcmp(currLevelBuff, prevLevelBuff, REDUCED_COMP_BINARY_BYTE_SIZE);
            if( res == 0){
                // This positions exists on a previous level, do not write
                // read again from both files
                currNumRead = fread(currLevelBuff, sizeof(unsigned char), REDUCED_COMP_BINARY_BYTE_SIZE, currLevelFile);
                prevNumRead = fread(prevLevelBuff, sizeof(unsigned char), REDUCED_COMP_BINARY_BYTE_SIZE, prevLevelFile );
                numDuplicates++;
            }
            else if( res < 0 ){
                // the current level has not caught up to the prev level in comparison.
                // write, and read the next line
                fwrite(currLevelBuff, sizeof(unsigned char), REDUCED_COMP_BINARY_BYTE_SIZE, currLevelFilePruned);
                currNumRead = fread(currLevelBuff, sizeof(unsigned char), REDUCED_COMP_BINARY_BYTE_SIZE, currLevelFile);
            }
            else {
                // the prev level file is behind the current level in comparison,
                // catch up
                prevNumRead = fread(prevLevelBuff, sizeof(unsigned char), REDUCED_COMP_BINARY_BYTE_SIZE, prevLevelFile );
            }
        }

        while(currNumRead > 0 ){
            fwrite(currLevelBuff, sizeof(unsigned char), REDUCED_COMP_BINARY_BYTE_SIZE, currLevelFilePruned);
            currNumRead = fread(currLevelBuff, sizeof(unsigned char), REDUCED_COMP_BINARY_BYTE_SIZE, currLevelFile);
        }

        printf( "%s had %d positions that existed in %s\n", currentLevelFileName, numDuplicates, prevLevelFileName);

        fclose(currLevelFile);
        fclose(prevLevelFile);
        fclose(currLevelFilePruned);

       remove(currentLevelFileName);
       rename(currentLevelPrunedFileName, currentLevelFileName);

        free( currentLevelPrunedFileName );
        free( currentLevelFileName );
        free( prevLevelFileName );


    }

    // use as in-file
    // adapt chessengined for rcbin as infile format

    free(writeBuffer);
    free(readBuffer);

}