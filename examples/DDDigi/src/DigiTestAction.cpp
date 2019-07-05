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
#ifndef DD4HEP_DDDIGI_DIGITESTACTION_H
#define DD4HEP_DDDIGI_DIGITESTACTION_H

// Framework include files
#include "DDDigi/DigiAction.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    // Forward declarations
    class DigiAction;
    class DigiTestAction;

    /// Class which applies random noise hits of a given amplitude
    /**
     *  Class which applies random noise hits of a given amplitude
     *  to a segmented sensitive element.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiTestAction : public DigiAction {
    protected:
      /// Sleep period to fake execution [milliseconds]
      int m_sleep = 0;
    protected:
      /// Inhibit copy constructor
      DigiTestAction() = default;
      /// Inhibit copy constructor
      DigiTestAction(const DigiTestAction& copy) = delete;
      /// Inhibit assignment operator
      DigiTestAction& operator=(const DigiTestAction& copy) = delete;

    public:
      /// Standard constructor
      DigiTestAction(const DigiKernel& kernel, const std::string& nam);
      /// Default destructor
      virtual ~DigiTestAction();
      /// Callback to read event input
      virtual void execute(DigiContext& context)  const override;
    };

  }    // End namespace digi
}      // End namespace dd4hep
#endif // DD4HEP_DDDIGI_DIGITESTACTION_H

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

// Framework include files
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DDDigi/DigiFactories.h"
//#include "DDDigi/DigiTestAction.h"

// C/C++ include files

#ifdef __APPLE__
static void noop(int) {}
#define usleep(x)  noop(x)
#endif

using namespace std;
using namespace dd4hep::digi;

DECLARE_DIGIACTION_NS(dd4hep::digi,DigiTestAction)

/// Standard constructor
DigiTestAction::DigiTestAction(const DigiKernel& kernel, const string& nam)
  : DigiAction(kernel, nam)
{
  declareProperty("sleep", m_sleep = 0);
  InstanceCount::increment(this);
}

/// Default destructor
DigiTestAction::~DigiTestAction() {
  InstanceCount::decrement(this);
}

/// Pre-track action callback
void DigiTestAction::execute(DigiContext& context)  const   {
  debug("+++ Event: %8d (DigiTestAction)  %d msec",
       context.event().eventNumber, m_sleep);
  if ( m_sleep > 0 ) ::usleep(1000*m_sleep);
}
