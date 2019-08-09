#!/usr/local/bin/node


var lineReader = require('readline').createInterface({
    input: require('fs').createReadStream('./outfile-sorted-counted-sorted.txt')
});

//const { execSync } = require('child_process');
const fs = require('fs');

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
    let data = "./chessengine -cfen \""+l[1]+"\" -maxlevel 6 -logtype binary -outfile results/level-6/level-6-mul-" + mul + ".txt ; sort results/level-6/level-6-mul-" + mul + ".txt -o results/level-6/level-6-mul-" + mul + "_sorted.txt\n";
    fs.writeFileSync( "workunits/workunits_" + l[0]+".sh", Buffer.from( data, "utf-8"), {flag:'a'} );
    // execSync("./chessengine -cfen \""+l[1]+" w 6\" -maxlevel 7 -logtype binary >> results/level-7/level-7-mul-" + l[0] + ".txt ");
    lineReader.resume();
});

lineReader.on('close', function(a,b){
    for( var t=0;t<480;t++){
        if( count[t] !== 0 ) {
            console.log(count[t]*t,t, count[t]);
        }
    }
});



