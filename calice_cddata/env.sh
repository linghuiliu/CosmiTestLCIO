#####################################################
#
# Set the environment for the calice dbfill routines - modify as needed
#
# F.Gaede, DESY
# 01-02-2005
#
#####################################################
export DEBUG=1

# -- required
export LCIO=/data/poeschl/ilcsoft/lcio/v01-08-02

#export LCIO=/data/poeschl/lcsoft/lcio/lcio

export LCCD=/data/poeschl/ilcsoft/lccd/v00-03-05

#-- comment out for production 
#export LCCDDEBUG=1

#-- optionally build with CondDBMySQL support
#export CondDBMySQL=/afs/desy.de/group/it/ilcsoft/CondDBMySQL
export CondDBMySQL=/data/poeschl/extern/CondDBMySQL
export MYSQL_PATH=/usr
export LD_LIBRARY_PATH=$CondDBMySQL/lib


#export MYSQL_UNIX_PORT=/tmp/mysql-3.23.58.sock
#export MYSQL_UNIX_PORT=/tmp/mysql.sock

#Setting USER environment
export CALUSER=/data/poeschl/ilcsoft/calice/testbed/calice_userlib

#The reconstruction outputs root histograms
export ROOTSYS=/scratch/opt/root
export PATH=$ROOTSYS/bin:$PATH
export LD_LIBRARY_PATH=$ROOTSYS/lib:$LD_LIBRARY_PATH
