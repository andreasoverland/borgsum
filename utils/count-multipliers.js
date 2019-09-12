#!/usr/local/bin/node


var lineReader = require('readline').createInterface({
	input: require('fs').createReadStream( "combined.txt" )
});

let numDifferentBoards = 0;
let numBoards = 0;

lineReader.on('line', function (line) {
	if( line.trim() === "" ){
		return;
	}

	let l = line.trim().split("m");
	let multiplier = parseInt(l[1]);
	numDifferentBoards++;
	numBoards+= multiplier;

});

lineReader.on('close', ()=>{

	console.log( "Num unique boards :",numDifferentBoards);
	console.log( "Num boards        :",numBoards);

});
