#Setting of environment variables
#Modify as appropriate
#Only during debugging phase 
export DEBUG=1

#Setting Marlin environment
#export MARLIN=/afs/desy.de/group/it/ilcsoft/marlin/v00-09-02
#export MARLIN=/scratch/data/poeschl/lcsoft/Marlin
export MARLIN=/data/poeschl/ilcsoft/cmake/Marlin/
export MARLIN64=/exp/flc/poeschl/Marlin/SL4/x86_64/v00-09-07

#Setting the calice userlib environment
#The calice userlib to access the calice testbeam datatype
export CALUSER=/data/poeschl/ilcsoft/calice/testbed/cmake-release/calice_userlib
export CALUSER64=/exp/flc/poeschl/calice/SL4/calice_userlib/v04-09


#Setting the calice sim environment
#for digirec only, ignore if you want to process only real data
export CALSIM=/data/poeschl/ilcsoft/calice/testbed/cmake-release/calice_sim/
export CALSIM64=/exp/flc/poeschl/calice/SL4/calice_reco/



#Setting the root environment
#export ROOTSYS=/usr/local/root/v5.17i386
export ROOTSYS=/scratch/opt/root/
export ROOTSYS64=/usr/local/root/v5.17x64

#CondDBMySQL is used to access the mysql calice database
export CondDBMySQL=/data/poeschl/extern/CondDBMySQL
export CondDBMySQL64=/exp/flc/ilc/SL4/x86_64/extern/CondDBMySQL
#...and therefore also
#export MYSQL_PATH=/exp/flc/ilc/SL4/i386/extern/
export MYSQL_PATH=/usr
export MYSQL_PATH64=/exp/flc/ilc/SL4/x86_64/extern/

#allow programs to read from dCache pool
#DESY and institutes where dcache is installed (ask your system administrator)
#export LD_PRELOAD=/opt/products/lib/libpdcap.so

#....End of user action 
#Process arguments   
MARLIN32=$MARLIN
CALUSER32=$CALUSER
CALSIM32=$CALSIM
CondDBMySQL32=$CondDBMySQL
MYSQL_PATH32=$MYSQL_PATH
ROOTSYS32=$ROOTSYS
LIBDIR32=lib
BINARYDIR32=bin

LIBDIR64=lib64
BINARYDIR64=bin64
red='\e[0;31m'
cyan='\e[0;36m' 
NC='\e[0m' # No Color
option=""
willcompile=0
setenv32=1
force32=0
setenv64=0
unset DATA_ONLY
dataonly=0
if test -z  "$1"; then
  echo "Setting environment ..."
fi

if !( test -z "$1" ) && ( test "$1" != "--help" )  && ( test "$1" != "--compile" )  && ( test "$1" != "--b64" )  && ( test "$1" != "--b32" ) && ( test "$1" != "--data" ); then
    echo -e "${red}Unknown option $1${NC}"
    echo ". ./env.sh [--data] - Sets nvironment for the given architecture [only for real data]"
    echo ". ./env.sh --compile  [--data] - Sets environment and creates libraries for all available architectures i.e. 64bit or 32bit [only for real data]"
    echo ". ./env.sh --b32  [--data] - Sets environment 32bit [only for real data]"
    echo ". ./env.sh --b64  [--data] - Sets environment 64bit [only for real data]"
    echo ". ./env.sh --help  - This help"
    return
fi


if test "$1" = "--help"; then
    echo ". ./env.sh [--data] - Sets environment for the given architecture [only for real data]"
    echo ". ./env.sh --compile  - Sets environment and creates libraries for all available architectures i.e. 64bit or 32bit [only for real data] "
    echo ". ./env.sh --b32  - Sets environment 32bit [only for real data]"
    echo ". ./env.sh --b64  - Sets environment 64bit [only for real data]"
    echo ". ./env.sh --help  - This help"
    return
fi

if test "$1" = "--compile"; then
    willcompile=1
fi

if test "$1" = "--b32"; then
    setenv32=2
fi

if test "$1" = "--b64"; then
    setenv64=1
    setenv32=0
fi


if ( test "$1" = "--data" ); then
    dataonly=1
    export DATA_ONLY=1
fi

if !( test -z "$2" ) && ( test "$2" = "--data" ); then
   dataonly=1
