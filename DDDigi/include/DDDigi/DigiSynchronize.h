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
#ifndef DD4HEP_DDDIGI_DIGISYNCHRONIZE_H
#define DD4HEP_DDDIGI_DIGISYNCHRONIZE_H

// Framework incloude files
#include "DDDigi/DigiAction.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  // Forward declarations

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// 
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiSynchronize : public DigiAction {
    protected:
      /// The list of action objects to be called
      Actors<DigiAction> m_actors;

    public:
      /// Standard constructor
      DigiSynchronize(const DigiKernel& kernel, const std::string& nam);
      /// Default destructor
      virtual ~DigiSynchronize();
      /// Get an action member by name
      DigiAction* get(const std::string& name) const;
      /// Access the children
      const Actors<DigiAction>& children()   const   {
        return m_actors;
      }
      /// Adopt a new action as part of the sequence. Sequence takes ownership.
      void adopt(DigiAction* action);
      /// Begin-of-event callback
      virtual void execute(DigiContext& context)  const override;
      ///
      void analyze();
    };

  }    // End namespace digi
}      // End namespace dd4hep

#endif // DD4HEP_DDDIGI_DIGISYNCHRONIZE_H
