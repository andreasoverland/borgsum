#!/usr/local/bin/node

/*


Eva solfrid øverland, Fredrik chistian Øverdand, andreas dcehvggfvgfgvhrfhgf øverland , ines christiane øveerland, sara franses glaser.

*/

const fs = require('fs');

// binary combiner

//let minNumMasterLines = 4000000;
let minNumMasterLines = 10000;
let masterLineFiles = fs.readdirSync(".");

let numLinesReadAndChecked = 0;

let masterBoards = Buffer.alloc(minNumMasterLines*72);

let scanLineFiles = fs.readdirSync(".");

let masterLineFileName = "lvl3.bin";// masterLineFiles.shift();

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
	let numRead = fs.readSync( file, masterBoards, 0 , size, position );

	console.log("bytes read ", numRead);

	let theMap = {};

	// scan igjennom for å finne duplikater
	for( let i = 0; i < numRead  ; i+=72){
		let master = Buffer.alloc(72);
		masterBoards.copy( master,0 ,i,i+72 );
		let key = master.slice(0,64);
		console.log(key.readBigUInt64LE(7).toString(36));
		let masterMul = master.readBigUInt64LE( 64 );
		if( theMap[key] === undefined ){
			theMap[key] = masterMul;
		}
		else {
			theMap[key] += masterMul;
			masterBoards.writeBigUInt64LE( 0n,i+64 );
		}

		if( (i/72) % 1000	 === 0 ){
			console.log((i/72));
		}
  }

	let totalLines = 0n;
	let uniqueLines = 0n;

	Object.keys( theMap ).forEach( k => {
		let board = Buffer.alloc(72);
		let key = Buffer.from( k );
		key.copy(board,0,0,64);
		key.writeBigUInt64LE(theMap[key],64);
		totalLines += theMap[k];
		uniqueLines ++;
	});


/*
		let origMul = scan.readBigUInt64LE( 64 );
		console.log( origMul );
		let idx = (i+1)*72;
		while( idx >= 0 && idx < size && masterBoards.readBigUInt64LE( idx+64 ) !== 0n ){
			let mul = masterBoards.readBigUInt64LE( idx+64 );
			let whites1 = scan.readBigUInt64LE( 0 );
			let whites2 = masterBoards.readBigUInt64LE( idx );
			console.log( s + " " + mul );
			// masterBoards.writeBigUInt64LE(  0n, idx+64 );
			scan.writeBigUInt64LE( mul + origMul );

			idx = masterBoards.indexOf( scan, idx+72 );
			numDuplicates++;
		}
*/


	console.log( "uniqueLines:", uniqueLines );
	console.log( "totalLines:", totalLines );

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
