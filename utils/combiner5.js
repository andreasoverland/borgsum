#!/usr/local/bin/node

// hexdump -e '8/1 "%02X" " " 8/1 "%02X" " " 8/1 "%02X" " " 1/1 "%02X" " " 8/1 "%02X" "\n"'  level1.nbin
// ls -1 |grep -v nbin|xargs -n 1 -I x mv x x.nbin

// 1. Go throug each individual file and combine it
// 2. Load each file as a map
// 2. Use that map to find duplicates in all other files
// 3. Save map-file as combined.nbin.out
// 2. Read next file

const fs = require('fs');

try {
	fs.unlinkSync("combined.nbin.out");
}
catch( w ){}

let startTime = new Date();
/*
split -b66000000 -a5 combined
rm combined
ls -1 x* |xargs -n 1 -I x mv x x.nbin
# run combiner
# check that all files were reduced
rm *.nbin
ls -1 x*|sort -R|xargs cat > combined
rm x*
split -b40722 -a7 combined
rm combined
ls -1 x*|sort -R|xargs cat > combined
rm x*
# repeat
*/
let numBoardsToRead = 2000000;

let boardSize = 33;
let readBuff = Buffer.alloc( numBoardsToRead * boardSize );

let fileNames = fs.readdirSync(".").filter( f => f.endsWith(".nbin") );

for( let i=0;i<fileNames.length;i++){
	console.log( i+"/"+fileNames.length );
	reduceSingleFile( fileNames[i] );
}

let endTime = new Date();
console.log( "Spent " + ((endTime.getTime() - startTime.getTime())/1000) + " seconds" );

// ---------------------- functions --------------------------------------------------------

function makeKey( buff ){
	let key = buff.readUInt32LE(0).toString(36);
	key += '|';
	key += buff.readUInt32LE(4).toString(36);
	key += '|';
	key += buff.readUInt32LE(8).toString(36);
	key += '|';
	key += buff.readUInt32LE(12).toString(36);
	key += '|';
	key += buff.readUInt32LE(16).toString(36);
	key += '|';
	key += buff.readUInt32LE(20).toString(36);
	key += '|';
	key += buff.readUInt8(24).toString(36);
	return key;
}

function reduceSingleFile( fileName ){

	let file = fs.openSync( fileName, "r" );

	let numRead = fs.readSync( file, readBuff, 0, numBoardsToRead*boardSize, 0 );
	let theMap = {};

	for( let i=0;i<numRead/boardSize;i++){

		let board = readBuff.slice(i*boardSize,i*boardSize+33);
		let key = makeKey( board );
		let mul = board.readBigUInt64LE(25);

		if( mul === 0n ){
			continue;
		}

		if( theMap[key] === undefined ) {
			theMap[key] = board;
		}
		else {
			let existingMul = theMap[key].readBigUInt64LE(25);
			existingMul += mul;
			theMap[key].writeBigUInt64LE( existingMul, 25 );
		}
	}
	fs.closeSync( file );

	let reducedFile = fs.openSync( fileName + ".reduced", "w");
	let boards = Object.values( theMap );
	for( let i=0;i<boards.length;i++){
		fs.writeSync( reducedFile, boards[i], 0, 33 );
	}
	fs.closeSync(reducedFile);
	console.log( fileName + " factor:", ( Object.keys(theMap).length) / (numRead/boardSize),numRead/33, boards.length  );

}