else
    if !( test -z "$2" ) && ( test "$2" != "--data" ); then
	echo -e "${red}Unknown option $1${NC}"
	echo ". ./env.sh [--data] - Sets environment for the given architecture [only for real data]"
	echo ". ./env.sh --compile  [--data] - Sets environment and creates libraries for all available architectures i.e. 64bit or 32bit [only for real data]"
	echo ". ./env.sh --b32  [--data] - Sets environment 32bit [only for real data]"
	echo ". ./env.sh --b64  [--data] - Sets environment 64bit [only for real data]"
	echo ". ./env.sh --help  - This help"
	return
  fi
fi

#e.g. The conversion runs only on 32 bit machines, therefore we need to test
#the architecture of the machine
#if you run a 64 bit machine please make sure that you have versions of necessary libraries for both
#architectures installed
unset X86_64
arch64=0
arch64=`uname -m | tr '[A-Z]' '[a-z]' | grep x86_64 | wc -l | awk '{print $1}'`

#Function to check the environment

checkenvironment() 
{
   red='\e[0;31m'
   cyan='\e[0;36m' 
   NC='\e[0m' # No Color


    echo -e "${cyan}Testing Marlin ...${NC}"
    if ( test -z "$MARLIN") ; then
	echo -e "${red}Error: No Marlin found${NC}"
	echo -e "${red}Please set MARLIN variable${NC}"
	return
    else
	
	if ( test -e $MARLIN ) ; then
	    echo "Marlin found in $MARLIN"
	else
	    echo -e "${red}Error: $MARLIN does not exist${NC}"
	    return
	fi
	
    fi
    
    
#Quick and dirty check whether your marlin version uses AIDA
    isaida=0
    isaida=`less $MARLIN/bin/marlin_includes.sh | grep AIDAJNI | wc -l | awk '{print \$1}'`
    unset MARLIN_USE_AIDA
    
    if [ $isaida -gt 0 ]; then
	echo "INFO: Your Marlin has been linked against AIDA. Checking your setup ..."
	export MARLIN_USE_AIDA=1
	
	echo "Testing JDK_HOME ..."
	if ( test -z "$JDK_HOME") ; then
	    echo -e "${red}Error: No JDK_HOME found${NC}"
	    echo -e "${red}Please set JDK_HOME variable${NC}"
	    echo -e "${red}Look for JDK_HOME${NC}"
	    return
	else
	    
	    if ( test -e $JDK_HOME ) ; then
		echo "JDK_HOME found in $JDK_HOME"
	    else
		echo -e "${red}Error: $JDK_HOME does not exist${NC}"
		return
	    fi
	    
	fi
	
	echo "Testing JAIDA_HOME ..."
	if ( test -z "$JAIDA_HOME") ; then
	    echo -e "${red}Error: No JAIDA_HOME found${NC}"
	    echo -e "${red}Please set JAIDA_HOME variable${NC}"
	    echo -e "${red}Look for JAIDA_HOME${NC}"
	    return
	else
	    
	    if ( test -e $JAIDA_HOME ) ; then
		echo "JAIDA_HOME found in $JAIDA_HOME"
		echo "Executing setup script ..."
		. $JAIDA_HOME/bin/aida-setup.sh
	    else
		echo -e "${red}Error: $JAIDA_HOME does not exist${NC}"
		return
	    fi
	    
	fi
	
	
	
	echo "Testing AIDAJNI_HOME ..."
	if ( test -z "$AIDAJNI_HOME") ; then
	    echo -e "${red}Error: No AIDAJNI_HOME found${NC}"
	    echo -e "${red}Please set AIDAJNI_HOME variable${NC}"
	    echo -e "${red}Look for AIDAJNI_HOME${NC}"
	    return
	else
	    
	    if ( test -e $AIDAJNI_HOME ) ; then
		echo "AIDAJNI_HOME found in $AIDAJNI_HOME"
		echo "Executing setup script ..."
		. $AIDAJNI_HOME/bin/Linux-g++/aidajni-setup.sh
	    else
		echo -e "${red}Error: $AIDAJNI_HOME does not exist"${NC}
		return
	    fi
	    
	fi

#endif for check of marlin has aida
    fi


#Quick and dirty check whether your marlin version has been compiled against CLHEP 
    isclhep=0
    isclhep=`less $MARLIN/bin/marlin_libs.sh | grep CLHEP | wc -l | awk '{print \$1}'`
    
    if [ $isclhep -gt 0 ]; then
	echo -e "INFO: Your marlin version $MARLIN has been linked against CLHEP"
	echo -e "${cyan}Adding CLHEP to your LD_LIBRARY_PATH ...${NC}"
	export CLHEP_LIB=`for i in \`less $MARLIN/bin/marlin_libs.sh | grep CLHEP\` ;do echo $i; done | grep /CLHEP`
	export LD_LIBRARY_PATH=$CLHEP_LIB:$LD_LIBRARY_PATH
    else 
       if [ $dataonly -eq 0 ]; then
        echo -e "${red}Error: CLHEP not found but calice_sim depends on, recompile marlin with CLHEP support${NC}"
        return
       fi
    fi

#Quick and dirty check whether your marlin version has been compiled against CLHEP 
    isgear=0
#    unset GEAR_INCLUDES 
    isgear=`less $MARLIN/bin/marlin_libs.sh | grep gear | wc -l | awk '{print \$1}'`
    
    if [ $isgear -gt 0 ]; then
	echo "INFO: Your marlin version $MARLIN has GEAR included"
	#echo "Adding GEAR to the include files..."
	#export GEAR_LIBS=`for i in \`less $MARLIN/bin/marlin_libs.sh | grep gear\` ;do echo $i; done | grep /CLHEP`
        #export GEAR_INCLUDES=`for i in \`less $MARLIN/bin/marlin_includes.sh | grep gear\` ;do echo $i; done | grep gear`
	#export LD_LIBRARY_PATH=$CLHEP_LIB:$LD_LIBRARY_PATH
    else  
       if [ $dataonly -eq 0 ]; then
        echo -e "${red}Error: gear not found but calice_sim depends on, recompile marlin with gear support${NC}"
        return
       fi 
    fi
    
    
#Quick and dirty check whether your marlin version has compiled in
#LCCD and CONDDB
    marlinok=0
    marlinok=`less $MARLIN/bin/marlin_includes.sh | grep USE_LCCD | grep USE_CONDDB | wc -l | awk '{print \$1}'`
    
    
    
    if [ $marlinok -eq 0 ]
	then
	echo "INFO: Your marlin version $MARLIN has no LCCD included"
	echo "Might be ok for you"
    fi
    
    
    if [ $marlinok -gt 0 ]
	then
	echo "INFO: Your marlin version $MARLIN has LCCD included"
	
	
	echo -e "${cyan}Testing CondDB environment ...${NC}" 
	
	
	
	if ( test -z "$CondDBMySQL"); then
	    echo -e "${red}Error: No CondDBMySQL found${NC}"
	    echo -e "${red}Please set CondDBMySQL variable${NC}"
	    echo -e "${red}Edit this script, look for CondDBMySQL${NC}"
	    return
	else 
	    
	    if ( test -e $CondDBMySQL ); then
		echo "CondDBMySQL found in $CondDBMySQL"
		export LD_LIBRARY_PATH=$CondDBMySQL/lib:$LD_LIBRARY_PATH
	    else 
		echo -e "${red}Error: $CondDBMySQL does not exist${NC}"
		return
	    fi
	    
	fi
	
	
	echo -e "${cyan}Testing mysql environment ...${NC}"
	
	if ( test -z "$MYSQL_PATH"); then
	    echo -e "${red}Error: No MYSQL_PATH found${NC}"
	    echo -e "${red}Please set MYSQL_PATH variable${NC}"
	    echo -e "${red}Edit this script, look for MYSQL_PATH${NC}"
	    return
	else
	    if ( test -e $MYSQL_PATH ); then  
		echo "mysql found in $MYSQL_PATH"
	    else
		echo "Error: $MYSQL_PATH does not exist" 
		return
	    fi
	fi
	
	
	
    fi

#Testing Calice USER environment
echo -e "${cyan}Testing for calice userlib ...${NC}"
if ( test -z "$CALUSER"); then
  echo -e "${red}Error: No calice userlib found"
  echo "Please set CALUSER variable"
  echo -e "Edit this script, look for CALUSER${NC}"
  return
else

  if ( test -e $CALUSER ); then
    echo "calice userlib found in $CALUSER"
  else
    echo -e "${red}Error: $CALUSER does not exist${NC}"
    return
  fi

fi

#Testing Calice reco environment
if [ $dataonly -eq 0 ]; then
    echo -e "${cyan}Testing for calice_sim ...${NC}"
    if ( test -z "$CALSIM"); then
	echo -e "${red}Error: No calice sim found"
	echo "Please set CALSIM variable"
	echo -e "Edit this script, look for CALSIM${NC}"
	return
    fi

    if ( test -e $CALSIM ); then
	echo "calice sim found in $CALSIM"
    else
	echo -e "${red}Error: $CALSIM does not exist${NC}"
	return
    fi
fi
    
#Start of testing root environment
#Need to have a ROOTSYS 
echo -e "${cyan}Checking for root environment ...${NC}"
#for root version >= 5.14/00 we need to link the Spectrum library seperately
#Will analyse the root version here
unset ROOTSPECTRUM
if ( test -z "$ROOTSYS"); then
   echo -e "${red}Error: No root found"
   echo -e "Please set ROOTSYS variable${NC}"
   return
else 

 if ( test -e $ROOTSYS ); then 
   echo "root found in $ROOTSYS"
   echo -e "${cyan}adding root to PATH ...${NC}"
   export PATH=$ROOTSYS/bin:$PATH
   echo -e "${cyan}adding root to LD_LIBRARY_PATH ...${NC}"
   export LD_LIBRARY_PATH=$ROOTSYS/lib:$LD_LIBRARY_PATH
   echo -e "${cyan}Analysing the root version ...${NC}"
#Getting the root version 
   rootversion=`root-config --version`
   echo "root version is $rootversion"
#Getting the major release number of root
   rootmaj=${rootversion/%.[0-9]*\/[0-9]*}
#Getting the minor release number of root   
   helpvar=${rootversion/#[0-9].}
   rootmin=${helpvar/%\/[0-9]*}

#Define Rootsepctrum if a corresponding root version has been identified
   if [ $rootmaj -gt 5 ]; then
       export ROOTSPECTRUM=-lSpectrum
   fi

   if [ $rootmaj -eq 5 ] && [ $rootmin -gt 13 ]
   then
       export ROOTSPECTRUM=-lSpectrum
#       echo "Extra linking of Spectrum" 
   fi 

  else
     echo -e "${red}Error: $ROOTSYS does not exist${NC}"
     return
  fi 

fi

echo -e "${cyan}Testing for lapack installation as it is needed by some processors${NC}"
echo "Looking in /usr/$LIBDIR and /usr/local/$LIBDIR"
unset LAPACK_DIR
#echo $libarch


islapack=0
islapack=`ls /usr/$LIBDIR | grep lapack | wc -l | awk '{print \$1}'`

if [ $islapack -gt 0 ]; then 
  export LAPACK_DIR=/usr/$LIBDIR 
  
else
  islapack=`ls /usr/local/$LIBDIR | grep lapack | wc -l | awk '{print \$1}'`

  if [ $islapack -gt 0 ]; then
    export LAPACK_DIR=/usr/local/$LIBDIR
  fi

fi 

if [ $islapack -gt 0 ]; then
 echo "liblapack found in $LAPACK_DIR"
 export LD_LIBRARY_PATH=$LAPACK_DIR:$LD_LIBRARY_PATH
else
 echo -e "${red}Error: liblapack not found in default locations${NC}" 
 echo "Please set by hand"
 echo "Do: export LAPACK_DIR=<your path to lapack>"
fi

echo "...your library path"
echo $LD_LIBRARY_PATH


}


docompile () 
{
   red='\e[0;31m'
   cyan='\e[0;36m' 
   NC='\e[0m' # No Color


    echo -e "${cyan}Cleaning up ...${NC}"
    gmake -C src clean
    echo -e "${cyan}Compiling ...${NC}"
    gmake -C src
    echo -e "${cyan}Finished !!!${NC}"
}


export LIBDIR=$LIBDIR32
export BINARYDIR=$BINARYDIR32


if [ $setenv32 -eq 1 ] && [ $setenv64 -eq 0 ] && [ $willcompile -gt 0 ]; then

    if [ $arch64 -gt 0 ]; then
	echo "64 bit architecture detected"
	export X86_64=1
	echo "Start with compiling the 32 bit version"
	checkenvironment
	docompile
	unset X86_64
	export MARLIN=$MARLIN64
	export CondDBMySQL=$CondDBMySQL64
        export CALUSER=$CALUSER64
        if [ $dataonly -eq 0 ]; then
	    export CALSIM=$CALSIM64
        fi
        export ROOTSYS=$ROOTSYS64
        export MYSQL_PATH=$MYSQL_PATH64
	export BINARYDIR=$BINARYDIR64
	export LIBDIR=$LIBDIR64
	checkenvironment
	echo "... and now the 64 bit version"
	if ( test -e $PWD/lib64 ) ; then
	    echo -e "${cyan}lib64 directory found in $PWD/lib64 ...${NC}"
	else
	    echo "${cyan}Creating lib64 directory ...${NC}"
	    mkdir $PWD/lib64
	fi
	docompile
	echo -e "${cyan} Your environment is set to a 64 bit architecture${NC}" 
	if [ $arch64 -eq 0 ]; then
	    echo -e "This is ${red}NOT${NC} the default on your machine" 
	else 
	    echo "This is the default on your machine" 
	    echo "Please type . ./env.sh --b32 to get the seetings for 32 bit" 
	fi
	setenv64=1
	setenv32=0
    else
	echo "32 bit architecture"
	checkenvironment 
        docompile
	echo -e "${cyan}Your environment is set to a 32 bit architecture${NC}" 
	if [ $arch64 -gt 0 ]; then
	    echo -e "This is ${red}NOT${NC} the default on your machine" 
	    export X86_64=1
	else 
	    echo "This is the default on your machine" 
	fi
	setenv32=1
	setenv64=0
    fi
fi

#Set the architectures explicitly before the setting
if [ $arch64 -gt 0 ] && [ $setenv32 -lt 2 ]; then
    setenv32=0 
    setenv64=1
fi

if [ $setenv32 -gt 0 ] && [ $willcompile -eq 0 ] && [ $setenv64 -eq 0 ]; then
    setenv64=0
    export MARLIN=$MARLIN32
    export CALUSER=$CALUSER32  
    if [ $dataonly -eq 0]; then
	export CALSIM=$CALSIM32  
    fi
    export MYSQL_PATH=$MYSQL_PATH32
    export CondDBMySQL=$CondDBMySQL32
    export ROOTSYS=$ROOTSYS32
    export BINARYDIR=$BINARYDIR32
    export LIBDIR=$LIBDIR32
    checkenvironment
    echo -e "${cyan}Your environment is set to a 32 bit architecture${NC}" 
    if [ $arch64 -gt 0 ]; then
      echo -e "This is ${red}NOT${NC} the default on your machine" 
      export X86_64=1
    else 
      echo "This is the default on your machine" 
    fi
fi

if [ $setenv64 -gt 0 ] && [ $willcompile -eq 0 ]; then
    export MARLIN=$MARLIN64
    export CALUSER=$CALUSER64
    if [ $dataonly -eq 0]; then
	export CALSIM=$CALSIM64
    fi
    export CondDBMySQL=$CondDBMySQL64
    export MYSQL_PATH=$MYSQL_PATH64
    export ROOTSYS=$ROOTSYS64
    export LIBDIR=$LIBDIR64
    export BINARYDIR=$BINARYDIR64
    checkenvironment
    echo -e "${cyan}Your environment is set to a 64 bit architecture${NC}" 
    if [ $arch64 -eq 0 ]; then
      echo -e "This is ${red}NOT${NC} the default on your machine" 
    else 
      echo "This is the default on your machine" 
      echo "Please type . ./env.sh --b32 to get the seetings for 32 bit" 
    fi
fi

#Finally setting calice include include files
#unset CALSIM_INCLUDES
#if [ $dataonly -eq 0 ]; then
#    export CALSIM_INCLUDES="-I $CALSIM/digitization/digihcal/include -I $CALSIM/digitization/digisim/include/ -I $CALSIM/ganging/include"
#fi
#echo $MARLIN
#echo $CondDBMySQL
#echo $LIBDIR

#echo -e "${red}This is BASH${NC}"
