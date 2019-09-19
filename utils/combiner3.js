#!/usr/local/bin/node

/*

Eva solfrid øverland, Fredrik chistian Øverdand, andreas dcehvggfvgfgvhrfhgf øverland , ines christiane øveerland, sara franses glaser.

*/


const fs = require('fs');

// binary combiner

//let minNumMasterLines = 4000000;
let minNumMasterLines = 3000000;
let numScanLines = 3000000;
//let minNumMasterLines = 1000;

let allFiles = fs.readdirSync(".");
let masterLineFiles = allFiles.filter( f => f.endsWith(".bin") );

let numLinesReadAndChecked = 0;

let masterBoards = Buffer.alloc(minNumMasterLines * 64);

let scanLineFiles = allFiles.filter( f => f.endsWith(".bin") );

let masterLineFileName =  masterLineFiles[0];

let moreMasterLinesAvailable = true;
let numLinesRead = 0;
let uniqueLines = 0;
let numKeys = 0;
let totalLines = 0n;

let theMap = {};

try {
	fs.unlinkSync("combined.out");
}
catch( w ){
	// yeah whatever
}


while( moreMasterLinesAvailable ){
	console.log( new Date() +  " : Reading " + minNumMasterLines + " master lines from " + masterLineFileName );
	readNextMasterLinesFromNextFile();

	console.log("Num keys:" + numKeys);

	scanAllFilesForMasterLines(); // bigtime
	writeMap();
	console.log( new Date() +  " : Scan done "  );

	masterLineFiles = fs.readdirSync(".").filter( f => f.endsWith(".bin") );
	if( masterLineFiles.length === 0 ){
		break;
	}
	masterLineFileName = masterLineFiles[0];
}

//console.log( "Total number of lines read:", numLinesRead );
//console.log( "Number of unique lines found:", uniqueLines );

function makeKeyString(buffer) {
	numKeys++;
	let key = "";
	for (var i = 0; i < 8; i++) {
		key += buffer.readBigUInt64LE(i * 8).toString(16) + ":";
	}
	return key;
}

function readNextMasterLinesFromNextFile() {

	let size = 64 * minNumMasterLines;
	let file = fs.openSync(masterLineFileName, "r");

	let readNumber = 0;

	// always read from beginning (there should be no used lines in a newly opened file)
	// TODO: use file-size to determine size for read, in case filesize - position < size

	let position = 0;
	let numRead = fs.readSync(file, masterBoards, 0, size, position);

	// scan igjennom for å finne duplikater
	for (let i = 0; i < numRead; i += 64) {
		let master = Buffer.alloc(64);
		masterBoards.copy(master, 0, i, i + 64);

		let key = makeKeyString(master);

		let whitePieces = master.readBigUInt64LE(0);
		let masterMul = master.readBigUInt64LE(56);
		// console.log( key + masterMul );
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

		if ((i / 64) % (minNumMasterLines / 10) === 0) {
			console.log((i / 64));
		}
	}

	// simply copy from numRead -> end into new file

	position += numRead;
	let writePosition = 0;
	let copyBuffer = Buffer.alloc(size);
	let copyFile = fs.openSync(masterLineFileName + ".new", "w");
	numRead = fs.readSync(file, copyBuffer, 0, size, position);

	position += numRead;
	while (numRead > 0) {
		fs.writeSync(copyFile, copyBuffer, 0, numRead, writePosition);
		writePosition += numRead;
		numRead = fs.readSync(file, copyBuffer, 0, size, position);

		position += numRead;
	}

	fs.closeSync(file);
	fs.closeSync(copyFile);

	fs.renameSync( masterLineFileName+".new", masterLineFileName );

	console.log("uniqueLines:", uniqueLines);
	console.log("totalLines:", totalLines);



}


function writeMap() {

	let writeBuff = Buffer.alloc(64 * Object.keys(theMap).length );

	// The keys in "theMap" are the keys we want to scan all files with. But first, we want to remove them from file[0]
	let i = 0;
	Object.keys(theMap).forEach(k => {
		totalLines += theMap[k];
		let buff = Buffer.alloc(64);
		let parts = k.split(":");
		for (let i = 0; i < 8; i++) {
			buff.writeBigUInt64LE(BigInt("0x" + parts[i]), 8 * i);
		}
		buff.writeBigUInt64LE(theMap[k], 56);
		buff.copy(writeBuff, i * 64, 0, 64);
		i++;
	});
	theMap = {};

	fs.writeFileSync("combined.out", writeBuff, {flag:'a'} );
}

function scanAllFilesForMasterLines() {
	scanLineFiles = fs.readdirSync(".").filter( f => f.endsWith(".bin") );
	if( scanLineFiles === 0 ){
		moreMasterLinesAvailable = false;
	}
	else {
		for (let i = 0; i < scanLineFiles.length; i++) {
			scanAndMarkLinesInFile(scanLineFiles[i]);
		}
	}
}

function scanAndMarkLinesInFile(filename) {

	let file = fs.openSync(filename, "r");
	let newFile = fs.openSync(filename + ".new", "w");

	let size = numScanLines*64;
	let position = 0;
	let readBuffer = Buffer.alloc(size);

	let numLinesWritten = 0;

	let writeBuffer = Buffer.alloc(size);
	let numLinesInBuff = 0;
	let writeFilePosition = 0;

	let numRead = fs.readSync(file, readBuffer, 0, size, position);


	while (numRead > 0) {
		position += numRead;
		for( let i = 0; i<numRead;i+=64){
			let scanLine = readBuffer.slice( i, i+64);
			let key = makeKeyString(scanLine);
			if( theMap[key] !== undefined ){
				theMap[key] += scanLine.readBigUInt64LE(56);
			}
			else {
				scanLine.copy( writeBuffer, numLinesInBuff*64, 0, 64 );
				numLinesInBuff++;
			}

			if(numLinesInBuff == numScanLines){
				// write to reduced version of current file
				fs.writeSync(newFile, writeBuffer,0,numLinesInBuff*64, writeFilePosition);
				writeFilePosition += size;
				numLinesWritten +=  numLinesInBuff;
				numLinesInBuff = 0;
			}
		}

		readBuffer.fill(0);
		numRead = fs.readSync(file, readBuffer, 0, size, position);
	}

	if (numLinesInBuff > 0) {
		numLinesWritten +=  numLinesInBuff;
		fs.writeSync(newFile, writeBuffer,0,numLinesInBuff*64,writeFilePosition);
		numLinesInBuff = 0;
	}

	fs.closeSync(file);
	fs.closeSync(newFile);

	if (numLinesWritten === 0) {
		// the file has no more lines of interest. delete it
		fs.unlinkSync(filename + ".new");
		fs.unlinkSync(filename);
	} else {
		fs.renameSync(filename + ".new", filename);
	}


}
