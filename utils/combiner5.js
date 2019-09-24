#!/usr/local/bin/node


// 1. Load a file as a map
// 2. Use that map to find duplicates in all other files
// 3. Save map-file as ../combined.nbin
// 2. Read next file

const fs = require('fs');


let numBoardsToRead = 200000;
let boardSize = 33; 

let map = {};

let fileNames = fs.readdirSync(".").filter( f => f.endsWith(".nbin") );


loadFileAsMap( fileNames.shift() , map );
for( let i=0;i<fileNames.length;i++){
	reduceFile( fileNames[i], map );
}
console.log( "Done scanning files. Write map to combined.out");


console.log( Object.keys(map).length );

function loadFileAsMap( fileName, theMap ){
	let file = fs.openSync( fileName, "r" );
	let readBuff = Buffer.alloc( numBoardsToRead*boardSize );
	
	console.time( "Reading data" );
	let numRead = fs.readSync( file, readBuff, 0, numBoardsToRead*boardSize, 0 );
	console.timeEnd( "Reading data");
	console.log( "Read " + numRead + " bytes" );
	console.time( "Mapping data.");
	for( let i=0;i<numRead/33;i++){

		let board = readBuff.slice(i*33,(i+1)*33);
		let key = (board.readBigUInt64LE(0) << 128n)| (board.readBigUInt64LE(8) << 64n) | board.readBigUInt64LE(16);
		let mul = board.readBigUInt64LE(25);

		if( mul === 0n ){
			continue;
		}

		if( theMap[key] === undefined ) {
			theMap[key] = mul;
		}
		else {
			theMap[key] += mul;
		}
	}
	console.timeEnd( "Mapping data.");
	fs.closeSync( file );	
}

function reduceFile( fileName, theMap ){
	
	let file = fs.openSync( fileName, "r" );
	let readBuff = Buffer.alloc( numBoardsToRead*boardSize );
	let writeBuff = Buffer.alloc( numBoardsToRead*boardSize );
	let writePos = 0;

	console.time( "Reading data" );
	let numRead = fs.readSync( file, readBuff, 0, numBoardsToRead*boardSize, 0 );
	console.timeEnd( "Reading data");
	console.log( "Read " + numRead + " bytes" );
	console.time( "Mapping data.");
	for( let i=0;i<numRead/33;i++){

		let board = readBuff.slice(i*33,(i+1)*33);
		let key = (board.readBigUInt64LE(0) << 128n)| (board.readBigUInt64LE(8) << 64n) | board.readBigUInt64LE(16);
		let mul = board.readBigUInt64LE(25);
		if( mul === 0n ){
			continue;
		}
		if( theMap[key] === undefined ) {
			// write board to buff to keep
			board.copy( writeBuff, writePos, 0, 33 );
			writePos += 33;
		}
		else {
			theMap[key] += mul;
		}
	}
	console.timeEnd( "Mapping data.");
	fs.closeSync( file );	

	console.log( "Write buff with " + writePos/33 + " boards to " + fileName );

}