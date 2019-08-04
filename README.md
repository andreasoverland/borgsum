# Borgsum Chess Engine
### by Andreas Øverland - andreasoverland@gmail.com



Super Simple Chess Engine

- Status : Working on finding moves as fast as possible. No evaluation yet. Single thread for simplicity.
- What : Will currently do a depth 5 search on Kiwi Pete (without doing an extra level for checks on level 5 to test for mate)
- How to compile : make
- How to run : ./chessengine

Preparing workbatches

./chessengine -diagram "r...k..r p.ppqpb. bn..pnp. ...PN... .p..P... ..N..Q.p PPPBBPPP R...K..R w KQkq - 0" -maxlevel 3 -workunitid u723bd823d -logtype diagram|grep "%"|sort|uniq -c|sort > workunit.txt
cat workunit.txt |awk '{printf("%s %s %s %s %s %s %s %s %s %s %s %s -mul %s\n",$3,$4,$5,$6,$7,$8,$9,$10,$11,$12,$13,$14,$1)}END{print}'

./chessengine -diagram "rnbqkbnr pppppppp ........ ........ ........ ........ PPPPPPPP RNBQKBNR w KQkq - 0" -maxlevel 7 -logtype diagram|grep "%" > level-7-all.txt


#### 2019.07.16 09:45 - Status
The fastest version is now in the branch bitboards, but the most versatile client is in master.
Plan : merge the client-parts from master, into bitboards. then move set bitboards as master.

### 2019.07.16 12:40 - Status
The branch bitboards is now merged into master, including the nice client features from master.

### 2019.07.17 10:00 - Status
The master branch is ok now. But I would like the input arguments for the client to be easier to use, also to add a "log=true/false" option.
Plan:
- argument for board should be both board, turn , castling, en passant square and move num : done
		  
- add argument names
  - diagram : done
  - fen
  - maxlevel : done
  - workunitid : done
  - log = fen/diagram (if omitted, only statistics are logged at the end) : done
 

### 2019.07.17 10:00 - Status all of the above done.


Todo
- CSV output format for statistics
- Parse FEN
- More parse friendly output of logging

### 2019.01.02 12:45 - Testing binary data output
- test reorganizing comparale piece data for fast output. må mekke fen med brikke + rle i en byte




