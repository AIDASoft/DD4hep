# DD4hep (Detector Description for High Energy Physics)
[![Build Status](https://travis-ci.org/AIDASoft/DD4hep.svg?branch=master)](https://travis-ci.org/AIDASoft/DD4hep)

DD4hep is a software framework for providing a complete solution for full detector description
(geometry, materials, visualization, readout, alignment, calibration, etc.) for the full experiment life cycle (detector concept development, detector optimization, construction, operation). It offers a consistent description through a single source of detector information for simulation, reconstruction, analysis, etc. It distributed under the [GPLv3 License](http://www.gnu.org/licenses/gpl-3.0.en.html)

[![License](https://www.gnu.org/graphics/gplv3-127x51.png)](https://www.gnu.org/licenses/gpl-3.0.en.html)

## Package Contents
-------------------
The DD4hep project consists of several packages. The idea has been to separate the common parts of the detector description toolkit from concrete detector examples.

- **DDCore**

 Contains the definition of the basic classes of the toolkit: Handle, DetElement, Volume, PlacedVolume, Shapes, Material, etc. Most of these classes are ‘handles’ to ROOT TGeom classes. The package provides the basic mechanisms for constructing the ‘Generic Detector Description Model’ in memory from XML compact detector definition files. Two methods are currently supported: one based on the C++ XercesC parser, and another one based on Python and using the PyROOT bindings to ROOT. 

 Two configuration options selects the desired method:
    `DD4HEP_USE_PYROOT:BOOL`
    `DD4HEP_USE_XERCESC:BOOL`

- **DDG4**

 Is the package that contains the conversion of DD4hep geometry into Geant4 geometry to be used for simulation. The conversion is currently based on the Virtual Geometry Modeler (VGM) package that is build internally when required. The option DD4HEP_USE_GEANT4:BOOL controls the building or not of this package that has the dependency to Geant4. The Geant4 installation needs to be located using the variable `-DGeant4_DIR=<path to Geant4Config.cmake>`

- **examples**

  Contains different detector examples (CLICSiD, AlignDet, ClientTests) together with example applications to illustrate the their usage for simulation, reconstruction and visualization with an emphasis to minimize software dependencies. For instance, the simulation application example will require to compiler and link against Geant4 but not the reconstruction application.

  For each detector example the directory ‘compact’ contains the XML compact detector description, the 
directory ‘drivers’ contains the Python functions to construct the detector from the XML, the ‘include’ 
and ‘src’ contain the equivalent drivers for the XercesC parser.

## Pre-requisites

  DD4hep depends on a number of ‘external’ packages. The user will need to install these in his/her 
system before building and running the examples

- **Mandatory**

  CMake (version 2.8 or higher), ROOT (version 5.30 or higher) and Boost installations are mandatory.

- **Optional**

  If the XercesC is used to parse compact descriptions and installation of XercesC will be required. To build and run the simulation examples Geant4 will be required. There is also an available interface to LCIO

## How to build DD4hep 
### Checkout code
`svn co https://svnsrv.desy.de/public/aidasoft/DD4hep/trunk DD4hep`

### Set the environment 
at least ROOT and BOOST needs to be initialized, e.g.
`. /afs/cern.ch/sw/lcg/app/releases/ROOT/6.06.02/x86_64-slc6-gcc48-opt/root/bin/thisroot.sh`
and
`export BOOST_ROOT=/afs/cern.ch/sw/lcg/releases/LCG_84/Boost/1.59.0_python2.7/x86_64-slc6-gcc48-opt`

### Configure and build 

  `cd DD4hep`

  `mkdir build; cd build`

  `cmake -DBoost_NO_BOOST_CMAKE=ON ..`

  `make -j4 install`

### Additional useful build options:

  * build Doxygen documentation ( after 'install' open ./doc/html/index.html)

    `-D INSTALL_DOC=on `
 
  * build with geant4 support:

   `-D DD4HEP_USE_GEANT4=on -D Geant4_DIR=__path_to_Geant4Config.cmake__`
  
    note: you might have to update your environment beforehand to have all needed libraries in the shared lib search path (this will vary with OS, shell, etc.) e.g. `. __path_to_Geant4__/bin/geant4.sh`
     `export CLHEP_BASE_DIR="__path_to_CLHEP__"`
     `export LD_LIBRARY_PATH="$CLHEP_BASE_DIR/lib:$LD_LIBRARY_PATH"`
     

  * use XercesC instead of TinyXml    
  `-D DD4HEP_USE_XERCESC=ON -D XERCESC_ROOT_DIR=__path_to_xercesc__`


  * use pyroot to write python geometry drivers 
   `-D DD4HEP_USE_PYROOT=ON`

### Setup the environment for running 
  
  `. ./bin/thisdd4hep.sh`

or

  `source ./bin/thisdd4hep.csh`


## Testing


 DD4hep has a testing mechanism built on CMake/CTest. If configured with `-DBUILD_TESTING=ON`, some make targets are
 created automatically. 

 - running the tests:

  `cd build`

  `make install`

  `make test`

 - running the build 

   For each commit all tests are run through **Travis CI**. DD4hep is tested against gcc 4.8 on SLC6. Build results can be observed [here](https://travis-ci.org/AIDASoft/DD4hep) and the status is indicated by the badge [![Build Status](https://travis-ci.org/AIDASoft/DD4hep.svg?branch=master)](https://travis-ci.org/AIDASoft/DD4hep)


 - running individual tests with output (e.g. in case of failed tests):

  `cd build` 

  `ctest -V -R units`
  # runs only test_units

 - running nightly builds and tests with cdash:

  `make NightlyStart`

  `make NightlyConfigure`

  `make NightlyBuild`

  `make install`

  `make NightlyTest`

  `make NightlySubmit`

## Documentation

Doxygen documentation is build nightly and can be found [here](http://test-dd4hep.web.cern.ch/test-dd4hep/doxygen/html/index.html)


## Extending

Users may extend the models by changing the compact description or adding new drivers


## Release notes

If things do not work please also consult the release notes. It may happen that it was forgotten to update the information here.


