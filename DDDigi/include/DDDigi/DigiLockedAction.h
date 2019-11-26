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
#ifndef DD4HEP_DDDIGI_DIGILOCKEDACTION_H
#define DD4HEP_DDDIGI_DIGILOCKEDACTION_H

/// Framework include files
#include "DDDigi/DigiEventAction.h"

/// C/C++ include files
#include <mutex>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    // Forward declarations
    class DigiAction;
    class DigiLockedAction;

    /// Class to execute non-reentrant subaction in wrapped mode
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiLockedAction : public DigiEventAction {
    protected:
      /// Action lock to inhibit calling the non-reentrant underlying
      std::mutex  m_lock;
      /// Reference to underlying action
      DigiEventAction* m_action = 0;

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiLockedAction);
      
    public:
      /// Standard constructor
      DigiLockedAction(const DigiKernel& kernel, const std::string& nam);
      /// Default destructor
      virtual ~DigiLockedAction();
      /// Underlying object to be used during the locked execution
      void use(DigiEventAction* action);
      /// Callback to read event locked
      virtual void execute(DigiContext& context)  const override;
    };

  }    // End namespace digi
}      // End namespace dd4hep
#endif // DD4HEP_DDDIGI_DIGILOCKEDACTION_H
