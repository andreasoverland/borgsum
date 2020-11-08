#!/usr/local/bin/node

let buffer = Buffer.alloc(20000000);

buffer.writeBigUInt64LE(1213141516171819n,10000000);

let p1 = buffer.readUInt8( 10000000 );

console.log( "|"+p1.toString(36) );

let map = {};

map[10] = "hehe";
console.log( typeof Object.keys(  map )[0] );