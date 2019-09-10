#!/usr/local/bin/node

const fs = require('fs');

// åpne første fil i folderen (antar at det er mange filer som skal kombineres, multipleres og reduseres)
// les 1000 første unike linjer/brett
//  alt må skje sync
//  fortsett å lese linjer, og sammenlikn med innleste
//  hvis det er match, kombiner&pultipliser, marker linje som brukt (med # feks på første char)
//  fortsett å les til alle linjer i alle filer er lest
//  skriv alle kombinerte linjer i en ny fil
// les nye 1000 (ubrukte) linjer, og repeter


