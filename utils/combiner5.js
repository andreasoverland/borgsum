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
let startTime = new Date();
// use : split -b66000000 
let numBoardsToRead = 2000000;
let boardSize = 33;
let readBuff = Buffer.alloc( numBoardsToRead * boardSize );
let writeBuffer = Buffer.alloc( numBoardsToRead * boardSize );
let totalBoardsWrittenToMap = 0;


let fileNames = fs.readdirSync(".").filter( f => f.endsWith(".nbin") );

fileNames.forEach( reduceSingleFile );

/*
while( fileNames.length > 0 ){
	let map = {};

	loadFileAsMap( fileNames.shift() , map );

	let numKeys =  Object.keys(map).length;
	console.log( "Keys in map : ", numKeys );

	for( let i=0;i<fileNames.length;i++){
		reduceFile( fileNames[i], map );
	}
	console.log( "Done scanning files. Writing map to tile");
	appendMapToFile( "combined.nbin.out", map );
	
}
*/

console.log( "totalBoardsWrittenToMap :",totalBoardsWrittenToMap);
let endTime = new Date();
console.log( "Spent " + ((endTime.getTime() - startTime.getTime())/1000) + " seconds" );

function writeMapToFile( fileName, map ){
	let numBoards =  Object.keys(map).length;
	console.log( "Writing " + numBoards + " boards to " + fileName );

	let i = 0;
	Object.keys(map).forEach( k => {
		fillBufferFromKey( writeBuffer, k, i*boardSize );
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
		fillBufferFromKey( writeBuffer, k, i*boardSize )
		writeBuffer.writeBigUInt64LE( map[k], 25 + i*boardSize);
		i++;
	});

	let mapFile = fs.openSync( fileName ,"a" );
	console.log( "Appending " + numBoards + " boards to " + fileName + " (" + mapFile +"), " + i*boardSize +" bytes." );
	fs.writeSync( mapFile, writeBuffer, 0, i*boardSize,totalBoardsWrittenToMap*boardSize );
	totalBoardsWrittenToMap += numBoards;
	fs.closeSync( mapFile );

}

function makeKey( buff ){
	let key = buff.readBigUInt64LE(0);
	key <<= 64n;
	key |= buff.readBigUInt64LE(8);
	key <<= 64n;
	key |= buff.readBigUInt64LE(16);
	key <<= 8n;
	key |= BigInt( buff.readUInt8(24) );
	return key;
}

function fillBufferFromKey( buffer, k, offSet){
	let key = BigInt( k );

	let moveNum = key & 0xFFn;
	key >>= 8n;

	let k2 = key & 0xFFFFFFFFFFFFFFFFn;
	key >>= 64n;
	let k1 = key & 0xFFFFFFFFFFFFFFFFn;
	key >>= 64n;
	let k0 = key & 0xFFFFFFFFFFFFFFFFn;

	buffer.writeBigUInt64LE( k0, 0 + offSet);
	buffer.writeBigUInt64LE( k1, 8 + offSet);
	buffer.writeBigUInt64LE( k2, 16 + offSet);
	buffer.writeUInt8( Number(moveNum), 24 + offSet);
}

// Assumes that the file has unique boards
function loadFileAsMap( fileName, theMap ){
	console.log( "Loading map from " + fileName );
	let file = fs.openSync( fileName, "r" );

	let numRead = fs.readSync( file, readBuff, 0, numBoardsToRead*boardSize, 0 );
	for( let i=0;i<numRead/33;i++){
		let board = readBuff.slice(i*33,(i+1)*33);
		let key = makeKey(board);
		let mul = board.readBigUInt64LE(25);
		if( mul === 0n ){
			continue;
		}
		theMap[key] = mul;
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

		let board = readBuff.slice(i*boardSize,(i+1)*boardSize);
		let key = makeKey( board );
		let mul = board.readBigUInt64LE(25);

		if( mul === 0n ){
			continue;
		}
		if( theMap[key] === undefined ) {
			// write board to buff to keep
			board.copy( writeBuffer, writePos, 0, boardSize );
			writePos += boardSize;
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

	for( let i=0;i<numRead/boardSize;i++){
		let board = readBuff.slice(i*boardSize,(i+1)*boardSize);
		let key = makeKey( board );
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
	fs.closeSync( file );
	console.log( "Boards in map: ", Object.keys( theMap).length() );
	//writeMapToFile( fileName, theMap );
	theMap = {};
}
