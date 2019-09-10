#!/usr/local/bin/node

const fs = require('fs');

// åpne første fil i folderen (antar at det er mange filer som skal kombineres, multipleres og reduseres)
// les 1000 første unike linjer/brett
//  alt må skje sync
//  fortsett å lese linjer, og sammenlikn med innleste
//  hvis det er match, kombiner&pultipliser, marker linje som brukt (med # feks på første char)
//  fortsett å les til alle linjer i alle filer er lest
//  skriv alle kombinerte linjer i en ny fil
// les nye 1000 (ubrukte) linjer, og repeter

let numMasterLines = 10000;
let masterLines = [];
let masterLineFiles = fs.readdirSync(".");

let numLinesReadAndChecked = 0;
let map = {};
let scanLineFiles = fs.readdirSync(".");

let masterLineFileName = masterLineFiles.shift();
let masterLineFilePosition = 0;

let moreMasterLinesAvailable = true;

fs.writeFileSync("../combined.txt", Buffer.from( "", "utf-8") );

while( moreMasterLinesAvailable ){
	masterLines = [];
	let currentFileName = masterLineFileName;
	readNextMasterLinesFromNextFile();
	console.log( "Number of lines read from " + currentFileName + " ", Object.keys(map).length);
	prepareScan(currentFileName);
}
// les ut og marker fra en fil, og så er man "ferdig" med den filen.
// når alle filer er gjennomgått tar vi en ny runde på samme fila, helt til det kun er #-linjer i den fila,
// hvorpå vi begynner å lese fra neste fil.


function readNextMasterLinesFromNextFile(){
	let size = 100000;
	let file = fs.openSync(masterLineFileName,"rs+");

	let readBuffer = Buffer.alloc(size);
	let numRead = fs.readSync(file,readBuffer,0,size,masterLineFilePosition);

	while( Object.keys(map).length < numMasterLines ){

		let lines = readBuffer.toString("UTF-8").trim().split("\n");

		if (numRead === size) {
			let lastLine = lines.pop();
			let lengthOfLastLine = lastLine.length;
			numRead -= lengthOfLastLine; // for next read
		}

		let dirty = false;

		for (let i = 0; i < lines.length; i++) {
			if ( !lines[i].startsWith("#") && lines[i].length < 100 ) {

				let p = lines[i].split("m");
				if (map[p[0]] === undefined ) {
					map[p[0]] = parseInt(p[1]);
					lines[i] = "#" + lines[i].substring(1);
					dirty = true;
				}
				else {
					map[p[0]] += parseInt(p[1]);
					lines[i] = "#" + lines[i].substring(1);
				}
			}
		}

		if (dirty) {
			let writeBuffer = lines.join("\n");
			fs.writeSync(file, writeBuffer, masterLineFilePosition, "UTF-8");
		}

		masterLineFilePosition += numRead;

		readBuffer = Buffer.alloc(size);
		numRead = fs.readSync(file, readBuffer, 0, size, masterLineFilePosition);

		if( numRead <= 0 && masterLineFiles.length > 0){
			masterLineFileName = masterLineFiles.shift();
			masterLineFilePosition = 0;
			console.log("** Next master line file :", masterLineFileName );
			scanLineFiles.shift();
			break;
		}
		else if ( numRead <= 0 && masterLineFiles.length === 0 ){
			console.log("***** No more master lines available.");
			moreMasterLinesAvailable = false;
			break;
		}
	}

	fs.closeSync(file);

}


function prepareScan(currentFileName){

	console.log( "Ready for big scan with " + Object.keys(map).length + " master lines");
	if( Object.keys(map).length === 4330 ){
		fs.writeFileSync("buggy-keys.json", Buffer.from( JSON.stringify(map,null,2), "utf-8"));
	}
	scanAllFilesForMasterLines(currentFileName); // bigtime

	console.log( "Scanned. Writing map.");
	let buff = "";
	Object.keys(map).forEach( k=>{
		buff += (k + "m" + map[k] + "\n");
	});
	fs.writeFileSync("../combined.txt", Buffer.from( buff, "utf-8"), {flag: 'a'});
	map = {};

}

function scanAllFilesForMasterLines(currentFileName){

	for( let i=0;i<scanLineFiles.length;i++){
		let file = fs.openSync(scanLineFiles[i],"rs+");
		process.stdout.write( scanLineFiles[i] + " " );
		let readStartPos = 0;
		if( currentFileName === scanLineFiles[i]){
			readStartPos = masterLineFilePosition;
		}
		scanAndMarkLinesInFile(file,readStartPos);
		fs.closeSync(file);
	}
	process.stdout.write("\n");
}


function scanAndMarkLinesInFile( file ,readStartPos) {
 	let size = 100000;
	let position = readStartPos;
	let buffer = Buffer.alloc(size);
	let numRead = fs.readSync(file, buffer, 0, size, position);

	while (numRead > 0) {

		let lines = buffer.toString("utf-8").trim().split("\n");

		if (numRead === size) {
			let lastLine = lines.pop();
			let lengthOfLastLine = lastLine.length;
			numRead -= lengthOfLastLine; // for next read
		}

		numLinesReadAndChecked += lines.length;

		let dirty = false;

		for (let i = 0; i < lines.length; i++) {
			if ( !lines[i].startsWith("#") && lines[i].length < 100 ) {
				let p = lines[i].split("m");
				if (map[p[0]] !== undefined) {
					map[p[0]] += parseInt(p[1]);
					lines[i] = "#" + lines[i].substring(1);
					dirty = true;
				}
			}
		}

		if (dirty) {
			let writeBuffer = lines.join("\n");
			fs.writeSync(file, writeBuffer, position, "UTF-8");
		}

		position += numRead;

		buffer = Buffer.alloc(size);
		numRead = fs.readSync(file, buffer, 0, size, position);
	}

}


