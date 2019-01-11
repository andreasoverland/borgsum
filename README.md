# Borgsum Chess Engine
### by Andreas Øverland - andreasoverland@gmail.com


Super Simple Chess Engine

- Status : Working on finding moves as fast as possible. No evaluation yet. Single thread for simplicity.
- What : Will currently do a depth 5 search on Kiwi Pete (without doing an extra level for checks on level 5 to test for mate)
- How to compile : make
- How to run : ./chessengine



cat testbatch.txt|xargs -n 4 sh -c './chessengine "\"$1\"" $2 $3 2 `md5 -qs "$1"` > res/`md5 -qs "$1"`_$0.txt'  
