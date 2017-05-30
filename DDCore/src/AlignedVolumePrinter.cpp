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
#include "DD4hep/AlignmentsPrinter.h"
#include "DD4hep/AlignedVolumePrinter.h"

using namespace DD4hep;
using namespace DD4hep::Alignments;

/// Initializing constructor
AlignedVolumePrinter::AlignedVolumePrinter(ConditionsMap* m, const std::string& pref,int flg)
  : AlignmentsProcessor(m), name("Alignment"), prefix(pref), printLevel(INFO), m_flag(flg)
{
}

/// Callback to output alignments information
int AlignedVolumePrinter::operator()(Alignment a)    {
  printAlignment(printLevel, name, a);
  return 1;
}

/// Callback to output alignments information of an entire DetElement
int AlignedVolumePrinter::processElement(DetElement de)  {
  if ( de.isValid() )  {
    if ( mapping )   {
      printElementPlacement(printLevel, name, de, *mapping);
      return 1;
    }
    except("Alignments","Failed to process alignments for DetElement:%s [No slice availible]",
           de.path().c_str());
  }
  except("Alignments","Cannot process alignments of an invalid detector element");
  return 0;
}
