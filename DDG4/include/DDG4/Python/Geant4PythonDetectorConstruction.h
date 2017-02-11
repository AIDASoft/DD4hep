//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author Markus Frank
//  \date   2015-11-03
//
//==========================================================================
#ifndef DD4HEP_DDG4_GEANT4PYTHONDETECTORCONSTRUCTION_H
#define DD4HEP_DDG4_GEANT4PYTHONDETECTORCONSTRUCTION_H

// Framework include files
#include "DDG4/Geant4DetectorConstruction.h"
#include "DDG4/Python/Geant4PythonCall.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    /// Base class to initialize a multi-threaded or single threaded Geant4 application
    /**
     *  Geant4PythonDetectorConstruction is the corresponding 
     *  python action to populate the detector construction sequencer.
     *  and supports three ccallbacks:
     *
     *      // Set the Detector initialization command
     *      void setConstructGeo(PyObject* callable, PyObject* args);
     *      // Set the field initialization command
     *      void setConstructField(PyObject* callable, PyObject* args);
     *      // Set the sensitive detector initialization command
     *      void setConstructSensitives(PyObject* callable, PyObject* args);
     *
     *  to be used in python as call sequence within the master thread:
     *
     *    init_seq = self.master().detectorConstruction(True)
     *    init_action = DetectorConstruction(self.master(),name_type)
     *    init_action.setConstructGeo(geometry_setup_call, < geometry_args > )
     *    init_action.setConstructField(field_setup_call, < field_args > )
     *    init_action.setConstructSensitives(sensitives_setup_call, < sensitives_args >)
     *    init_seq.adopt(init_action)
     *
     *  If any of the three callback is not set, the corresponding actiion is not executed.
     *  Hereby are geometry_setup_call, field_setup_call and sensitives\_setup\_call
     *  the callable objects to configure the geometry, the tracking field 
     *  and the sensitive detectors.
     *  < geometry_args >, < field_args > and < sensitives\_args > are 
     *  the corresponding callable arguments in the form of a python tuple object.   
     *
     *  All python callbacks are supposed to return the integer '1' on success.
     *  Any other return code is assumed to be failure.
     * 
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4PythonDetectorConstruction : public Geant4DetectorConstruction  {
    protected:
      /// Sensitive detector initialization command. Default: empty
      Geant4PythonCall m_constructSD;
      /// Field initialization command. Default: empty
      Geant4PythonCall m_constructFLD;
      /// Geometry initialization command. Default: empty
      Geant4PythonCall m_constructGEO;
      /// Execute command in the python interpreter
      void exec(const std::string& desc, const Geant4PythonCall& cmd)  const;

    public:
      /// Standard constructor
      Geant4PythonDetectorConstruction(Geant4Context* c, const std::string& nam);
      /// Default destructor
      virtual ~Geant4PythonDetectorConstruction() {}
      /// Set the Detector initialization command
      void setConstructGeo(PyObject* callable, PyObject* args);
      /// Set the field initialization command
      void setConstructField(PyObject* callable, PyObject* args);
      /// Set the sensitive detector initialization command
      void setConstructSensitives(PyObject* callable, PyObject* args);
      /// Geometry construction callback. Called at "Construct()"
      virtual void constructGeo(Geant4DetectorConstructionContext* ctxt);
      /// Electromagnetic field construction callback. Called at "ConstructSDandField()"
      virtual void constructField(Geant4DetectorConstructionContext* ctxt);
      /// Sensitive detector construction callback. Called at "ConstructSDandField()"
      virtual void constructSensitives(Geant4DetectorConstructionContext* ctxt);

    };
  }    // End namespace Simulation
}      // End namespace DD4hep
#endif // DD4HEP_DDG4_GEANT4PYTHONDETECTORCONSTRUCTION_H
