#!/usr/local/bin/node

/*


Eva solfrid øverland, Fredrik chistian Øverdand, andreas dcehvggfvgfgvhrfhgf øverland , ines christiane øveerland, sara franses glaser.

*/

const fs = require('fs');

// Combiner 2
// Strategi : Slett nøkler og matcher fra filene, for å lage mindre filer
// - Les nøkler ut av fil 1
// - Hvis alle linjer er brukt til nøkler, slett fil 1
// - Ellers skriv resterende linker til file 1.new
// - Flytt fil 1.new til 1
// - Les hver fil, ta vare på ubrukte linjer i ny fil, og overskriv når enden på fila er nådd

// original data : 5941248 kb
//     1046 : 112 ,   24528Kb,   219Kb/sec
//    16804 : 115 ,   57252Kb,   498Kb/sec
//   135404 : 130 ,  106684Kb,   821Kb/sec
//   506989 : 139 ,  324580Kb,  2335Kb/sec
//  1049080 : 138 ,  552684Kb,  4005Kb/sec
//  2256721 : 140 , 1174248Kb,  8387Kb/sec
//  4520146 : 142,  2275524Kb, 16025Kb/sec **
//  5337391 : 168 , 2525568Kb, 15033Kb/sec
// 10000000 : Not finished reading linesn after 18 minNumMasterLines

let minNumMasterLines = 3000000;
let masterLineFiles = fs.readdirSync(".");

let numLinesReadAndChecked = 0;
let map = {};
let scanLineFiles = fs.readdirSync(".");

let masterLineFileName = masterLineFiles.shift();

let moreMasterLinesAvailable = true;
let numLinesRead = 0;
let uniqueLines = 0;
let minLineLength = 1000;

fs.writeFileSync("../combined.txt", Buffer.from( "", "utf-8") );

//while( moreMasterLinesAvailable ){
	console.log( new Date() +  " : Reading " + minNumMasterLines + " master lines from " + masterLineFileName );
	readNextMasterLinesFromNextFile();
	let counts = [0,0,0,0,0,0];

	let minLength = 1000;
	let shortestKey = "";

	console.log( "Shortest k4 " + minLength + " " + shortestKey );
	console.log( "Min line length " + minLineLength );
	console.log( counts );


	scanAllFilesForMasterLines(); // bigtime
	writeMap();
	console.log( new Date() +  " : Scan done "  );
//}

console.log( "Total number of lines read:", numLinesRead );
console.log( "Number of unique lines found:", uniqueLines );

function makeKeyArray( line ){
	let a = [];
	a.push( line.substring(0,8) );
	a.push( line.substring(8,16) );
	a.push( line.substring(16,24) );
	a.push( line.substring(24) );
	return a;
}



function readNextMasterLinesFromNextFile(){
	let size = 40*minNumMasterLines;
	let file = fs.openSync(masterLineFileName,"r");

	let readBuffer = Buffer.alloc(size);

	// always read from beginning (there should be no used lines in a newly opened file)
	let position = 0;
	let numRead = fs.readSync( file, readBuffer, 0 , size, position );

	while( uniqueLines < minNumMasterLines ){

		let strBuf = readBuffer.toString("UTF-8");
		let lines = strBuf.trim().split("\n");

		while( lines[lines.length-1].startsWith("\0")){
			lines.pop();
		}

		if (numRead === size && strBuf[numRead-1] !== '\n') {
			let lastLine = lines.pop();
			let lengthOfLastLine = lastLine.length;
			numRead -= lengthOfLastLine; // for next read
		}


		numLinesRead += lines.length;

		for (let i = 0; i < lines.length; i++) {

			// TODO: Split the line in 5 byte chunks, and make binary tree with 4 levels

			let p = lines[i].split("m");
			let keys = makeKeyArray( p[0] );

			if( minLineLength > p[0].length ){
				minLineLength = p[0].length;
			}

			if( map[ keys[0] ] === undefined ){
				map[ keys[0] ] = {};
			}
			if( map[ keys[0] ][ keys[1] ] === undefined ){
				map[ keys[0] ][ keys[1] ] = {};
			}
			if( map[ keys[0] ][ keys[1] ][ keys[2] ] === undefined ){
				map[ keys[0] ][ keys[1] ][ keys[2] ] = {};
			}
			if( map[ keys[0] ][ keys[1] ][ keys[2] ][keys[3] ] === undefined ){
				map[ keys[0] ][ keys[1] ][ keys[2] ][keys[3] ] = parseInt(p[1]);
				uniqueLines++;
			}
			else {
				map[ keys[0] ][ keys[1] ][ keys[2] ][keys[3] ] += parseInt(p[1]);
			}

		}

		position += numRead;

		readBuffer = Buffer.alloc(size);
		numRead = fs.readSync(file, readBuffer, 0, size, position );

		if( numRead <= 0 && masterLineFiles.length > 0){
			// simply delete the current master file. it is empty by now.@
			// close the file
			fs.closeSync( file );
			// delete the file
			fs.unlinkSync( masterLineFileName );
			// set position to 0
			position = 0;
			// open next masterLineFile
			masterLineFileName = masterLineFiles.shift();
			file = fs.openSync(masterLineFileName,"r");
			// continue reading there
			readBuffer = Buffer.alloc(size);
			numRead = fs.readSync(file, readBuffer, 0, size, position );

		}
		else if ( numRead <= 0 && masterLineFiles.length === 0 ){
			// simply delete the current master file. it is empty by now.
			// close the file
			fs.closeSync( file );
			// delete the file
			fs.unlinkSync( masterLineFileName );

			console.log("***** No more master lines available.");
			moreMasterLinesAvailable = false;

			// return from a job well done
			return;
		}
	}


	// skriv til ny fil
	let copySize = 1024*1024;
	let copyFile = fs.openSync(masterLineFileName+".copy","w");
	let copyPosition = 0;
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

}


function writeMap(){

	let buff = "";
	Object.keys(map).forEach( k1 => {
		Object.keys(map[k1]).forEach( k2 => {
			Object.keys(map[k1][k2]).forEach( k3 => {
				Object.keys(map[k1][k2][k3]).forEach( k4 => {
					buff += (k1 + k2 + k3 + k4 + "m" + (map[k1][k2][k3][k4]) + "\n");
				});
			});
		});
	});

	fs.writeFileSync("../combined.txt", Buffer.from( buff, "utf-8"), {flag: 'a'});
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
				 map[k[0]][k[1]] !== undefined &&
				 map[k[0]][k[1]][k[2]] !== undefined &&
				 map[k[0]][k[1]][k[2]][k[3]] !== undefined ){
				 map[k[0]][k[1]][k[2]][k[3]] += parseInt(p[1]);
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
