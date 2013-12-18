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

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    /** @class Geant4Call Geant4Call.h DDG4/Geant4Call.h
     *
     * Callback interface class with argument
     *
     * @author  M.Frank
     * @version 1.0
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
