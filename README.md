# Borgsum Chess Engine
### by Andreas Øverland - andreasoverland@gmail.com



Super Simple Chess Engine

- Status : Working on finding moves as fast as possible. No evaluation yet. Single thread for simplicity.
- What : Will currently do a depth 5 search on Kiwi Pete (without doing an extra level for checks on level 5 to test for mate)
- How to compile : make
- How to run : ./chessengine

Preparing workbatches

1. ./chessengine -maxlevel 5 -logtype cfen -outfile results/level_5.cfen
2. sort results/level_5.cfen -o results/level_5_sorted.cfen
3. uniq -c results/level_5_sorted.cfen > results/level_5_sorted_counted.cfen
4. cat results/level_5_sorted_counted.cfen | sed $'s/m/\ /g'| awk '{printf("%sm%i\n",$2,$1*$3)};END{}' > results/level_5_sorted_counted_multiplied.cfen


tail level_6_mul_9.txt.sorted | awk '{printf("%s %s\n",$2,$1);}END{}'

cat level-5-unique-sorted.txt |awk '{printf("./chessengine -maxlevel 6 -logtype cfen >> results/level_6/level_6_%s.txt\n",$2,$1);}END{}' > run_level_6.sh

sort --random-sort ../level_3_results.txt -o ../level_3_results_rnd.txt

// split opp resultat
split -l1113  ../level_3_results_rnd.txt level_3_

// generer kjørings
ls -1 level_5_* | awk '{printf("../chessengine -infile %s -maxlevel 6 > level6/%s_to_6_out.txt &\n",$1,$1);}END{}' > run_level_6.sh

### NÅR
- lag sortering av hver fil
ls -1 level_6_mul_*.txt | awk '{printf("sort %s > sorted/%s ; ",$1,$1);}END{}'
- og kjør generert sortering

- merge resultat
sort -m *.txt > level_6_sorted.txt

-. Sorterresultat output
ls -1 | awk '{printf("sort %s -o %s.sorted &;\n",$1,$1);}END{}'

- Sorter og tell
cat level_6_sorted.txt| uniq -c > level_6_sorted_counted.txt

- split opp cfen slik at m blir erstattet med space, for å kunne multiplisere
cat level_6_sorted_counted.txt | sed $'s/m/\ /g'| awk '{printf("%sm%i\n",$2,$1*$3)};END{}' > level_6_multiplied.txt

- Kjør combiner
 ~/Projects/Personal/ChessEngine/borgsum/workunit-preparator/count-multipliers.js

for f in *.txt; do
sort $f -o $f.sorted
done


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

### 2019.08.02 12:45 - Testing binary data output
- test reorganizing comparale piece data for fast output. må mekke fen med brikke + rle i en byte

### 2019.08.06 20:50 - Binary logging works. Now for proper duplication removal and next level calculations.
- find a way to count the duplicates from different files
 - todo :
   - make cfen -> bitboard
   - bring in the multiplier count for each result
   - make it possile to run a large amount of runs



### 2019.09.07 - todo
- MÅL: lag et redusert workset på level 7 for å nå level 10 på default setup
- lage outputfile med counters, for å dele opp filene i mindre biter
- utfordring er å ta duplikater fra feks level_6_mul_4.txt og level_6_mul_5.txt og slå de sammen i neste workunit


### 2019.09.24 - hexdump trick

 ls -1|xargs -n 1 -I x mv x x.nbin

For cbin
hexdump -e '8/1 "%02X" " " 8/1 "%02X" " " 8/1 "%02X" " " 8/1 "%02X" " " 1/1 "%02X" " " 1/1 "%02X" " " 1/1 "%02X" " " 1/1 "%02X" " " 1/1 "%02X"  "\n" '  data.cbin

For nbin

hexdump -e '8/1 "%02X" " " 8/1 "%02X" " " 8/1 "%02X" " " 1/1 "%02X" " " 8/1 "%02X" "\n"'  level1.nbin


### 2020.09.12 - try to rewrite engine to use stack + non-recursive
