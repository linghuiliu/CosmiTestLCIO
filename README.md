# CosmiTestLCIO
LCIO analyzer for cosmic test of HBU at DESY 2017-18

How to setup
1. Replace "directorypath" to your own directory path. There are 5 files "directorypath" is included.
        user-pro-test_x86_64_gcc44_sl6.cmake
        calice_cmake/calice.cmake
        RootTreeWriter/engines/HodoscopeWriteEngine.cc
        setup.sh
        test/steering_hodoscope.xml

2.
```
 mkdir myInstall
 mkdir build_calice_userlib
 mkdir build_calice_analysis
 mkdir build_calice_calib
 mkdir build_calice_reco
 mkdir build_labview_converter
 mkdir build_RootTreeWriter
```
                   
3.
```
 cd build_calice_userlib
 cmake -C ../user-pro-test_x86_64_gcc44_sl6.cmake ../calice_userlib/
 make
 make install
```

   Do the same for all the "build_" directories.

4. ``` source setup.sh```

5. Run
```
 Marlin test/steering_hodoscope.xml
```
   with proper input and output files.

