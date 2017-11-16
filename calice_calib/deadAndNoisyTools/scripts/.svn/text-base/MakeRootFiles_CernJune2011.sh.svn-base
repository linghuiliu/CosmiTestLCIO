#!/bin/bash


runs=$*

echo "------------------------------------------------------------------------------------------------------"
echo "if the AHC.cfg has been modified, this script has to be modified. it uses the AHC.cfg for the June 2011 run." 
echo "------------------------------------------------------------------------------------------------------"

#cd /scratch/data/cguenter/my_calice_daq_install
export BASEDIR=$PWD
export PATH=$PATH:/home/caliceon/localsoft/deadAndNoisyChannels/workdir/bin/

O_FLAG="";
FILE_NAME_TAG=""

for run in $runs;do
    # for simplicity this batch script takes 
    # the flag and argument as one word ( -o0 without space)
    # while ahcBinHst takes a space
    if [ "$run" == "-o0" ] 
    then 
	O_FLAG="-o 0"
	#leave the tag empty, this is consistent with a normal PMNoise run
	#FILE_NAME_TAG="_o0"
	continue
    fi

    if [ "$run" == "-o1" ] 
    then 
	O_FLAG="-o 1"
	FILE_NAME_TAG="_o1"
	continue
    fi


    cd $BASEDIR

echo " -----------------------------"
echo " processing next CRC at slot 9"
echo " -----------------------------"

    ahcBinHst $O_FLAG -s  9 -f 11111111 $run
    mv allChannels.root $BASEDIR/data/temp/$run"_"1.root

echo " ------------------------------"    
echo " processing next CRC at slot 12"
echo " ------------------------------"

    ahcBinHst $O_FLAG -s 12 -f 00100111 $run
    mv allChannels.root $BASEDIR/data/temp/$run"_"2.root

echo " ------------------------------"    
echo " processing next CRC at slot 15"
echo " ------------------------------"

    ahcBinHst $O_FLAG -s 15 -f 00111011 $run
    #ahcBinHst_cnf1 -s 15 -f 00111010 $run
    mv allChannels.root $BASEDIR/data/temp/$run"_"3.root

echo " ------------------------------"    
echo " processing next CRC at slot 17"
echo " ------------------------------"

    ahcBinHst $O_FLAG -s 17 -f 11101111 $run
    mv allChannels.root $BASEDIR/data/temp/$run"_"4.root

echo " ------------------------------"    
echo " processing next CRC at slot 19"
echo " ------------------------------"

    ahcBinHst $O_FLAG -s 19 -f 11110111 $run
    mv allChannels.root $BASEDIR/data/temp/$run"_"5.root

echo " ------------------------------"    
echo " processing next CRC at slot 7"
echo " ------------------------------"

    ahcBinHst $O_FLAG -s 7 -f 01111111 $run
    mv allChannels.root $BASEDIR/data/temp/$run"_"6.root

echo " ------------------------"    
echo " processed all CRC boards"
echo " ------------------------"

    cd $BASEDIR/data/temp
    
echo " ------------------"    
echo " merging root files"
echo " ------------------"

    hadd $run$FILE_NAME_TAG.root $run"_"1.root $run"_"2.root $run"_"3.root $run"_"4.root $run"_"5.root $run"_"6.root

echo " -----------"    
echo " cleaning up"
echo " -----------"

    mv  $run$FILE_NAME_TAG.root ../${run}$FILE_NAME_TAG.root
    rm $run"_"1.root
    rm $run"_"2.root
    rm $run"_"3.root
    rm $run"_"4.root
    rm $run"_"5.root
    rm $run"_"6.root

cd $BASEDIR

done