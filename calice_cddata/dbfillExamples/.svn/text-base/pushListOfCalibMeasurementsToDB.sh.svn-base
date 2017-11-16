#!/bin/zsh

# mip / gain

#runList=( $(ls mip_single | cut -d n -f 2 | cut -d c -f 1) )
#runList=( $( ls -l mip_single/*clearFit.txt | awk '{ if ($7 > 18) print $9}' | cut -d\/ -f2 | cut -d n -f 2 | cut -d c -f 1 ) )
#
#for run in $runList; do
#    pushMIPtoDB.sh $run
#done;


# ic

fileList=( $( ls -l ic_single/*.dat | awk '{ if (NF > 1) print $9 }' ) )

for file in $fileList; do

    echo $file

    runPM=$( echo $file | cut -d\/ -f2 | cut -d. -f 1 | cut -d\- -f 2 )
    runCM=$( echo $file | cut -d\/ -f2 | cut -d. -f 1 | cut -d\- -f 3 )

    pushSingleICToDB.sh $runPM $runCM
done;
