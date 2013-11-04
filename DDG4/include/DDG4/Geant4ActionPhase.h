// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4ACTIONPHASE_H
#define DD4HEP_DDG4_GEANT4ACTIONPHASE_H

// Framework include files
#include "DDG4/Geant4Action.h"

// C/C++ include files
#include <vector>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation   {

    /*

       Geant4Phase,  G4EventGenerator    --> G4VUserPrimaryGeneratorAction::GeneratePrimaries
       Geant4Begin,  G4Run               --> G4UserRunAction::BeginOfRunAction
       Geant4End,    G4Run               --> G4UserRunAction::EndOfRunAction
       Geant4Begin,  G4Event             --> G4UserEventAction::BeginOfEventAction
       Geant4End,    G4Event             --> G4UserEventAction::EndOfEventAction
       Geant4Begin,  G4Track             --> G4UserTrackingAction::PreUserTrackingAction
       Geant4End,    G4Track             --> G4UserTrackingAction::PostUserTrackingAction
       Geant4Phase,  G4Step              --> G4UserSteppingAction::UserSteppingAction
       Geant4Begin,  G4TrackStack        --> G4UserStackingAction::NewStage
       Geant4End,    G4TrackStack        --> G4UserStackingAction::PrepareNewEvent
       
    */

    /** @class Invoke Geant4ActionPhase.h DDG4/Geant4ActionPhase.h
     * 
     * Default base class for all geant 4 actions and derivates thereof.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4ActionPhase : public Geant4Action  {
    public:
      typedef std::vector<Callback> Members;
    protected:
      /// Phase members (actions) being called for a particular phase
      Members               m_members;
      /// Type information of the argument type of the callback
      const std::type_info* m_argTypes[3];

    public:
      /// Standard constructor
      Geant4ActionPhase(Geant4Context* context, const std::string& name, const std::type_info& arg_type0,
			const std::type_info& arg_type1, const std::type_info& arg_type2);
      /// Default destructor
      virtual ~Geant4ActionPhase();
      /// Access phase members
      const Members& members() const                 {  return m_members;      }
      /// Type of the first phase callback-argument
      const std::type_info* const * argTypes() const {  return m_argTypes;     }
      /// Execute all members in the phase context
      void execute(void* argument);
      /// Add a new member to the phase
      virtual bool add(Callback callback);
      /// Remove an existing member from the phase. If not existing returns false
      virtual bool remove(Callback callback);
      /// Add a new member to the phase
      template <typename TYPE, typename IF_TYPE, typename A0, typename R> 
	bool add(TYPE* member, R (IF_TYPE::*callback)(A0 arg))  {
	typeinfoCheck(typeid(A0),*m_argTypes[0],"Invalid ARG0 type. Failed to add phase callback.");
	if ( dynamic_cast<IF_TYPE*>(member) )  {
	  //member->addRef();
	  return add(Callback(member).make(callback));
	}
	throw unrelated_type_error(typeid(TYPE),typeid(IF_TYPE),"Failed to add phase callback.");
      }
      /// Add a new member to the phase
      template <typename TYPE, typename IF_TYPE, typename A0, typename A1, typename R> 
	bool add(TYPE* member, R (IF_TYPE::*callback)(A0 arg0, A1 arg1))  {
	typeinfoCheck(typeid(A0),*m_argTypes[0],"Invalid ARG0 type. Failed to add phase callback.");
	typeinfoCheck(typeid(A1),*m_argTypes[1],"Invalid ARG1 type. Failed to add phase callback.");
	if ( dynamic_cast<IF_TYPE*>(member) )  {
	  //member->addRef();
	  return add(Callback(member).make(callback));
	}
	throw unrelated_type_error(typeid(TYPE),typeid(IF_TYPE),"Failed to add phase callback.");
      }
      /// Add a new member to the phase
      template <typename TYPE, typename IF_TYPE, typename A0, typename A1, typename A2, typename R> 
	bool add(TYPE* member, R (IF_TYPE::*callback)(A0 arg0, A1 arg1))  {
	typeinfoCheck(typeid(A0),*m_argTypes[0],"Invalid ARG0 type. Failed to add phase callback.");
	typeinfoCheck(typeid(A1),*m_argTypes[1],"Invalid ARG1 type. Failed to add phase callback.");
	typeinfoCheck(typeid(A2),*m_argTypes[2],"Invalid ARG2 type. Failed to add phase callback.");
	if ( dynamic_cast<IF_TYPE*>(member) )  {
	  //member->addRef();
	  return add(Callback(member).make(callback));
	}
	throw unrelated_type_error(typeid(TYPE),typeid(IF_TYPE),"Failed to add phase callback.");
      }
      /// Remove all member callbacks from the phase. If not existing returns false
      template <typename TYPE, typename PMF> bool remove(TYPE* member)  {
	return remove(Callback(member));
      }
      /// Remove an existing member callback from the phase. If not existing returns false
      template <typename TYPE, typename PMF> bool remove(TYPE* member, PMF callback)  {
	Callback cb(member);
	return remove(cb.make(callback));
      }
      /// Create action to execute phase members
      void call();
      template <typename A0> void call(A0 a0);
      template <typename A0, typename A1> void call(A0 a0, A1 a1);
      template <typename A0, typename A1, typename A2> void call(A0 a0, A1 a1, A2 a2);
    };

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4ACTIONPHASE_H
