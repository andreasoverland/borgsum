#!/usr/local/bin/node

/*

Strategi :
1. Les de første N linjene fra fila.
2. Bygg en map av de N linjene
3. Skriv map til starten av fila.
4. Les de neste N fra slutten av der de siste ble lest inn
5. Bygg en map av de N linjene
6. Skriv map til slutten av siste map i samme fil.
7. repeat til slutten av fila er nådd, truncate fila på slutten av siste write

*/

// TODO: i stedet for å skrive til samme fil, som virket tungt, skriv til en ny, og overskriv

const fs = require('fs');

// binary combiner

let readPosition = 0;
let writePosition = 0;

let masterLineFileName = "level6.bin";

let boardSize = 64;
let numScanLines = 100000;
let maxMapSize = 1000000;
console.time("allocing");
let masterBoards = Buffer.alloc(numScanLines * boardSize);
console.timeEnd("allocing");

let totalLines = 0n;

let writeNum = 0;

let theMap = {};

reduceFile();

function reduceFile() {

	let size = boardSize * numScanLines;
	let file = fs.openSync( masterLineFileName, "r" );


	let numRead = fs.readSync(file, masterBoards, 0, size, readPosition);

	let uniqueLines = 0;

	while( numRead > 0){


		readPosition += numRead;

		// scan igjennom for å finne duplikater

		for (let i = 0; i < numRead; i += 64) {
			let master = Buffer.alloc(64);
			masterBoards.copy(master, 0, i, i + 64);

			let key = makeKeyString(master);

			let whitePieces = master.readBigUInt64LE(0);
			let masterMul = master.readBigUInt64LE(56);
			if (masterMul === 0 || whitePieces === 0) {
				continue;
			}
			if (theMap[key] === undefined) {
				theMap[key] = masterMul;
				uniqueLines++;

			} else {
				theMap[key] += masterMul;
			}
			masterBoards.writeBigUInt64LE(0n, i + 56);


		}
		console.log( "Boards in map", uniqueLines );

		if( uniqueLines > maxMapSize ){
			writeMap(uniqueLines, masterLineFileName );
			uniqueLines = 0;
		}

		numRead = fs.readSync(file, masterBoards, 0, size, readPosition);
	}
	fs.closeSync(file);

	if( uniqueLines > 0){
		writeMap(uniqueLines, masterLineFileName);
	}
}



function writeMap( numLines, filename ){
	console.log( "Writing map with "+numLines+" to file.");
	let outFile = fs.openSync( filename + "_" + pad( writeNum, 6 ) + ".out", "w+" );
	let writeSize = numLines*boardSize;
	console.time("Making writebuffer");
	console.log( "Making writebuffer with writesize  ", writeSize);
	let writeBuffer = Buffer.alloc(writeSize);
	let keyNum = 0;
	let buff = Buffer.alloc(64);
	Object.keys(theMap).forEach(k => {
		totalLines += theMap[k];
		let parts = k.split(":");
		for (let i = 0; i < 7; i++) {
			buff.writeBigUInt64LE(BigInt("0x" + parts[i]), 8 * i);
		}
		buff.writeBigUInt64LE(theMap[k], 56);
		buff.copy(writeBuffer, keyNum * 64, 0, 64);
		keyNum++;
	});

	theMap = {};

	console.timeEnd( "Making writebuffer");
	fs.writeSync( outFile, writeBuffer, 0,writeSize, 0 );
	fs.closeSync( outFile );
	writeNum++;
}

function makeKeyString(buffer) {
	let key = "";
	for (var i = 0; i < 7; i++) {
		key += buffer.readBigUInt64LE(i * 8).toString(16) + ":";
	}
	return key;
}

function pad(num, size) {
    var s = num+"";
    while (s.length < size) s = "0" + s;
    return s;
}
