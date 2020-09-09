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
#ifndef DDDIGI_DIGIINPUTACTION_H
#define DDDIGI_DIGIINPUTACTION_H

/// Framework include files
#include "DDDigi/DigiEventAction.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    // Forward declarations
    class DigiAction;
    class DigiInputAction;

    /// Base class for input actions to the digitization
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiInputAction : public DigiEventAction {
    protected:
      /// Input data specification
      std::vector<std::string> m_input;
    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiInputAction);

    public:
      /// Standard constructor
      DigiInputAction(const DigiKernel& kernel, const std::string& nam);
      /// Default destructor
      virtual ~DigiInputAction();
      /// Callback to read event input
      virtual void execute(DigiContext& context)  const override;
    };

  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGIINPUTACTION_H
