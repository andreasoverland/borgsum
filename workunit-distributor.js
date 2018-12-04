#!/usr/local/bin/node

const fs = require("fs");
const exec = require('child_process').exec;

let workunits = [];
let numberOfWorkunits = 0;

let startTime = new Date().getTime();

fs.readFile("settings.txt", (err,res) => {

  let contents = Buffer.from(res).toString("UTF-8");
  let lines = contents.split("\n");

  let settings = {};
  lines.forEach( l => {
      let parts = l.split("=");
      if( parts.length === 2  ){
          settings[parts[0]] = parts[1];
      }
  } );

  console.log( settings );

  fs.readFile( settings.workunits , (werr,wres) => {

      workunits = Buffer.from(wres).toString("UTF-8").trim().split("\n");

      workunits.shuffle();
      numberOfWorkunits = workunits.length;

      for( let numthreads = 0; numthreads < settings.numthreads ; numthreads++ ){

          calculateNextUnit( settings );
      }

  })


});

let counts = {};// level {} -> Metric:N
let statsReported = false;
let numFinished = 0;

function calculateNextUnit( settings ){
    let workunit = workunits.shift();
    console.log( new Date() + " Starting workunit " + workunit + " Remaining units " + workunits.length );
    os.execCommand("./chessengine " + workunit + " " + settings.depth, result => {

        numFinished++;

        result = result.substring( result.indexOf("Depth"));
        result = result.substring( 0, result.indexOf("Total"));

        result = result.replace(/ /g, '');


        // TODO: accumulate into counts
        lines = result.split("\n");
        lines.forEach( ll => {
           if( ll.indexOf("Depth") < 0 && ll.length > 2) {
                let l = ll.split("\t");
                let d = l[0];
                if( counts[d] ){
                    let c = counts[d];
                    c.nodes += parseInt(l[1]);
                    c.caps += parseInt(l[2]);
                    c.ep += parseInt(l[3]);
                    c.castles += parseInt(l[4]);
                    c.promos += parseInt(l[5]);
                    c.checks += parseInt(l[6]);
                    c.mates += parseInt(l[7]);
                }
                else {
                    let c = {};
                    c.nodes = parseInt(l[1]);
                    c.caps = parseInt(l[2]);
                    c.ep = parseInt(l[3]);
                    c.castles = parseInt(l[4]);
                    c.promos = parseInt(l[5]);
                    c.checks = parseInt(l[6]);
                    c.mates = parseInt(l[7]);
                    counts[ d ] = c;
                }

           }


        });


        if( workunits.length > 0  ){
            calculateNextUnit(settings);
        }
        if(  numFinished === numberOfWorkunits ){
            console.log(counts);
            let endTime = new Date().getTime();
            console.log( "millis:", endTime-startTime);
        }
    });
}


function os_func() {
    this.execCommand = function(cmd, callback) {
        exec(cmd, (error, stdout, stderr) => {
            if (error) {
                console.error(`exec error: ${error}`);
                return;
            }

            callback(stdout);
        });
    }
}
var os = new os_func();

Array.prototype.shuffle = function() {
    var input = this;

    for (var i = input.length-1; i >=0; i--) {

        var randomIndex = Math.floor(Math.random()*(i+1));
        var itemAtIndex = input[randomIndex];

        input[randomIndex] = input[i];
        input[i] = itemAtIndex;
    }
    return input;
}