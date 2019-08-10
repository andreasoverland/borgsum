#!/usr/local/bin/node

const fs = require('fs');

var lineReader = require('readline').createInterface({
    input: require('fs').createReadStream('./level-6.txt')
});

fs.writeFileSync("run-level-7.sh","#!/bin/bash\n\n");



let count = [];
for( var t=0;t<480;t++){
    count.push(0);
}
let prevLine = "";
lineReader.on('line', function (line) {
    lineReader.pause();
    if( line.trim() === "" ){
        return;
    }

    let l = line.trim().split(" ");
    let mul = parseInt(l[0]);
    count[ mul ]++;
    let data = l[1]+"\n";
    fs.writeFileSync( "workunits/workunits_lvl6_mul_" + l[0]+".txt", Buffer.from( data, "utf-8"), {flag:'a'} );

    lineReader.resume();
});



lineReader.on('close', function(a,b){
    for( var t=0;t<480;t++){
        if( count[t] !== 0 ) {
            fs.writeFileSync( "run-level-7.sh", "./chessengine -infile workunits/workunits_lvl6_mul_"+t+".txt  -outfile results/level_7_mul_"+t+".txt -maxlevel 7 -logtype binary\n", {flag:'a'});
            console.log(t, count[t], t*count[t],);
        }
    }
});



