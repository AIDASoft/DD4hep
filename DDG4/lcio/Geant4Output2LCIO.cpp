// $Id: Geant4Field.cpp 875 2013-11-04 16:15:14Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4OUTPUT2LCIO_H
#define DD4HEP_DDG4_GEANT4OUTPUT2LCIO_H

// Framework include files
#include "DD4hep/VolumeManager.h"
#include "DDG4/Geant4OutputAction.h"

// lcio include files
#include "lcio.h"
#include "IO/LCWriter.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  class ComponentCast;

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation   {
    
    /** @class Geant4Output2LCIO Geant4Output2LCIO.h DDG4/Geant4Output2LCIO.h
     * 
     * Base class to output Geant4 event data to media
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4Output2LCIO : public Geant4OutputAction  {
    protected:
      Geometry::VolumeManager m_volMgr;
      lcio::LCWriter*  m_file;
      int              m_runNo;
      
    public:
      /// Standard constructor
      Geant4Output2LCIO(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4Output2LCIO();
      /// Callback to store the Geant4 run information
      virtual void beginRun(const G4Run* run);
      /// Callback to store the Geant4 run information
      virtual void endRun(const G4Run* run);

      /// Callback to store the Geant4 run information
      virtual void saveRun(const G4Run* run);
      /// Callback to store the Geant4 event
      virtual void saveEvent( OutputContext<G4Event>& ctxt);
      /// Callback to store each Geant4 hit collection
      virtual void saveCollection( OutputContext<G4Event>& ctxt, G4VHitsCollection* collection);
      /// Commit data at end of filling procedure
      virtual void commit( OutputContext<G4Event>& ctxt);

      /// begin-of-event callback - creates LCIO event and adds it to the event context
      virtual void begin(const G4Event* event);
    };

  }    // End namespace Simulation
}      // End namespace DD4hep
#endif // DD4HEP_DDG4_GEANT4OUTPUT2LCIO_H

// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4HitCollection.h"
#include "DDG4/Geant4DataConversion.h"
#include "DDG4/Geant4Context.h"

//#include "DDG4/Geant4Output2LCIO.h"
#include "G4Event.hh"
#include "G4Run.hh"

// LCIO include files
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCRunHeaderImpl.h"
#include "IMPL/LCCollectionVec.h"


using namespace DD4hep::Simulation;
using namespace DD4hep;
using namespace std;


#include "DDG4/Factories.h"
DECLARE_GEANT4ACTION(Geant4Output2LCIO)


/// Standard constructor
Geant4Output2LCIO::Geant4Output2LCIO(Geant4Context* ctxt, const string& nam)
: Geant4OutputAction(ctxt,nam), m_volMgr(), m_file(0), m_runNo(0)
{
  InstanceCount::increment(this);
}

/// Default destructor
Geant4Output2LCIO::~Geant4Output2LCIO()  {
  InstanceCount::decrement(this);
  if ( m_file )  {
    m_file->close();
    deletePtr(m_file);
  }
}

// Callback to store the Geant4 run information
void Geant4Output2LCIO::beginRun(const G4Run* )  {
  if ( 0 == m_file && !m_output.empty() )   {
    m_file = lcio::LCFactory::getInstance()->createLCWriter();
    m_file->open(m_output,lcio::LCIO::WRITE_NEW);
    // Get the volume manager here: 
    // in the constructor the geometry may not yet be built!
    m_volMgr = context()->lcdd().volumeManager();
  }
}

/// Callback to store the Geant4 run information
void Geant4Output2LCIO::endRun(const G4Run* )  {
}

/// Commit data at end of filling procedure
void Geant4Output2LCIO::commit( OutputContext<G4Event>& ctxt)   {
  //lcio::LCEventImpl* e = ctxt.data<lcio::LCEventImpl>();
  lcio::LCEventImpl* e = context()->event().extension<lcio::LCEventImpl>();
  m_file->writeEvent(e);
  std::cout << " ########### Geant4Output2LCIO::commit() : writing LCIO event to file .... "  << std::endl ;
}

/// Callback to store the Geant4 run information
void Geant4Output2LCIO::saveRun(const G4Run* run)  {
  // --- write an lcio::RunHeader ---------
  lcio::LCRunHeaderImpl* rh =  new lcio::LCRunHeaderImpl;
  rh->setRunNumber(m_runNo=run->GetRunID());
  rh->setDetectorName(context()->lcdd().header().name());
  m_file->writeRunHeader(rh);
}


void Geant4Output2LCIO::begin(const G4Event* event){

  lcio::LCEventImpl* e  = new lcio::LCEventImpl;

  //fg: fixme: should be this call (deleting the pointer in the end) but that does not compile ...
  //  context()->event().addExtension<lcio::LCEventImpl>( e );

  context()->event().addExtension<lcio::LCEventImpl>( e );
  //context()->event().addExtension( e , typeid( lcio::LCEventImpl ), 0);

  //  std::cout << " ########### Geant4Output2LCIO::begin  add new LCIO event  event context " << std::endl ;
}


/// Callback to store the Geant4 event
void Geant4Output2LCIO::saveEvent(OutputContext<G4Event>& ctxt)  {
  // lcio::LCEventImpl* e  = new lcio::LCEventImpl;
  //  ctxt.userData = e;
  lcio::LCEventImpl* e = context()->event().extension<lcio::LCEventImpl>();

  e->setRunNumber(m_runNo);
  e->setEventNumber(ctxt.context->GetEventID());
  e->setDetectorName(context()->lcdd().header().name());
  e->setRunNumber(m_runNo);

}

/// Callback to store each Geant4 hit collection
void Geant4Output2LCIO::saveCollection(OutputContext<G4Event>& ctxt, G4VHitsCollection* collection)  {
  size_t nhits = collection->GetSize();
  std::string hc_nam = collection->GetName();
  if ( nhits > 0 )   {
    typedef pair<Geometry::VolumeManager,G4VHitsCollection*> _A;
    typedef Geant4Conversion<lcio::LCCollectionVec,_A> _C;
    const _C& cnv = _C::converter(typeid(Geant4HitCollection));
    //lcio::LCEventImpl* evt = ctxt.data<lcio::LCEventImpl>();
    lcio::LCEventImpl* evt = context()->event().extension<lcio::LCEventImpl>();
    lcio::LCCollectionVec* col = cnv(_A(m_volMgr,collection));
    evt->addCollection(col,hc_nam);
  }
}
