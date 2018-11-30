#!/usr/local/bin/node

const fs = require("fs");
const exec = require('child_process').exec;

let workunits = [];

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

      workunits = Buffer.from(wres).toString("UTF-8").split("\n");

      workunits.shuffle();

      console.log( workunits );

      for( let numthreads = 0; numthreads < settings.numthreads ; numthreads++ ){
          calculateNextUnit( settings );
      }

  })


});


function calculateNextUnit( settings ){
    let workunit = workunits.shift();
    console.log( new Date() + " Starting workunit " + workunit + " Remaining units " + workunits.length );
    os.execCommand("./chessengine " + workunit + " " + settings.depth, result => {
        // console.log( result );
        if( workunits.length > 0 ){
            calculateNextUnit(settings);
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