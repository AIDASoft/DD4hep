// $Id: Geant4Field.cpp 875 2013-11-04 16:15:14Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4CALL_H
#define DD4HEP_DDG4_GEANT4CALL_H

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    /// Callback interface class with argument
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class  Geant4Call   {
    public:
      /// Default destructor
      virtual ~Geant4Call();
      /// Default callback with argument
      virtual void operator()(void* param) = 0;
    };

  }    // End namespace Simulation
}      // End namespace DD4hep
#endif // DD4HEP_DDG4_GEANT4CALL_H
