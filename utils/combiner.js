#!/usr/local/bin/node

const fs = require('fs');
const readline = require('readline');
// åpne første fil i folderen (antar at det er mange filer som skal kombineres, multipleres og reduseres)
// les 1000 første unike linjer/brett
//  alt må skje sync
//  fortsett å lese linjer, og sammenlikn med innleste
//  hvis det er match, kombiner&pultipliser, marker linje som brukt (med # feks på første char)
//  fortsett å les til alle linjer i alle filer er lest
//  skriv alle kombinerte linjer i en ny fil
// les nye 1000 (ubrukte) linjer, og repeter



let masterLine = "";

let masterLineFileName = "xaa";

let lineReader = readline.createInterface({
	input: fs.createReadStream(masterLineFileName)
});

let bytesRead = 0;

lineReader.on('line', function (line) {
	bytesRead = masterLine.length+1;
	if( !line.startsWith("#") ) {
		masterLine = line;
		lineReader.pause();
		console.log( "Master line read : "+ masterLine +"\nPreparing for big scan");
		prepareScan();
	}
});

/*lineReader.on('close', ()=>{
	if( masterLines.length < numMasterLines ){
		prepareScan();
	}
});*/


let writePosition = 0;

function prepareScan(){


	let p = masterLine.split("m");

	if (map[p[0]] === undefined) {
		map[p[0]] = parseInt(p[1]);
	} else {
		map[p[0]] += parseInt(p[1]);
	}
	masterLine = "#" + masterLine.substring(1);

	console.log( "Ready for big scan with initial map:\n",map);

	let file = fs.openSync(masterLineFileName,"rs+");
	fs.writeSync(file, masterLine, writePosition, "UTF-8");
	writePosition += bytesRead;
	bytesRead = 0;

	fs.closeSync(file);

	scanAllFilesForMasterLines( ); // bigtime

	fs.writeFileSync("../combined.txt", Buffer.from( JSON.stringify(map), "utf-8"), {flag: 'a'});
	map = {};
	// lineReader.resume();
}

let numLinesReadAndChecked = 0;
let map = {};
let scanLineFiles = fs.readdirSync(".");

function scanAllFilesForMasterLines(){

	for( let i=0;i<scanLineFiles.length;i++){
		let file = fs.openSync(scanLineFiles[i],"rs+");
		scanAndMarkLinesInFile(file);
		fs.closeSync(file);
	}
	console.log( "Partial after round one.\n", map );
}




function scanAndMarkLinesInFile( file ) {
 	let size = 100000;

	let position = 0;

	let buffer = Buffer.alloc(size);
	let numRead = fs.readSync(file, buffer, 0, size, position);

	while (numRead > 0) {

		let lines = buffer.toString("utf-8").trim().split("\n");

		if (numRead === size) {
			lastLine = lines.pop();
			lengthOfLastLine = lastLine.length;
			numRead -= lengthOfLastLine; // for next read
		}

		numLinesReadAndChecked += lines.length;

		let dirty = false;

		for (let i = 0; i < lines.length; i++) {
			if (lines[i].startsWith("#")) {
				break;
			}
			let p = lines[i].split("m");
			if (map[p[0]] !== undefined) {
				map[p[0]] += parseInt(p[1]);
				lines[i] = "#" + lines[i].substring(1);
				dirty = true;
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


