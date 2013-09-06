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

- DDExamples

  Contains different detector examples (CLICSiD, ILDEx) together with example applications to illustrate
the their usage for simulation, reconstruction and visualization with an emphasis to minimize software
dependencies. For instance, the simulation application example will require to compiler and link 
against Geant4 but not the reconstruction application.

  For each detector example the directory ‘compact’ contains the XML compact detector description, the 
directory ‘drivers’ contains the Python functions to construct the detector from the XML, the ‘include’ 
and ‘src’ contain the equivalent drivers for the XercesC parser.

- DDExamples/ILDEx

  Contains the original example ILDEx adapted from the Geant4 example N03

3) Pre-requisites
-----------------

  DD4hep depends on a number of ‘external’ packages. The user will need to install these in his/her 
system before building and running the examples

- Mandatory
  CMake (version 2.8 or higher) and ROOT (version 5.30 or higher) installations are mandatory.

- Optional
  If the XercesC is used to parse compact descriptions and installation of XercesC will be required.
To build and run the simulation examples Geant4 will be required. 


2) How to build DD4hep 
-----------------------

- Checkout code

  svn co https://svnsrv.desy.de/public/aidasoft/DD4hep/trunk DD4hep

- set the environment, at least ROOT needs to be initialized, e.g.

  source  /data/ilcsoft/root/5.34.03/bin/thisroot.sh

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






- Setup the running environment

. thisdd4hep.sh
     or
source thisdd4hep.csh

### now call make ###########
make -j4



- Run ILDEx display

DDExamples/ILDExDisplay/ILDExDisplay file:../DD4hep/DDExamples/ILDExDet/compact/ILDEx.xml

- Run CLICSiD display

DDExamples/CLICSiD/CLICSiDtest file:../DD4hep/DDExamples/CLICSiD/compact/compact.xml


3) Extending
------------

  Users may extend the models by changing the compact description or adding new drivers


4) Release notes
----------------
  If things do not work please also consult the release notes. It may happen that it was 
  forgotten to update the information here.


5) Building the examples
-----------------------

Before any of the (new) examples in the ./examples directory can be build,
one needs to set the environment:

 source __path_where_ROOT_is_installed___/bin/thisroot.sh
 source __path_where_DD4hep_is_installed_/bin/thisdd4hep.sh
 
 [note: possible other scripts need to be called, e.g. when using geant4  - see above ]


- then building the examples should be very straight forward. e.g:
   (check for additional instructions in example subdirectory)

  cd ./examples/ILDExDet/
  mkdir build ; cd build 
  cmake ..
  make -j install

- before running the example, again the (DY)LD_LIBRARY_PATH has to be updated:
  export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$PWD/../lib

- then you can for example display the ILD detector (toy model):

  geoDisplay ../compact/ILDEx.xml

- or convert the compact to an lcdd file:

  geoConverter -compact2lcdd -input file:../compact/ILDEx.xml -output ILD_toy.lcdd



6) run SLIC simulation on lccd file created with DD4Hep:

# create lcdd file from compact:
./bin/geoConverter -compact2lcdd -input file:../DD4hep/DDExamples/ILDExDet/compact/ILDEx.xml -output ILD_toy.lcdd

# make sure slic is in the path and then:
slic -g ./ILD_toy.lcdd -o ILD_toy.slcio -x -m ../DD4hep/DDExamples/ILDExSimu/run1.mac
