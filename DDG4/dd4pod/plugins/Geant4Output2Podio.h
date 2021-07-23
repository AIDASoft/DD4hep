#ifndef DD4HEP_DDG4_GEANT4Output2Podio_H
#define DD4HEP_DDG4_GEANT4Output2Podio_H

#include "DDG4/Geant4OutputAction.h"
//#include "podio/GenericParameters.h"
#include "podio/CollectionBase.h"
#include "podio/CollectionIDTable.h"

#include "podio/EventStore.h"
#include "podio/ROOTWriter.h"

namespace dd4pod {
class Geant4Particle;
class Geant4ParticleCollection;
}

namespace dd4hep {


  class ComponentCast;

  namespace sim {

    class Geant4Particle;

    /** Saves output to podio data model.
     *
     * Uses the podio data model "dd4pod" as the output. 
     */
    class Geant4Output2Podio: public Geant4OutputAction {
    protected:
     void
     ConstructParticle(dd4pod::Geant4ParticleCollection* col,
                       dd4hep::sim::Geant4Particle*      part);

     /// Flag if Monte-Carlo truth should be followed and checked
     bool m_handleMCTruth;

     /// Property: vector with disabled collections
     std::vector<std::string> m_disabledCollections;

     /// Property: vector with disabled collections
     std::vector<std::string> m_enabledCollections;

     /// Property: vector with disabled collections
     bool m_disableParticles = false;

     podio::EventStore  store;
     podio::ROOTWriter* writer = nullptr;

     std::map<std::string, podio::CollectionBase*>               m_col_map;
     std::vector<std::pair<std::string, podio::CollectionBase*>> m_collections;
     std::vector<std::pair<std::string, podio::CollectionBase*>>
                               m_readCollections;
     podio::CollectionIDTable* m_collectionIDs;

    public:
      Geant4Output2Podio(Geant4Context* context, const std::string& nam);
      virtual ~Geant4Output2Podio();
      
      ///// Fill single EVENT branch entry (Geant4 collection data)
      //int fill(const std::string& nam, const ComponentCast& type, void* ptr);

      /// Callback to store the Geant4 run information
      virtual void beginRun(const G4Run* run);

      /// Callback to store the Geant4 run information
      virtual void endRun(const G4Run* run);

      /// Callback to store the Geant4 run information
      virtual void saveRun(const G4Run*  run );

      /// Callback to store each Geant4 hit collection
      virtual void saveCollection(OutputContext<G4Event>& ctxt, G4VHitsCollection* collection);

      /// Callback to store the Geant4 event
      virtual void saveEvent(OutputContext<G4Event>& ctxt);

      /// Commit data at end of filling procedure
      virtual void commit(OutputContext<G4Event>& ctxt);
    };

  }    // End namespace sim
}      // End namespace dd4hep
#endif // DD4HEP_DDG4_GEANT4Output2Podio_H
