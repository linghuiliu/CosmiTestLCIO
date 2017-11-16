Installation:
You need CMake 2.6 (or higher), ROOT and a FindROOT.cmake in cmakes system
module directory. Usually this comes as CMakeModules from ILCSoft, which is
described here. Lets assume you have a working ILCSoft installation located
in ${ILCPATH}.

It is recommended to do an out of source build (create a build directory for
compilation) to avoid mixing of source code and compiled code.

The installation is easy:

1.) Create a build directory relative to the DeadAndNoisyTools main directory.
    Change into it.
~/DeadAndNoisyTools> mkdir build
~/DeadAndNoisyTools> cd build

2.) Run cmake, using the initial cache file from ICLSoft. This creates the 
    Makefile. Do not forget the two dots at the end.
~/DeadAndNoisyTools/build> cmake -C ${ILCPATH}/ILCSoft.cmake ..

3.) Just run make and make install. It will install to bin and lib 
    relative to the DeadAndNoisyTools directory.
~/DeadAndNoisyTools/build> make
~/DeadAndNoisyTools/build> make install

The install directory is the root of the DeadAndNoisyTools. The library in
in the 'lib' subdirectory, the execuabltes in 'bin'. 'make install' also
creates the documentation.

Creating the docmentation:
Running 'make doc' in the build directory you can create the doxygen html
information. Obviously you need to have doxygen installed for this.
Afterwards launch doc/html/index.html in your favorite web browser.
Launching the docu build manually can be useful in case the code does not
compile and make install never reaches this point.
