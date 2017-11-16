runnumberPM=$1
runnumberCM=$2
runnumber=$runnumberPM

# get runtime
start=-999
stop=-999

runtimetmp=`mktemp`

runTime $runnumber > $runtimetmp

if [ ! -f $runtimetmp ]; then exit 1
fi

grep "start" $runtimetmp || exit 1

start=`grep start $runtimetmp | awk '{print $4}'`
stop=`grep stop $runtimetmp   | awk '{print $5}'`

rm $runtimetmp

# filter values

filteredfile=`mktemp`

cat ic_single/ic-${runnumberPM}-${runnumberCM}.dat | grep -v \# | grep -v "0$" > ${filteredfile}


# set DB variables
parameterstring1="int: runnumber $runnumber"
parameterstring2="int: runnumber_pm $runnumberPM"
parameterstring3="int: runnumber_cm $runnumberCM"

tmpfilename=`mktemp`

echo $parameterstring1 > $tmpfilename
echo $parameterstring2 >> $tmpfilename
echo $parameterstring3 >> $tmpfilename

DBINIT=$CALICE_DB_INIT

FOLDER=/cd_calice/Hcal/Calib_Measurements/ic_single

COLLECTION_NAME="ic_single"

DESCRIPTION="intercalibration from PM run $runnumberPM and CM run $runnumberCM"

echo "file: $filteredfile
      start: $start
      stop: $stop
      parameterstring: $parameterstring"

########################### push


writeSimpleValues --inFile $filteredfile --dbinit $DBINIT \
		  --from $start --until $stop \
		  --isTimeStamp "true" \
		  --parameterFile $tmpfilename \
		  --colName $COLLECTION_NAME \
		  --folder $FOLDER \
		  --description "$DESCRIPTION" \
                  --write
rm $filteredfile
rm $tmpfilename
