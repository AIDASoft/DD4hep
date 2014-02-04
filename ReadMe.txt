Detector Description for HEP Package
-------------------------------------

1) Package Contents
-------------------

  The DD4hep project consists of several packages. The idea has been to separate the common parts of 
the detector description toolkit from concrete detector examples. 

- DDCore

  Contains the definition of the basic classes of the toolkit: Handle, DetElement, Volume, PlacedVolume,
Shapes, Material, etc. Most of these classes are ‘handles’ to ROOT TGeom classes.
The package provides the basic mechanisms for constructing the ‘Generic Detector Description Model’ 
in memory from XML compact detector definition files. Two methods are currently supported: one based
on the C++ XercesC parser, and another one based on Python and using the PyROOT bindings to ROOT. 

  Two configuration options selects the desired method:

     DD4HEP_USE_PYROOT:BOOL
     DD4HEP_USE_XERCESC:BOOL

- DDGeant4

  Is the package that contains the conversion of DD4hep geometry into Geant4 geometry to be used for 
simulation. The conversion is currently based on the Virtual Geometry Modeler (VGM) package that is
build internally when required.
  The option DD4HEP_WITH_GEANT4:BOOL controls the building or not of this package that has the dependency
to Geant4. The Geant4 installation needs to be located using the variable -DGeant4_DIR=<path to Geant4Config.cmake>

- examples

  Contains different detector examples (CLICSiD, ILDExDet, ILDExSimu) together with example applications to illustrate
the their usage for simulation, reconstruction and visualization with an emphasis to minimize software
dependencies. For instance, the simulation application example will require to compiler and link 
against Geant4 but not the reconstruction application.

  For each detector example the directory ‘compact’ contains the XML compact detector description, the 
directory ‘drivers’ contains the Python functions to construct the detector from the XML, the ‘include’ 
and ‘src’ contain the equivalent drivers for the XercesC parser.


3) Pre-requisites
-----------------

  DD4hep depends on a number of ‘external’ packages. The user will need to install these in his/her 
system before building and running the examples

- Mandatory
  CMake (version 2.8 or higher) and ROOT (version 5.30 or higher) installations are mandatory.

- Optional
  If the XercesC is used to parse compact descriptions and installation of XercesC will be required.
To build and run the simulation examples Geant4 will be required. 


4) How to build DD4hep 
-----------------------

- Checkout code

  svn co https://svnsrv.desy.de/public/aidasoft/DD4hep/trunk DD4hep

- set the environment, at least ROOT needs to be initialized, e.g.

  source  /data/ilcsoft/root/5.34.03/bin/thisroot.sh

  ( minimum is: export ROOTSYS=/data/ilcsoft/root/5.34.03 )


- configure and build:

  cd DD4hep
  mkdir build; cd build
  cmake ..
  make -j install


- some useful build options:

  * build doxygen documentation ( after 'install' open ./doc/html/index.html)
    -D INSTALL_DOC=on 
 
  * build with geant4 support:
   -D DD4HEP_WITH_GEANT4=on -D Geant4_DIR=__path_to_Geant4Config.cmake__
  
    [note: you might have to update your environment beforehand to have all needed 
     libraries in the shared lib search path (this will vary with OS, shell, etc.) e.g 
      . /data/ilcsoft/geant4/9.5/bin/geant4.sh
      export CLHEP_BASE_DIR="/data/ilcsoft/HEAD/CLHEP/2.1.0.1"
      export CLHEP_INCLUDE_DIR="$CLHEP_BASE_DIR/include"
      export PATH="$CLHEP_BASE_DIR/bin:$PATH"
      export LD_LIBRARY_PATH="$CLHEP_BASE_DIR/lib:$LD_LIBRARY_PATH"
     ]

  * use xerces instead of tinyxml    
  -DDD4HEP_USE_XERCESC=ON -DXERCESC_ROOT_DIR=<xercesc>


  * use pyroot to write python geometry drivers ( currently broken !?)
   -DDD4HEP_USE_PYROOT=ON

 - example for building a 'complete' DD4hep:
    cmake -D DD4HEP_WITH_GEANT4=1 -DDD4HEP_USE_BOOST=ON -DDD4HEP_USE_LCIO=ON -DBOOST_ROOT=/data/ilcsoft/boost/1.55.0 -DBUILD_TESTING=On ..

- Setup the environment for running 
  
  . ./bin/thisdd4hep.sh
     or
  source ./bin/thisdd4hep.csh


5) Testing
-----------

 DD4hep has a testing mechanism built on CMake/CTest. If configured with -DBUILD_TESTING=On, some make targets are
 created automatically. 
 - running the tests:

  cd build
  make install
  make test

 - running the build - after an update (to the trunk) and then the tests with finally submitting the results to the Dasboard 
   ( http://aidasoft.desy.de/CDash/index.php?project=DD4hep )

  cd build
  make -j4 ExperimentalStart ExperimentalUpdate ExperimentalBuild ExperimentalSubmit
  make install ExperimentalTest ExperimentalSubmit

 - running individual tests with output (e.g. in case of failed tests):

  cd build 
  ctest -V -R units
  # runs only test_units

6) Extending
------------

  Users may extend the models by changing the compact description or adding new drivers


7) Release notes
----------------
  If things do not work please also consult the release notes. It may happen that it was 
  forgotten to update the information here.


8) Building the examples
-----------------------

Before any of the (new) examples in the ./examples directory can be build,
one needs to set the environment:

  source __path_where_DD4hep_is_installed_/bin/thisdd4hep.sh
  source __path_to_ilcsoft_installation/init_ilcsoft.sh

or, optionally one can initialize ROOT only, and then specify all dependencies
on the cmake command line - replace 'cmake ..' below with, e.g:

  source /space/ilcsoft/root/5.34.05/bin/thisroot.sh 
  cmake -DDD4hep_DIR=~/DD4Hep/trunk/ -DGeant4_DIR=/space/ilcsoft/geant4/9.6.p01/lib/Geant4-9.6.1  \
        -D LCIO_DIR=/space/ilcsoft/v01-17-04/lcio/v02-04-03 -D GEAR_DIR=/space/ilcsoft/v01-17-04/gear/v01-03-01  ..

(change pathes according to your environment)


- then building the examples should be very straight forward. e.g:
   (check for additional instructions in example subdirectory)

  cd ./examples/ILDExDet/
  mkdir build ; cd build 
  cmake ..
  make -j install

or build all examples in one go:

  cd ./examples
  mkdir build ; cd build 
  cmake ..
  make -j install
 

- before running the example, initialize the environment:
  . ./bin/thisILDExDet.sh 


- then you can for example display the ILD detector (toy model):

  geoDisplay ../compact/ILDEx.xml

- or convert the compact to an lcdd file:

  geoConverter -compact2lcdd -input file:../compact/ILDEx.xml -output ILD_toy.lcdd


- running example tests:
  -----------------------

  make install ; make test
 
  or: 
  ctest -V -R converter
  ctest -V -R sim
  ctest -V -R noviceN04


9) run SLIC simulation on lccd file created with DD4Hep:


# make sure slic is in the path and then:
slic -g ./ILD_toy.lcdd -o ILD_toy.slcio -x -m ../DD4hep/DDExamples/ILDExSimu/run1.mac
