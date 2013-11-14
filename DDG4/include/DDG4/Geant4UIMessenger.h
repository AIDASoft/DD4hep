// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4UIMESSENGER_H
#define DD4HEP_DDG4_GEANT4UIMESSENGER_H

// Framework include files
#include "DDG4/ComponentProperties.h"
#include "DDG4/Geant4Callback.h"
#include "G4UImessenger.hh"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    /** @class Geant4UIMessenger Geant4UIMessenger.h DDG4/Geant4UIMessenger.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4UIMessenger: public G4UImessenger {
    public:
      typedef std::map<G4UIcommand*, std::string> Commands;
      typedef std::map<G4UIcommand*, Callback> Actions;
    protected:
      /// The UI directory of this component
      G4UIdirectory* m_directory;
      /// Reference to the property manager corresponding to the component
      PropertyManager* m_properties;
      /// Component name
      std::string m_name;
      /// Path in the UI hierarchy of this component
      std::string m_path;
      /// Property update command map
      Commands m_propertyCmd;
      /// Action map
      Actions m_actionCmd;

    public:
      /// Initializing constructor
      Geant4UIMessenger(const std::string& name, const std::string& path);
      /// Default destructor
      virtual ~Geant4UIMessenger();
      /// Add a new callback structure
      void addCall(const std::string& name, const std::string& description, const Callback& cb);
      /// Add any callback (without parameters to the messenger
      template <typename Q, typename R, typename T>
      void addCall(const std::string& name, const std::string& description, Q* p, R (T::*f)()) {
        CallbackSequence::checkTypes(typeid(Q), typeid(T), dynamic_cast<T*>(p));
        addCall(Callback(p).make(f));
      }
      /// Export all properties to the Geant4 UI
      void exportProperties(PropertyManager& mgr);
      /// Accept ne property value from Geant4 UI
      void SetNewValue(G4UIcommand *c, G4String val);
      /// Pass current property value to Geant4 UI
      G4String GetCurrentValue(G4UIcommand *c);
    };

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4UIMESSENGER_H
