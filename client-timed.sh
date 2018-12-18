#!/bin/bash
echo "Starting Borgsum Chessengine Workunit Client";

while :; do
 currenttime=$(date +%H:%M)
 dayofweek=$(date +%u)
 if ([[ "$currenttime" > "08:00" ]] && [[ "$currenttime" < "17:00" ]]) && ( [[ "$dayofweek" != "6" ]] && [[ "$dayofweek" != "7" ]] ); then
   echo "keep waiting"
   sleep 60
 else
   echo `date`
   echo "done waiting, doning calculation"
   curl -s https://www.andreasoverland.no/chessengine/workunits/postresult/ -d "result=`curl -s https://www.andreasoverland.no/chessengine/workunits/ |xargs ./chessengine`"
 fi

done
