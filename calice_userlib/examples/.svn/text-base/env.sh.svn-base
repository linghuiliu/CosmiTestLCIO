#Setting of environment variables
#Modify as appropriate

#Setting LCIO environment
#export LCIO=/data/poeschl/lcsoft/lcio/lcio
#export LCIO=/afs/desy.de/group/it/ilcsoft/lcio/v01-05

#modify the following pathes as needed
#export MARLIN=/afs/desy.de/group/it/ilcsoft/marlin/v00-09
#export MARLIN=/afs/desy.de/group/it/ilcsoft/marlin/v00-09-02
export MARLIN=/scratch/ilcsoft/marlin/v00-09-05/Marlin
#Quick and dirty check whether your marlin version has compiled in
#LCCD and CONDDB
marlinok=0
marlinok=`less $MARLIN/bin/marlin_includes.sh | grep USE_LCCD | grep USE_CONDDB | wc -l | awk '{print \$1}'`

if [ $marlinok -eq 0 ]
then
echo "Your marlin version $MARLIN has no LCCD included"
echo "Please provide a proper version, otherwise the code will not compile"
fi


#export MARLIN_USE_AIDA=0
#export JDK_HOME=/opt/products/java/1.4.2

#export JAIDA_HOME=/opt/products/JAIDA/3.2.3

#export AIDAJNI_HOME=/opt/products/AIDAJNI/3.2.3

#. $JAIDA_HOME/bin/aida-setup.sh
#. $AIDAJNI_HOME/bin/Linux-g++/aidajni-setup.sh


#Setting LCCD environment
#export LCCD=/data/poeschl/lcsoft/lccd
#export LCCD=/afs/desy.de/group/it/ilcsoft/lccd/v00-03
export MYSQL_PATH=/usr/


#Setting CondDB environment
#export CondDBMySQL=/data/poeschl/conddb/CondDBMySQL
#export CondDBMySQL=/data/poeschl/CondDBMySQL
export CondDBMySQL=/scratch/condb_co/CondDBMySQL
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CondDBMySQL/lib

#Setting USER environment
export CALUSER=/scratch/ilcsoft/calice/calice_userlib/
#export USER=/data/poeschl/hcal/data/goetz/new_prop_1/userlib/
#export USER=/flc/flcl01/data/poeschl/hcal/userlib/

#Invoke ROOT (if desired)
unset USE_ROOT
#export USE_ROOT=1
#Need to give a ROOTSYS (if not set already)
#export ROOTSYS=/opt/products/root/4.00.08/

#allow programs to read from dCache pool
#DESY internal only!!!
#export LD_PRELOAD=/opt/products/lib/libpdcap.so
