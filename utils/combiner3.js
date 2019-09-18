#!/usr/local/bin/node

/*

Eva solfrid øverland, Fredrik chistian Øverdand, andreas dcehvggfvgfgvhrfhgf øverland , ines christiane øveerland, sara franses glaser.

*/



const fs = require('fs');

// binary combiner

//let minNumMasterLines = 4000000;
let minNumMasterLines   = 10000;
//let minNumMasterLines = 1000;

let masterLineFiles = fs.readdirSync(".");

let numLinesReadAndChecked = 0;

let masterBoards = Buffer.alloc(minNumMasterLines*72);

let scanLineFiles = fs.readdirSync(".");

let masterLineFileName = "level5.bin";// masterLineFiles.shift();

let moreMasterLinesAvailable = true;
let numLinesRead = 0;
let uniqueLines = 0;
let numKeys = 0;

//fs.writeFileSync("combined.bin", Buffer.from("") );

//while( moreMasterLinesAvailable ){
	//console.log( new Date() +  " : Reading " + minNumMasterLines + " master lines from " + masterLineFileName );
	readNextMasterLinesFromNextFile();

	console.log( "Num keys:" + numKeys );

	//scanAllFilesForMasterLines(); // bigtime
	writeMap();
	//console.log( new Date() +  " : Scan done "  );
//}

//console.log( "Total number of lines read:", numLinesRead );
//console.log( "Number of unique lines found:", uniqueLines );

function makeKeyString( buffer ){
	numKeys ++;
	let key = "";
	for( var i=0;i<8;i++){
		key += buffer.readBigUInt64LE(i*8).toString(16) + ":";
	}
	return key;
}

function readNextMasterLinesFromNextFile(){

	let size = 9*8*minNumMasterLines;
	let file = fs.openSync(masterLineFileName,"r");

	let readNumber = 0;


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

		let key = makeKeyString( master );

		let whitePieces = master.readBigUInt64LE( 0 );
		let masterMul = master.readBigUInt64LE( 64 );
		// console.log( key + masterMul );
		if( masterMul === 0 || whitePieces === 0){
			continue;
		}
		if( theMap[key] === undefined ){
			theMap[key] = masterMul;
			uniqueLines++;
		}
		else {
			theMap[key] += masterMul;
		}
		masterBoards.writeBigUInt64LE( 0n,i+64 );

		if( (i/72) % (minNumMasterLines/10) === 0 ){
			console.log((i/72));
		}
    }



	let totalLines = 0n;

	let writeBuff = Buffer.alloc( 72 * uniqueLines );

	// The keys in "theMap" are the keys we want to scan all files with. But first, we want to remove them from file[0]
	let i = 0;
	Object.keys( theMap ).forEach( k => {
		totalLines += theMap[k];
		let buff = Buffer.alloc(72);
		let parts = k.split(":");
		for( let i = 0;i<8;i++){
			buff.writeBigUInt64LE( BigInt( "0x"+parts[i] ), 8*i );
		}
		buff.writeBigUInt64LE( theMap[k],64 );
		buff.copy( writeBuff, i*72, 0, 72 );
		i++;
	});

	// simply copy from numRead -> end into new file

	position += numRead;
	let writePosition = 0;
	let copyBuffer = Buffer.alloc(size);
	let copyFile = fs.openSync(masterLineFileName+".new","w");
	numRead = fs.readSync( file, copyBuffer, 0 , size, position );
	console.log( "read " + numRead + " bytes from position " + position );
	position += numRead;
	while( numRead > 0 ){
		fs.writeSync( copyFile, copyBuffer, 0, numRead, writePosition );
		writePosition += numRead;
		numRead = fs.readSync( file, copyBuffer, 0 , size, position );
		console.log( "read " + numRead + " bytes from position " + position );
		position += numRead;
	}

	fs.closeSync(file);
	fs.closeSync(copyFile);
/*
	try {
		fs.unlinkSync("combined.bin");
	}
	catch( w ){
		// yeah whatever
	}
	fs.writeFileSync("combined.bin", writeBuff, {flag:'a'} );
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
//	console.log("Key reading done");
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
