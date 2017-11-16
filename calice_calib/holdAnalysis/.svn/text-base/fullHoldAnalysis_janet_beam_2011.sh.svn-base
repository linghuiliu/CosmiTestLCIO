#!/bin/bash
# this macro can be used to do a beam hold scan- 
#The order of the complete hold scan analysis is the following:  
# - produce from binary files text files using the class ahcBinHold.cc 
# - plot hold scans and do the fits using the class holdMultiModule_2011_beam.c
# - compare old with new results using compare_holds.c
  
#All you need to do is to run the script!!!  
#start with ./fullHoldAnalysis_janet_beam_2011.sh runnumber  
#  


WORKDIR=/home/caliceon/DAQ/daquser
HOLDSCANDIR=$WORKDIR/hold_analysis

usage () {
    echo "Usage:"
    echo "$0 <runNumber>"
}

if [ $# -lt 1 ]; then
    usage
    exit 1
fi

if [ "$1" = "-h" ]; then
    usage
    exit 0
fi

STARTDIR=`pwd` 
RUNNUMBER=$1

# only works in work dir
cd $WORKDIR
echo "I am in directory:"
echo $STARTDIR
ahcBinHold $RUNNUMBER
echo "I am in directory:"
STARTDIR=`pwd`
echo $STARTDIR
for FILE in hold_module*.dat; do
    if [ -f $FILE ]; then
	mv $WORKDIR/$FILE $HOLDSCANDIR/
	echo "copy file"
	echo $FILE
    fi
done 
cd $HOLDSCANDIR/

for FILE in hold_module*.dat; do
    if [ -f $FILE ]; then

       MODULE=`echo $FILE | sed 's/hold_module//'`
       MODULE=`echo $MODULE | sed 's/.dat//'`
       echo "Analysing hold scans for module $MODULE ..."
       echo "in directory:"
       echo $STARTDIR
       echo $MODULE
       root -q -b -L "holdMultiModule_2011_beam.c($MODULE)" >> fullHoldLog.txt
	
    fi
done


mkdir $RUNNUMBER
for FILE in hold_module*.dat; do
    if [ -f $FILE ]; then
	mv $FILE $RUNNUMBER/
	echo "copy file"
 	echo $FILE
    fi
done
mv *.pdf $RUNNUMBER/
mv *.root $RUNNUMBER/
mv *.gif $RUNNUMBER/
mv *.ps $RUNNUMBER/


cp summary.txt $RUNNUMBER/
cp holdfit_module*.dat $RUNNUMBER/
cp compare_holds.c $RUNNUMBER/
cd $RUNNUMBER/
root  -L compare_holds.c
