
# Workflow

## Lag en output-fil

## Sorte resultatene
mkdir sorted

### genrerer sorerings-kommandoer
ls -1 *out*.txt | awk '{printf("sort %s -o sorted/%s ;\n",$1,$1);}END{}'

### kjør sorterings-kommandoer
..
cd sorted

### merge sortert resultat
sort -m *.txt -o merged.txt


### count occurances of same bitboards
uniq -c merged.txt counted.txt

### combine counts with multipliers
cat counted.txt | sed $'s/m/\ /g'| awk '{printf("%sm%i\n",$2,$1*$3)};END{}' > multiplied.txt
 ~/Projects/Personal/borgsum/utils/count-multipliers.js

### reduce duplicates

The results are now ready to be used as input

### prepare inputs

Split in managable sizes

## make runs
mkdir level_NEXT
ls -1 level_7_wu_* | awk '{printf(" ~/Projects/Personal/ChessEngine/borgsum/chessengine -logtype cfen -infile %s -maxlevel 8 -outfile level_8/%s_to_8_out.txt; \n",$1,$1);}END{}' > run_level_8.sh

## run
