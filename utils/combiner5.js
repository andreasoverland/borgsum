#!/usr/local/bin/node

// hexdump -e '8/1 "%02X" " " 8/1 "%02X" " " 8/1 "%02X" " " 1/1 "%02X" " " 8/1 "%02X" "\n"'  level1.nbin
// ls -1|xargs -n 1 -I x mv x x.nbin

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

let numBoardsToRead = 200000;
let boardSize = 33;
let readBuff = Buffer.alloc( numBoardsToRead * boardSize );
let writeBuffer = Buffer.alloc( numBoardsToRead * boardSize );
let totalBoardsWrittenToMap = 0;


let map = {};

let fileNames = fs.readdirSync(".").filter( f => f.endsWith(".nbin") );

fileNames.forEach( reduceSingleFile );


while( fileNames.length > 0 ){
	loadFileAsMap( fileNames.shift() , map );

	let numKeys =  Object.keys(map).length;
	console.log( "Keys in map : ", numKeys );

	for( let i=0;i<fileNames.length;i++){
	//for( let i=0;i<4;i++){
		reduceFile( fileNames[i], map );
	}
	console.log( "Done scanning files. Writing map to tile");
	appendMapToFile( "combined.nbin.out", map );
	map = {};
}

console.log( "totalBoardsWrittenToMap :",totalBoardsWrittenToMap);

function writeMapToFile( fileName, map ){
	let numBoards =  Object.keys(map).length;
	console.log( "Writing " + numBoards + " boards to " + fileName );

	let i = 0;
	Object.keys(map).forEach( k => {
		let key = BigInt( k );
		let k2 = key & 0xFFFFFFFFFFFFFFFFn;
		key >>= 64n;
		let k1 = key & 0xFFFFFFFFFFFFFFFFn;
		key >>= 64n;
		let k0 = key & 0xFFFFFFFFFFFFFFFFn;
		key >>= 64n;
		writeBuffer.writeBigUInt64LE( k0, 0 + i*boardSize);
		writeBuffer.writeBigUInt64LE( k1, 8 + i*boardSize);
		writeBuffer.writeBigUInt64LE( k2, 16 + i*boardSize);
		writeBuffer.writeUInt8( 5, 24 + i*boardSize);
		writeBuffer.writeBigUInt64LE( map[k], 25 + i*boardSize);
		i++;
	});

	let mapFile = fs.openSync( fileName ,"w" );
	fs.writeSync( mapFile, writeBuffer, 0, i*33, 0 );
	fs.closeSync( mapFile );

}

function appendMapToFile( fileName, map ){
	let numBoards =  Object.keys(map).length;


	let i = 0;
	Object.keys(map).forEach( k => {
		let key = BigInt( k );
		let k2 = key & 0xFFFFFFFFFFFFFFFFn;
		key >>= 64n;
		let k1 = key & 0xFFFFFFFFFFFFFFFFn;
		key >>= 64n;
		let k0 = key & 0xFFFFFFFFFFFFFFFFn;
		key >>= 64n;
		writeBuffer.writeBigUInt64LE( k0, 0 + i*boardSize);
		writeBuffer.writeBigUInt64LE( k1, 8 + i*boardSize);
		writeBuffer.writeBigUInt64LE( k2, 16 + i*boardSize);
		writeBuffer.writeUInt8( 6, 24 + i*boardSize);
		writeBuffer.writeBigUInt64LE( map[k], 25 + i*boardSize);
		i++;
	});

	let mapFile = fs.openSync( fileName ,"a" );
	console.log( "Appending " + numBoards + " boards to " + fileName + " (" + mapFile +"), " + i*boardSize +" bytes." );
	fs.writeSync( mapFile, writeBuffer, 0, i*boardSize,totalBoardsWrittenToMap*boardSize );
	totalBoardsWrittenToMap += numBoards;
	fs.closeSync( mapFile );

}


function loadFileAsMap( fileName, theMap ){
	console.log( "Loading map from " + fileName );
	let file = fs.openSync( fileName, "r" );

	let numRead = fs.readSync( file, readBuff, 0, numBoardsToRead*boardSize, 0 );
	let total = 0;
	let unique = 0;
	for( let i=0;i<numRead/33;i++){
		total++;
		let board = readBuff.slice(i*33,(i+1)*33);
		let key = (board.readBigUInt64LE(0) << 128n)| (board.readBigUInt64LE(8) << 64n) | board.readBigUInt64LE(16);
		// key <<= 8n;

		let mul = board.readBigUInt64LE(25);

		if( mul === 0n ){
			continue;
		}

		if( theMap[key] === undefined ) {
			theMap[key] = mul;
			unique++;
		}
		else {
			theMap[key] += mul;
		}
	}
	//console.log( "Mapped " + unique + " unique, in " + total + " total.");
	fs.closeSync( file );
	fs.unlinkSync( fileName );
}

function reduceFile( fileName, theMap ){

	let file = fs.openSync( fileName, "r" );
	let writePos = 0;

	let numRead = fs.readSync( file, readBuff, 0, numBoardsToRead*boardSize, 0 );
	for( let i=0;i<numRead/33;i++){

		let board = readBuff.slice(i*33,(i+1)*33);
		let key = (board.readBigUInt64LE(0) << 128n)| (board.readBigUInt64LE(8) << 64n) | board.readBigUInt64LE(16);
		let mul = board.readBigUInt64LE(25);
		if( mul === 0n ){
			continue;
		}
		if( theMap[key] === undefined ) {
			// write board to buff to keep
			board.copy( writeBuffer, writePos, 0, 33 );
			writePos += 33;
		}
		else {
			theMap[key] += mul;
		}
	}
	fs.closeSync( file );

	let writeFile = fs.openSync( fileName, "w" );
	fs.writeSync( writeFile, writeBuffer, 0, writePos, 0 );
	fs.closeSync( writeFile );


}


function reduceSingleFile( fileName ){
	console.log( "Reducing file " + fileName );
	let file = fs.openSync( fileName, "r" );

	let numRead = fs.readSync( file, readBuff, 0, numBoardsToRead*boardSize, 0 );
	let theMap = {};
	let total = 0;
	let unique = 0;
	for( let i=0;i<numRead/33;i++){
		total++;
		let board = readBuff.slice(i*33,(i+1)*33);
		let key = (board.readBigUInt64LE(0) << 128n)| (board.readBigUInt64LE(8) << 64n) | board.readBigUInt64LE(16);
		// key <<= 8n;

		let mul = board.readBigUInt64LE(25);

		if( mul === 0n ){
			continue;
		}

		if( theMap[key] === undefined ) {
			theMap[key] = mul;
			unique++;
		}
		else {
			theMap[key] += mul;
		}
	}
	//console.log( "Mapped " + unique + " unique, in " + total + " total.");
	fs.closeSync( file );

	writeMapToFile( fileName, theMap );
}
