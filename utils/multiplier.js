#!/usr/local/bin/node

const fs = require('fs');

let files = fs.readdirSync(".");

doEachFile( files[0] );

let buffer = "";
function doEachFile( fileName ){

    if( fileName.endsWith(".txt") ){
    	let p = fileName.substring(0,fileName.indexOf(".txt")).split("_");

    	let multiplier = parseInt(p[p.length-1]);

    	console.log( fileName + " mul:"  + multiplier );

		var lineReader = require('readline').createInterface({
			input: require('fs').createReadStream( fileName )
		});

		lineReader.on('line', function (line) {
			if( line.trim() === "" ){
				return;
			}
			let l = line.trim().split(" ");
			let mul = parseInt(l[0]);
			buffer += l[1]+" " + mul*multiplier + "\n";
			if( buffer.length > 1000000 ) {
				fs.writeFileSync("multiplied/" + fileName, Buffer.from(buffer, "utf-8"), {flag: 'a'});
				buffer = "";
			}
		});

		lineReader.on('close', ()=>{

			if( buffer.length > 0 ) {
				fs.writeFileSync("multiplied/" + fileName, Buffer.from(buffer, "utf-8"), {flag: 'a'});
				buffer = "";
			}

			files.shift();
			if(files.length > 0){
				doEachFile(files[0]);
			}
		});

	}
    else {
		files.shift();
		if(files.length > 0){
			doEachFile(files[0]);
		}
	}

}


