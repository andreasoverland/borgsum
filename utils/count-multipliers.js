#!/usr/local/bin/node
const fs = require('fs');
let size = 6400000;
let position = 0;
let file = fs.openSync("level7.bin", "r");
let buff = Buffer.alloc(size);
let numRead = fs.readSync(file, buff, 0, size, position);

let count = 0n;
let uniqueCount = 0n;
while( numRead > 0 ){
	position += numRead;
	for( let i = 0;i<numRead/64;i++){
		uniqueCount++;
		count += buff.readBigUInt64LE(i*64+56);

	}
	numRead = fs.readSync(file, buff, 0, size, position);
}

console.log( "unique:", uniqueCount );
console.log( "totel count:", count );


fs.closeSync(file);
