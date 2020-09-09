//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================
#ifndef DDDIGI_DIGIEVENTACTION_H
#define DDDIGI_DIGIEVENTACTION_H

// Framework include files
#include "DDDigi/DigiAction.h"


/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Default base class for all Digitizer actions and derivates thereof.
    /**
     *  This is a utility class supporting properties, output and access to
     *  event and run objects through the context.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class DigiEventAction : public DigiAction   {
      friend class DigiKernel;

    protected:
      /// Property: Support parallel execution
      bool               m_parallel    = false;

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiEventAction);

      /// Default destructor
      virtual ~DigiEventAction();

    public:
      /// Standard constructor
      DigiEventAction(const DigiKernel& kernel, const std::string& nam);
      /// Access parallization flag
      bool executeParallel()  const  {
        return m_parallel;
      }      
      /// Set the parallization flag; returns previous value
      bool setExecuteParallel(bool new_value);
      /// Main functional callback
      virtual void execute(DigiContext& context)   const = 0;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGIEVENTACTION_H
