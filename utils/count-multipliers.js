#!/usr/local/bin/node

const fs = require('fs');


var lineReader = require('readline').createInterface({
	input: require('fs').createReadStream( "multiplied.txt" )
});

fs.writeFileSync("combined.txt", Buffer.from("", "UTF-8"));

let numDifferentBoards = 0;
let numBoards = 0;
let lastLine = "";
let lastMultiplier = 0;
let buffer = "";
let duplicates = 0;

lineReader.on('line', function (line) {
	if( line.trim() === "" ){
		return;
	}

	let l = line.trim().split("m");
	let multiplier = parseInt(l[1]);
	if( l[0] === lastLine ){
		lastMultiplier += multiplier;
		duplicates++;
	}
	else {
		if( lastLine !== "" ) {
			buffer += lastLine + "m" + lastMultiplier + "\n";
			numDifferentBoards++;
			numBoards+= lastMultiplier;
			if (buffer.length > 1000000) {
				fs.writeFileSync("combined.txt", Buffer.from(buffer, "utf-8"), {flag: 'a'});
				buffer = "";
			}
		}
		lastLine = l[0];
		lastMultiplier = multiplier;
	}

});

lineReader.on('close', ()=>{
	if( buffer.length > 0 ){
		buffer += lastLine + "m" + lastMultiplier + "\n";
		numDifferentBoards++;
		numBoards+= lastMultiplier;
		fs.writeFileSync("combined.txt", Buffer.from(buffer, "utf-8"), {flag: 'a'});
	}
	console.log( "Num unique boards :",numDifferentBoards);
	console.log( "Num boards        :",numBoards);
	console.log( "Duplicates found  :",duplicates);

});
