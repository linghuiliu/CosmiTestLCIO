#!/bin/zsh

source /afs/desy.de/group/flc/bin/setflcini
flcini calice_pro_test

executable="relocateCollection"


#HcalModuleLocationReference
$executable -i $CALICE_DB_INIT -r /cd_calice_fnalbeam/Hcal/HcalModuleLocationReference -a 2009-05-01-00-00-00 -l /cd_calice_desylab/Hcal/HcalModuleLocationReference -f 2010-01-01-00-00-00 -u 2010-08-31-23-59-59.999999999999999;

##SroModMapping
#$executable -i $CALICE_DB_INIT -r /cd_calice_fnalbeam/Hcal/AhcSroModMapping -a 2009-05-01-00-00-00 -l /cd_calice_desylab/Hcal/AhcSroModMapping -f 2010-01-01-00-00-00 -u 2010-08-31-23-59-59.999999999999999;


##TempSensorCalib
#$executable -i $CALICE_DB_INIT -r /cd_calice/Hcal/tempSensors -a 2009-05-01-00-00-00 -l /cd_calice/Hcal/tempSensors -f 2010-09-01-00-00-00 -u 2015-12-31-23-59-59.999999999999999;



##TriggerCheck
#$executable -i $CALICE_DB_INIT -r /cd_calice_fnalbeam/CALDAQ_TriggerCheck -a 2009-05-01-00-00-00 -l /cd_calice_desylab/CALDAQ_TriggerCheck -f 2010-01-01-00-00-00 -u 2010-08-31-23-59-59.999999999999999;


#HcalModuleDescription
#$executable -i $CALICE_DB_INIT -r /cd_calice_fnalbeam/Hcal/HcalModuleDescription -a 2009-05-01-00-00-00 -l /cd_calice_desylab/Hcal/HcalModuleDescription -f 2010-01-01-00-00-00 -u 2010-08-31-23-59-59.999999999999999;


#IC
#$executable -i $CALICE_DB_INIT -r /cd_calice/Hcal/ic_constants -a 2009-01-01-00-00-00 -l /cd_calice/Hcal/ic_constants -f 2010-01-01-00-00-00 -u 2015-12-31-23-59-59.999999999999999;

#gain
#$executable -i $CALICE_DB_INIT -r /cd_calice/Hcal/gain_constants -a 2009-01-01-00-00-00 -l /cd_calice/Hcal/gain_constants -f 2010-01-01-00-00-00 -u 2015-12-31-23-59-59.999999999999999;
#
#$executable -i $CALICE_DB_INIT -r /cd_calice/Hcal/gain_slopes -a 2009-01-01-00-00-00 -l /cd_calice/Hcal/gain_slopes -f 2010-01-01-00-00-00 -u 2015-12-31-23-59-59.999999999999999;

#mip
#$executable -i $CALICE_DB_INIT -r /cd_calice/Hcal/mip_constants -a 2009-01-01-00-00-00 -l /cd_calice/Hcal/mip_constants -f 2010-01-01-00-00-00 -u 2015-12-31-23-59-59.999999999999999;

#$executable -i $CALICE_DB_INIT -r /cd_calice/Hcal/mip_slopes -a 2009-01-01-00-00-00 -l /cd_calice/Hcal/mip_slopes -f 2010-01-01-00-00-00 -u 2015-12-31-23-59-59.999999999999999;



#trigger
#install/bin/relocateCollection -i flccaldb02.desy.de:calice:calicedb:gFd+5Thn:3306 -r /cd_calice_fnalbeam/CALDAQ_TriggerAssignment -a 2008-04-24-05-00-00 -l /test_nf/Hcal/test8 -k
#install/bin/relocateCollection -i flccaldb02.desy.de:calice:calicesu:as3.Rgvd:3306 -r /cd_calice_fnalbeam/CALDAQ_TriggerAssignment -a 2008-04-24-05-00-00 -l /test_nf/Hcal/test8 -k


#HCAL detector position and mapping

#install/bin/relocateCollection -i flccaldb02.desy.de:calice:calicedb:bh7+4FUw:3306 -r /cd_calice_fnalbeam/Hcal/DetectorPosition -a 2008-05-15-00-00-00 -l /cd_calice_fnalbeam/Hcal/DetectorPosition -f 2009-01-01-00-00-00 -u 2009-12-31-23-59-59;

#install/bin/relocateCollection -i flccaldb02.desy.de:calice:calicedb:bh7+4FUw:3306 -r /cd_calice_fnalbeam/Hcal/HcalModuleLocation -a 2009-09-15-00-00-00 -l /test_nf/Hcal/HcalModuleLocation -f 2008-01-01-00-00-00 -u 2009-12-31-23-59-59;


#TCMT
#install/bin/relocateCollection -i flccaldb02.desy.de:calice:calicedb:bh7+4FUw:3306 -r /test_nf/Tcmt/Mip00 -a 2008-09-15-00-00-00 -l /test_nf/Tcmt/Mip00 -f 2009-01-01-00-00-00 -u 2009-12-31-23-59-59;
#
#install/bin/relocateCollection -i flccaldb02.desy.de:calice:calicedb:bh7+4FUw:3306 -r /test_nf/Tcmt/TcmtModuleConnection -a 2008-09-15-00-00-00 -l /test_nf/Tcmt/TcmtModuleConnection -f 2009-01-01-00-00-00 -u 2009-12-31-23-59-59;
#
#install/bin/relocateCollection -i flccaldb02.desy.de:calice:calicedb:bh7+4FUw:3306 -r /test_nf/Tcmt/TcmtModuleLocation -a 2008-09-15-00-00-00 -l /test_nf/Tcmt/TcmtModuleLocation -f 2009-01-01-00-00-00 -u 2009-12-31-23-59-59;
#
#install/bin/relocateCollection -i flccaldb02.desy.de:calice:calicedb:bh7+4FUw:3306 -r /test_nf/Tcmt/TcmtModuleDescription -a 2008-09-15-00-00-00 -l /test_nf/Tcmt/TcmtModuleDescription -f 2009-01-01-00-00-00 -u 2009-12-31-23-59-59;
#
#install/bin/relocateCollection -i flccaldb02.desy.de:calice:calicedb:bh7+4FUw:3306 -r /test_nf/Tcmt/TcmtDetectorPosition -a 2008-09-15-00-00-00 -l /test_nf/Tcmt/TcmtDetectorPosition -f 2009-01-01-00-00-00 -u 2009-12-31-23-59-59;
#




