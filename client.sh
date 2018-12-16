#!/bin/bash


echo "Starting Borgsum Chessengine Workunit Client";

while true
do
    curl -s http://localhost:4030/postresult/ -d "result=`curl -s localhost:4030 |xargs ./chessengine`"
done
