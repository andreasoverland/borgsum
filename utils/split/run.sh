#!/bin/zsh

echo `date` 

split -b66000000 -a5 combined
rm combined
ls -1 x*|xargs -n 1 -I x mv x x.nbin
../combiner5.js
rm *.nbin
ls -1 x*|sort -R|xargs cat > combined
rm x*
split -b40722 -a6 combined
rm combined
ls -1 x*|sort -R|xargs cat > combined
rm x*
echo `ls -l combined` 
