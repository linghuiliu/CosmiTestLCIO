runnumber=$1

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

cat mip_single/Run${runnumber}clearFit.txt | grep -v \# | grep -v Module > ${filteredfile}


# set DB variables
parameterstring="int: runnumber $runnumber"

tmpfilename=`mktemp`

echo $parameterstring > $tmpfilename

DBINIT=$CALICE_DB_INIT

FOLDER=/cd_calice/Hcal/Calib_Measurements/mip_single

COLLECTION_NAME="mip_single"

DESCRIPTION="result of mip run $runnumber"

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
