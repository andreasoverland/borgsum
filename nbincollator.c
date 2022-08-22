// - takes a file input argument like 'level5.nbin'
// - sorts content as it were 33 byte numbers
// - counts distinct elements and updates the multiplier field in the nbin board


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"

int main(int argc, char **argv) {

    FILE *inFile = fopen("counted", "r+b");
    FILE *outFile = fopen("collated", "wb");

    unsigned int *bigBuff = malloc(5 * 1000 * 1000 * 100);
    bigBuff[0] = 1;

    const int INDEX_SIZE_MAX = 1000;
    const int CACHE_MAX = 100000;
    unsigned long total = 0;
    unsigned char nibbleBinary[COMP_BINARY_BYTE_SIZE * CACHE_MAX];
    unsigned char clearNibbleBinary[COMP_BINARY_BYTE_SIZE];
    unsigned char subNibbleBinary[COMP_BINARY_BYTE_SIZE * CACHE_MAX];
    memset(clearNibbleBinary, 0, sizeof(unsigned char) * COMP_BINARY_BYTE_SIZE);
    unsigned long pos = 0;
    unsigned long numUniqueBoards = 0;
    unsigned long zeroBoardsInCache = 0;
    memset(nibbleBinary, 0, sizeof(unsigned char) * COMP_BINARY_BYTE_SIZE * INDEX_SIZE_MAX);
    memset(subNibbleBinary, 0, sizeof(unsigned char) * COMP_BINARY_BYTE_SIZE * CACHE_MAX);
    unsigned long totalRead = 0;
    unsigned long read = fread(nibbleBinary, sizeof(unsigned char), COMP_BINARY_BYTE_SIZE * INDEX_SIZE_MAX, inFile);

    unsigned long mainIterations = 0;
    unsigned long subIterations = 0;

    while (read > 0) {


        totalRead += read;

        // Phase 1 - Read 10,000 boards into cache

        int cacheSize = 0;

        pos += read;
        cacheSize = read / COMP_BINARY_BYTE_SIZE;


        // Phase 1.1 - Make sure no duplicates exist in the cache
        for (int i = 0; i < cacheSize; i++) {
            unsigned long *k = (unsigned long *) &nibbleBinary[i * COMP_BINARY_BYTE_SIZE];
            if (k[0] == 0) {
                // May well be zero after iteration 0
                continue;
            }
            unsigned char *k1 = (unsigned char *) &nibbleBinary[i * COMP_BINARY_BYTE_SIZE];
            for (int ii = 0; ii < i; ii++) {
                unsigned char *kk = (unsigned char *) &nibbleBinary[ii * COMP_BINARY_BYTE_SIZE];
                mainIterations++;
                if (kk[0] == 0 && kk[1] == 0 && kk[2] == 0 && kk[3] == 0) {
                    // May well be zero after iteration 0
                    continue;
                }
                int mismatch = 0;
                for( int ki = 0;ki<37;ki++){
                    if( k1[ki] != kk[ki] ){
                        mismatch = 1;
                        break;
                    }
                }
                if( mismatch == 1 ){
                    continue;
                }

                    unsigned int *multi = (unsigned int *) &nibbleBinary[37 + i * COMP_BINARY_BYTE_SIZE];
                    unsigned int multiplier = *multi;

                    unsigned int *multi2 = (unsigned int *) &nibbleBinary[37 + ii * COMP_BINARY_BYTE_SIZE];
                    unsigned int multiplier2 = *multi2;

                    multiplier += multiplier2;

                    memset(nibbleBinary + COMP_BINARY_BYTE_SIZE * ii, 0, COMP_BINARY_BYTE_SIZE);
                    break;
            }
        }

        // Now the cache contains unique boards, with correct multipliers


        // Phase 3 - Scan each line of the file and compare with boards in cache


        unsigned long subRead = fread(subNibbleBinary, sizeof(unsigned char), COMP_BINARY_BYTE_SIZE * CACHE_MAX,
                                      inFile);

        unsigned long subPos = pos;
        while (subRead > 0) {

            totalRead += subRead;
            for (int si = 0; si < subRead / COMP_BINARY_BYTE_SIZE; si++) {

                unsigned long *subk = (unsigned long *) &subNibbleBinary[si * COMP_BINARY_BYTE_SIZE];
                if (subk[0] == 0) {
                    continue;
                }
                unsigned char *subcomp = (unsigned char *) &subNibbleBinary[si * COMP_BINARY_BYTE_SIZE];
                unsigned int *submulti = (unsigned int *) &subNibbleBinary[37 + si * COMP_BINARY_BYTE_SIZE];
                unsigned int submultiplier = *submulti;

                // gå igjennom alle nibbleBinary-gærningene og sjekk om de er like
                for (int i = 0; i < cacheSize; i++) {

                    unsigned long *k = (unsigned long *) &nibbleBinary[i * COMP_BINARY_BYTE_SIZE];
                    if (k[0] == 0) {
                        //printf("strange cached board %lu %lu %lu %lu", k[0], k[1], k[2], multiplier );
                        continue;
                    }
                    unsigned int *multi = (unsigned int *) &nibbleBinary[37 + i * COMP_BINARY_BYTE_SIZE];
                    unsigned int multiplier = *multi;
                    unsigned char *comp = (unsigned char *) &nibbleBinary[i * COMP_BINARY_BYTE_SIZE];
                    subIterations++;
                    int mismatch = 0;
                    for (int ci = 0; ci < 37; ci++) {
                        if (comp[ci] != subcomp[ci]) {
                            mismatch = 1;
                            break;
                        }
                    }

                    if( mismatch == 1){
                        break;
                    }

                    multiplier += submultiplier;
                    multi[0] = multiplier;

                    // printf("%lu %lu %lu %lu\n", subk[0], subk[1], subk[2], submultiplier);
                    subk[0] = 0;
                    subk[1] = 0;
                    subk[2] = 0;
                    submulti[0] = 0;
                }
            }
            fseek(inFile, subPos, SEEK_SET);
            fwrite(subNibbleBinary, sizeof(unsigned char), subRead, inFile);
            subPos += subRead;
            subRead = fread(subNibbleBinary, sizeof(unsigned char), COMP_BINARY_BYTE_SIZE * CACHE_MAX, inFile);
        }


        // Phase 4 - write cache to file
        for (int i = 0; i < cacheSize; i++) {
            unsigned long *k = (unsigned long *) &nibbleBinary[i * COMP_BINARY_BYTE_SIZE];
            if (k[0] != 0 && k[1] != 0 && k[2] != 0) {
                fwrite(nibbleBinary + i * COMP_BINARY_BYTE_SIZE, sizeof(unsigned char), COMP_BINARY_BYTE_SIZE, outFile);
                unsigned int *multi = (unsigned int *) &nibbleBinary[37 + i * COMP_BINARY_BYTE_SIZE];
                unsigned int multiplier = *multi;
                total += multiplier;
                numUniqueBoards++;
            }
        }

        fseek(inFile, (int) pos, SEEK_SET);
        memset(nibbleBinary, 0, sizeof(unsigned char) * COMP_BINARY_BYTE_SIZE * INDEX_SIZE_MAX);
        read = fread(nibbleBinary, sizeof(unsigned char), COMP_BINARY_BYTE_SIZE * INDEX_SIZE_MAX, inFile);

    }

    printf("zeroboards in cache: %lu\n", zeroBoardsInCache);
    printf("num boards read:%lu\n", pos / 41);
    printf("num unique boards:%lu\n", numUniqueBoards);
    printf("totalread :%lu\n", totalRead);
    printf("mainIter :%lu\n", mainIterations);
    printf("subIter :%lu\n", subIterations);

    fclose(inFile);
    fclose(outFile);

    return 0;
}
