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


2) How to build and run
-----------------------

- Checkout code
svn co https://svnsrv.desy.de/public/aidasoft/DD4hep/trunk DD4hep

- Configure (for PyROOT)

mkdir build; cd build
cmake ../DD4hep
## setup environment first - see below: make -j4

- Configure (for XercesC)

cmake -DDD4HEP_USE_XERCESC=ON -DDD4HEP_USE_PYROOT=OFF -DXERCESC_ROOT_DIR=<xercesc> ../DD4hep


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


5) Running the examples
-----------------------

Here are a few command lines for running examples
Do not forget to set for Geant4:  
G4SYS = Geant4_ROOT_DIR
export G4LEDATA=${G4SYS}/share/Geant4-9.5.1/data/G4EMLOW6.23
export G4LEVELGAMMADATA=${G4SYS}/share/Geant4-9.5.1/data/PhotonEvaporation2.2


- ILDExDet example with the example detectors
  bin/geoDisplay file:../DD4hep/DDExamples/ILDExDet/compact/ILDEx.xml

- Run Geant4 with ILDExDet detector
  bin/CLICSiDSimu   \
     file:../DD4hep/DDExamples/ILDExDet/compact/ILDEx.xml  \
     file:../DD4hep/DDExamples/ILDExDet/compact/geant4.xml \
     ../DD4hep/DDExamples/ILDExSimu/run1.mac 

- ILDExDet example with Astrid's prototype TPC
  bin/geoDisplay  file:../DD4hep/DDExamples/ILDExDet/compact/TPCPrototype.xml

- ILDExDet example with the LDC00_01Sc detector (under construction)
  bin/geoDisplay  file:../DD4hep/DDExamples/ILDExDet/compact/Tesla.xml

- CLICSid example with all SiD sub-detectors
  bin/geoDisplay file:../DD4hep/DDExamples/CLICSiD/compact/compact.xml 

- Geant4 example with SiD (careful, need to set Geant4 variables in addition)
  bin/CLICSiDSimu   \
     file:../DD4hep/DDExamples/CLICSiD/compact/compact.xml             \
     file:../DD4hep/DDExamples/CLICSiD/compact/sensitive_detectors.xml \
     ../DD4hep/DDExamples/CLICSiDSimu/run.mac 


6) run SLIC simulation on lccd file created with DD4Hep:

# create lcdd file from compact:
./bin/geoConverter -compact2lcdd -input file:../DD4hep/DDExamples/ILDExDet/compact/ILDEx.xml -output ILD_toy.lcdd

# make sure slic is in the path and then:
slic -g ./ILD_toy.lcdd -o ILD_toy.slcio -x -m ../DD4hep/DDExamples/ILDExSimu/run1.mac
