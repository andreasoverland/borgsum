#!/bin/bash


echo "Starting Borgsum Chessengine Workunit Client";

while true
do
    curl -s https://www.andreasoverland.no/chessengine/workunits/postresult/ -d "result=`curl -s https://www.andreasoverland.no/chessengine/workunits/ |xargs ./chessengine`"
done
