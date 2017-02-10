#include "DD4hep/DDTest.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <exception>

#include "DD4hep/Plugins.h"
#include "DD4hep/Primitives.h"
#include "DDG4/Geant4InputAction.h"
#include "DDG4/Geant4Particle.h"
#include "DDG4/Geant4Vertex.h"

//using namespace DD4hep::Simulation;
typedef DD4hep::Simulation::Geant4Vertex   Vertex;
typedef DD4hep::Simulation::Geant4Particle Particle;

static DD4hep::DDTest test( "EventReader" ) ;

class TestTuple {
public:
  std::string readerType;
  std::string inputFile;
  bool skipEOF; //LCIO skipNEvents does not signal end of file??
  TestTuple( std::string const& rT, std::string const& iF, bool sEOF=false): readerType(rT), inputFile(iF), skipEOF(sEOF) {}
};


int main(int argc, char** argv ){

  if( argc < 2 ) {
    std::cout << " usage:  test_EventReaders Path/To/InputFiles " << std::endl ;
    exit(1) ;
  }

  std::string inputFileFolder = argv[1];

  std::vector<TestTuple> tests;
  tests.push_back( TestTuple( "LCIOStdHepReader", "bbudsc_3evt.stdhep" ) );
  tests.push_back( TestTuple( "LCIOFileReader",   "muons.slcio" , /*skipEOF= */ true ) );
  tests.push_back( TestTuple( "Geant4EventReaderHepEvtShort", "Muons10GeV.HEPEvt" ) );
  tests.push_back( TestTuple( "Geant4EventReaderHepMC", "g4pythia.hepmc" ) );


  try{
    for(std::vector<TestTuple>::const_iterator it = tests.begin(); it != tests.end(); ++it) {
      std::string readerType = (*it).readerType;
      std::string fileName = (*it).inputFile;
      bool skipEOF =  (*it).skipEOF;
      //InputFiles are in DDTest/inputFiles, argument is cmake_source directory
      std::string inputFile = argv[1]+ std::string("/inputFiles/") + fileName;
      DD4hep::Simulation::Geant4EventReader* thisReader = DD4hep::PluginService::Create<DD4hep::Simulation::Geant4EventReader*>(readerType, inputFile);
      if ( not thisReader ) {
        test.log( "Plugin not found" );
        test.log( readerType );
        continue;
      }
      test( thisReader->currentEventNumber() == 0 , readerType + std::string("Initial Event Number") );
      thisReader->moveToEvent(1);
      test( thisReader->currentEventNumber() == 1 , readerType + std::string("Event Number after Skip") );
      std::vector<Particle*> particles;
      std::vector<Vertex*> vertices ;

      DD4hep::Simulation::Geant4EventReader::EventReaderStatus sc = thisReader->readParticles(3,vertices,particles);
      std::for_each(particles.begin(),particles.end(),DD4hep::deleteObject<Particle>);
      test( thisReader->currentEventNumber() == 2 && sc == DD4hep::Simulation::Geant4EventReader::EVENT_READER_OK,
            readerType + std::string("Event Number Read") );

      //Reset Reader to check what happens if moving to far in the file
      if (not skipEOF) {
        thisReader = DD4hep::PluginService::Create<DD4hep::Simulation::Geant4EventReader*>(readerType, inputFile);
        sc = thisReader->moveToEvent(1000000);
        test( sc != DD4hep::Simulation::Geant4EventReader::EVENT_READER_OK , readerType + std::string("EventReader False") );
      }
    }

  } catch( std::exception &e ){
    //} catch( ... ){

    test.log( e.what() );
    test.error( "exception occurred" );
  }
  return 0;
}

