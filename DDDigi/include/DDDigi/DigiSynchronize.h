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
#ifndef DDDIGI_DIGISYNCHRONIZE_H
#define DDDIGI_DIGISYNCHRONIZE_H

// Framework incloude files
#include "DDDigi/DigiEventAction.h"

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
    class DigiSynchronize : public DigiEventAction {
    protected:
      /// The list of action objects to be called
      Actors<DigiEventAction> m_actors;

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiSynchronize);

    public:
      /// Standard constructor
      DigiSynchronize(const DigiKernel& kernel, const std::string& nam);
      /// Default destructor
      virtual ~DigiSynchronize();
      /// Get an action member by name
      DigiEventAction* get(const std::string& name) const;
      /// Access the children
      const Actors<DigiEventAction>& children()   const   {
        return m_actors;
      }
      /// Adopt a new action as part of the sequence. Sequence takes ownership.
      void adopt(DigiEventAction* action);
      /// Begin-of-event callback
      virtual void execute(DigiContext& context)  const override;
      ///
      void analyze();
    };

  }    // End namespace digi
}      // End namespace dd4hep

#endif // DDDIGI_DIGISYNCHRONIZE_H
