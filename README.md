# Borgsum Chess Engine
### by Andreas Øverland - andreasoverland@gmail.com


Super Simple Chess Engine

- Status : Working on finding moves as fast as possible. No evaluation yet. Single thread for simplicity.
- What : Will currently do a depth 5 search on Kiwi Pete (without doing an extra level for checks on level 5 to test for mate)
- How to compile : make
- How to run : ./chessengine

to make diagrams for input
cat testbatch.txt|xargs -n 5  sh -c 'echo $0 $1 $2 $4'  
