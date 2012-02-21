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
svn co https://svnsrv.desy.de/basic/aidasoft/DD4hep/trunk DD4hep

- Configure (for PyROOT)

mkdir build; cd build
cmake ../DD4hep
make -j4

- Configure (for XercesC)

cmake -DDD4HEP_USE_XERCESC=ON -DDD4HEP_USE_PYROOT=OFF -DXERCESC_ROOT_DIR=<xercesc> ../DD4hep

- Run ILDEx display

DDExamples/ILDExDisplay/ILDExDisplay file:../DD4hep/DDExamples/ILDExDet/compact/ILDEx.xml

- Run CLICSiD display

DDExamples/CLICSiD/CLICSiDtest file:../DD4hep/DDExamples/CLICSiD/compact/compact.xml


3) Extending
------------

  Users may extend the models by changing the compact description or adding new drivers
