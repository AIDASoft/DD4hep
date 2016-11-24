//==========================================================================
//  AIDA Detector description implementation for LCD
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

// Framework includes
#include "DD4hep/Printout.h"
#include "DD4hep/DetAlign.h"
#include "DD4hep/AlignmentsProcessor.h"

using namespace DD4hep;
using namespace DD4hep::Alignments;

/// Callback to output alignments information
int AlignmentsProcessor::operator()(Alignment /* alignment */)    {
  return 1;
}

/// Container callback for object processing
int AlignmentsProcessor::operator()(Container container)   {
  if ( m_pool )  {
    for(const auto& k : container.keys() )  {
      Alignment a = container.get(k.first,*m_pool);
      (*this)(a);
    }
    return 1;
  }
  except("Alignments","Cannot process alignments container without user-pool.");
  return 0;
}

/// Callback to output alignments information of an entire DetElement
int AlignmentsProcessor::processElement(Geometry::DetElement de)    {
  if ( de.isValid() )  {
    if ( m_pool )  {
      DetAlign align(de);
      (*this)(align.alignments());
      return 1;
    }
    except("Alignments","Cannot process DetElement alignments from '%s' without user-pool",de.name());
  }
  except("Alignments","Cannot process alignments of an invalid detector element");
  return 0;
}
