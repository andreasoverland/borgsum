#!/usr/local/bin/node

/*


Eva solfrid øverland, Fredrik chistian Øverdand, andreas dcehvggfvgfgvhrfhgf øverland , ines christiane øveerland, sara franses glaser.

*/

const fs = require('fs');


// binary combiner

//let minNumMasterLines = 4000000;
let minNumMasterLines = 1000;
let masterLineFiles = fs.readdirSync(".");

let numLinesReadAndChecked = 0;
let map = {};
let scanLineFiles = fs.readdirSync(".");

let masterLineFileName = "mates.out"; masterLineFiles.shift();

let moreMasterLinesAvailable = true;
let numLinesRead = 0;
let uniqueLines = 0;

fs.writeFileSync("combined.bin", Buffer.from("") );


//while( moreMasterLinesAvailable ){
	console.log( new Date() +  " : Reading " + minNumMasterLines + " master lines from " + masterLineFileName );
	readNextMasterLinesFromNextFile();

	//scanAllFilesForMasterLines(); // bigtime
	writeMap();
	console.log( new Date() +  " : Scan done "  );
//}

//console.log( "Total number of lines read:", numLinesRead );
//console.log( "Number of unique lines found:", uniqueLines );


function readNextMasterLinesFromNextFile(){
	let size = 9*8*minNumMasterLines;
	let file = fs.openSync(masterLineFileName,"r");

	let readNumber = 0;
	let readBuffer = Buffer.alloc(size);

	// always read from beginning (there should be no used lines in a newly opened file)
	// TODO: use file-size to determine size for read, in case filesize - position < size

	let position = 0;
	let numRead = fs.readSync( file, readBuffer, 0 , size, position );

	console.log("bytes numread key lines", numRead);
	console.time("adding keys to map");

	for( var i=0;i<minNumMasterLines;i++){
		let board = readBuffer.slice( i*9*8, i*9*8+9*8 );
		let key = board.slice(0,8*8);

		if( map[key] === undefined ){
			map[key] = board.readBigUInt64LE(8*8);
		}
		else {
			map[key] += board.readBigUInt64LE(8*8);
		}
	}


	console.timeEnd("adding keys to map" );

	console.log("Num keys in map:" ,  Object.keys(map).length)



/*
	// skriv til ny fil
	let copySize = 1024*1024;
	let copyFile = fs.openSync(masterLineFileName+".copy","w");
	let copyPosition = 0;
	readBuffer.
	readBuffer = Buffer.alloc(copySize);
	numRead = fs.readSync( file, readBuffer, 0, copySize, position );
	while( numRead > 0) {
		fs.writeSync(copyFile, readBuffer, 0, numRead, copyPosition);
		copyPosition += numRead;
		position += numRead;
		numRead = fs.readSync( file, readBuffer, 0, copySize, position );
	}
	fs.closeSync( file );
	fs.closeSync( copyFile );
	fs.renameSync( masterLineFileName+".copy", masterLineFileName );
*/
	console.log("Key reading done");
}


function writeMap(){

	Object.keys(map).forEach( keyBuff => {
		let writeBuff = Buffer.alloc(9*8);
		writeBuff.fill( keyBuff, 0, 8*8 );
		writeBuff.writeBigUInt64LE( map[keyBuff],8*8 );
		console.log( writeBuff.readBigUInt64LE(0) );
		console.log( writeBuff.readBigUInt64LE(1) );
		console.log( writeBuff.readBigUInt64LE(2) );
		console.log( writeBuff.readBigUInt64LE(3) );

		console.log( writeBuff.readBigUInt64LE(4) );
		console.log( writeBuff.readBigUInt64LE(5) );
		console.log( writeBuff.readBigUInt64LE(6) );
		console.log( writeBuff.readBigUInt64LE(7) );

		console.log( writeBuff.readBigUInt64LE(8) );
		console.log( writeBuff.readBigUInt64LE(9) );

		console.log("----");
		fs.writeFileSync("combined.bin", writeBuff, {flag: 'a'});
	});


	map = {};

}


function scanAllFilesForMasterLines(){
	scanLineFiles = fs.readdirSync(".");
	for( let i=0;i<scanLineFiles.length;i++){
		scanAndMarkLinesInFile(scanLineFiles[i]);
	}
}


function scanAndMarkLinesInFile( filename ) {

	let file = fs.openSync(filename,"r");
	let newFile = fs.openSync( filename+".new", "w");

	let size = 1000000;
	let position = 0;
	let buffer = Buffer.alloc(size);
	let numRead = fs.readSync(file, buffer, 0, size, position);

	let numLinesWritten = 0;

	let buff = "";
	let numLinesInBuff = 0;

	while (numRead > 0) {

		let strBuf = buffer.toString("UTF-8");
		let lines = strBuf.trim().split("\n");

		while( lines[lines.length-1].startsWith("\0")){
			lines.pop();
		}

		if (numRead === size && strBuf[numRead-1] !== '\n') {
			let lastLine = lines.pop();
			let lengthOfLastLine = lastLine.length;
			numRead -= lengthOfLastLine; // for next read
		}

		numLinesReadAndChecked += lines.length;

		for (let i = 0; i < lines.length; i++) {

			let p = lines[i].split("m");

			let k = makeKeyArray( p[0] );

			if ( map[k[0]] !== undefined &&
				 	 map[k[0]][k[1]] !== undefined ) {
					 map[k[0]][k[1]] += parseInt(p[1]);
			}
			else {
				numLinesWritten++;
				numLinesInBuff++;
				buff += lines[i] + "\n";
				if( numLinesInBuff > 100000 ){
					fs.writeFileSync( newFile, buff );
					buff = "";
					numLinesInBuff = 0;
				}
			}
		}

		position += numRead;

		buffer = Buffer.alloc(size);
		numRead = fs.readSync(file, buffer, 0, size, position);
	}

	if( numLinesInBuff > 0 ){
		fs.writeFileSync( newFile, buff );
	}

	fs.closeSync( file );
	fs.closeSync( newFile );

	if( numLinesWritten === 0){
		// the file has no more lines of interest. delete it

		fs.unlinkSync( filename+".new" );
		fs.unlinkSync( filename );
	}
	else {
		fs.renameSync( filename+".new", filename );
	}

}
