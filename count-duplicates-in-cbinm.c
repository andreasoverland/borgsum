#include <stdio.h>
#include <stdlib.h>
#include <libc.h>

/*
* NBINC Format. Since the nbinc file always will have the same ply for all boards, a unique key consists in the file,
* without keeping the ply in the data. The first 4*8+2 (34) bytes can be used for comparison
*  0 krp map      (8 bytes)
*  8 qrb map      (8 bytes)
* 16 bnp map      (8 bytes)
* 24 w map        (8 bytes)
* 32 castling     (1 byte)
* 33 ep index     (1 byte) <- ********** NEW * only set this, if an en passant strike is possible
* ---- end of key
* 34 lastMoveWas  (1 byte)
* 35 check status (1 byte)
* 36 dup. on ply  (1 bytes) <- if this position already exists, this points to the ply ************** NEW
* 37 multiplier   (4 bytes) <- int at index 10
* Total 41 bytes.
  */

const int LINE_BYTE_SIZE = 41;
const int MAX_READ_LINES = 24390;

// Used to sort board positions in a buffer
int cmpFunc(const void *a, const void *b) {
    return memcmp(a, b, 34 * sizeof(unsigned char));
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: collator filename.cbinm\n");
        return 0;
    }

    FILE *inFile = fopen(argv[1], "r"); // open binary file reading
    if (inFile == NULL) {
        printf("File not found: %s\n", argv[1]);
        return 0;
    }

    printf("Merging duplicates in file %s\n",argv[1]);

    // strategy :
    // 1. Read 100k chuncks of the file into buffer (2440
    // 2. Sort each buffer and save to separate files
    // 3. Mergesort files
    // 4. Merge duplicates
    // 5. Write results back to filename.merged

    fseek(inFile, 0L, SEEK_END);
    long currentFileSize = ftell(inFile);
    printf("Filesize: %ld\n", currentFileSize);
    printf("Num lines in %s: %ld\n", argv[1], currentFileSize / LINE_BYTE_SIZE);
    fseek(inFile, 0, SEEK_SET);


    // Read 2439 positions into buffer
    unsigned char *readBuffer = malloc(MAX_READ_LINES * LINE_BYTE_SIZE * sizeof(unsigned char));
    for( int f=0;f<=(int)(currentFileSize/(MAX_READ_LINES*LINE_BYTE_SIZE));f++){
        memset(readBuffer, 0x00, MAX_READ_LINES * LINE_BYTE_SIZE * sizeof(signed char));
        printf("Reading buffer %d of %d\n",f+1, 1+(int)(currentFileSize/(MAX_READ_LINES*LINE_BYTE_SIZE)));
        unsigned long numRead = fread(readBuffer, sizeof(unsigned char), MAX_READ_LINES * LINE_BYTE_SIZE, inFile);
        printf("Read %lu bytes from file\n", numRead );

        qsort(readBuffer, numRead / LINE_BYTE_SIZE, LINE_BYTE_SIZE * sizeof(unsigned char), cmpFunc);

        // todo: merge duplicates in the buffer
        // The positions are now sorted, 41 bytes for each position
        // Start on the top, then look at the next position and add the multipliers as long as the
        // positions are the same (34 first bytes)

        for( int p1 = 0;p1<numRead/LINE_BYTE_SIZE;p1++){
            for( int p2 = p1+1; p2<numRead/LINE_BYTE_SIZE;p2++){
                // TODO: implement the merge and count of multipliers
                // ALSO: create a new buffer to store unique lines
            }
        }
        // write back to a temporary file
        char outFileFileName[100];
        sprintf(outFileFileName,"%s_%d.tmp",argv[1],f);
        FILE *outFile = fopen( outFileFileName,"w");
        fwrite( readBuffer, sizeof(unsigned char), numRead, outFile );
        fclose( outFile );
    }
    free(readBuffer);

    fclose( inFile );

    // next tool : find duplicates
    // 1. For each position in the file
    //    - Check if it exists in the previous files. Use binary sort method to find it fast
    //    - If found, mark duplicate ply in the same spot in the file
    //    - If not found, do nothing

    return 0;
}
