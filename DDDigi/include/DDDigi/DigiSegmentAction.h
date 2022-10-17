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
#ifndef DDDIGI_DIGISEGMENTACTION_H
#define DDDIGI_DIGISEGMENTACTION_H

// Framework include files
#include <DDDigi/DigiData.h>
#include <DDDigi/DigiEventAction.h>
#include <DDDigi/DigiSegmentationTool.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Forward declarations
    class DigiSegmentAction;
    class DigiSegmentContext;
    class DigiSegmentSplitter;

    /// Default base class for all Digitizer actions and derivates thereof.
    /**
     *  This is a utility class supporting properties, output and access to
     *  event and run objects through the context.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class DigiSegmentAction : virtual public DigiAction   {
    protected:
      friend class DigiSegmentSplitter;

      /// Segmentation split context
      DigiSegmentContext    segment  { };
  
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiSegmentAction);

    public:
      /// Standard constructor
      DigiSegmentAction(const DigiKernel& kernel, const std::string& name);
      /// Default destructor
      virtual ~DigiSegmentAction();
      /// Main functional callback. Default implementnation is noop.
      virtual DepositVector handleSegment(DigiContext&              context,
					  const DepositMapping&     deposits)  const;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGISEGMENTACTION_H
