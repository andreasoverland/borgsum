// - takes a file input argument like 'level5.nbin'
// - sorts content as it were 33 byte numbers
// - counts distinct elements and updates the multiplier field in the nbin board


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "constants.h"

int main(int argc, char **argv) {

	FILE *inFile = fopen("combined", "r+b");
	FILE *outFile = fopen("collated","wb");

	unsigned long total = 0;
	unsigned char nibbleBinary[NIBBLE_BINARY_BYTE_SIZE];
	unsigned char clearNibbleBinary[NIBBLE_BINARY_BYTE_SIZE];
	memset(clearNibbleBinary,0,sizeof(unsigned char) * NIBBLE_BINARY_BYTE_SIZE);
	int read = -1;
	int pos = 0;
	//while( read != 0){

	    read = fread(nibbleBinary, sizeof(unsigned char), NIBBLE_BINARY_BYTE_SIZE, inFile);  
		
		fseek( inFile, pos, SEEK_SET );
		fwrite( clearNibbleBinary, sizeof(unsigned char), NIBBLE_BINARY_BYTE_SIZE, inFile );
		pos += read;

		unsigned long *k = (unsigned long *) &nibbleBinary[0];

		if( k[0] == 0 && k[1] == 0 && k[2] == 0){
	//		continue;	
		}

		unsigned long *multi = (unsigned long *) &nibbleBinary[25];
		unsigned char level = nibbleBinary[24];
		unsigned long multiplier = *multi;
		
		printf( "%lu %lu %lu %lu\n", k[0],k[1],k[2],multiplier );

		unsigned char subNibbleBinary[NIBBLE_BINARY_BYTE_SIZE];
		int subPos = pos;
		int subRead = fread(subNibbleBinary, sizeof(unsigned char), NIBBLE_BINARY_BYTE_SIZE, inFile );
		while( subRead != 0){			
			unsigned long *subk = (unsigned long *) &subNibbleBinary[0];
			unsigned long *submulti = (unsigned long *) &subNibbleBinary[25];
			unsigned long submultiplier = *submulti;
			if( subk[0] == k[0] && subk[1] == k[1] && subk[2] == k[2] ){
				multiplier += submultiplier;
				printf( "%lu %lu %lu %lu\n", subk[0],subk[1],subk[2],submultiplier );
				fseek(inFile, subPos, SEEK_SET );
				fwrite( clearNibbleBinary, sizeof(unsigned char), NIBBLE_BINARY_BYTE_SIZE, inFile );
			}
			subPos += subRead;
			subRead = fread(subNibbleBinary, sizeof(unsigned char), NIBBLE_BINARY_BYTE_SIZE, inFile );
		}
		
		// set multivalue on board to new multiplier
		multi[0] = multiplier;
		fwrite( nibbleBinary, sizeof(unsigned char), NIBBLE_BINARY_BYTE_SIZE, outFile );

		total += multiplier;
		printf( "multi after scan %lu\r", multiplier); 

	//	fseek( inFile, pos, SEEK_SET );

	///}

	printf( "\nnum boards read:%d\n", pos/33 );

	fclose(inFile);
	fclose(outFile);

	return 0;
}
