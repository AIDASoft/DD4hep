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
#include "DD4hep/AlignmentsPrinter.h"
#include "DD4hep/objects/AlignmentsInterna.h"

using namespace DD4hep;
using namespace DD4hep::Alignments;

/// Initializing constructor
AlignmentsPrinter::AlignmentsPrinter(const std::string& pref, int flg)
  : AlignmentsProcessor(0), name("Alignment"), prefix(pref), m_flag(flg)
{
}

/// Callback to output alignments information
int AlignmentsPrinter::operator()(Alignment a)    {
  if ( a.isValid() )   {
    printout(INFO,name,"++ %s%s [%p]",prefix.c_str(),a.name(),a.ptr());
    const Alignments::Delta& D = a.data().delta;
    std::string new_prefix = prefix;
    new_prefix.assign(prefix.length(),' ');
    printout(INFO,name,"++ %s \tPath:%s",
             new_prefix.c_str(), a.name());
    printout(INFO,name,"++ %s \tData:(%11s-%8s-%5s) Cond:%p 'Alignment'",
             new_prefix.c_str(), 
             D.hasTranslation() ? "Translation" : "",
             D.hasRotation() ? "Rotation" : "",
             D.hasPivot() ? "Pivot" : "",
             a.data().hasCondition() ? a.data().condition.ptr() : 0);
  }
  return 1;
}

/// Container callback for object processing
int AlignmentsPrinter::operator()(Container container)   {
  if ( m_pool )  {
    printout(INFO,name,"++ Alignments of DE %s [%d entries]",
             container->detector.path().c_str(), int(container.keys().size()));
    for(const auto& k : container.keys() )  {
      try {
        Alignment align = container.get(k.first,*m_pool);
        std::string an = align.name();
        std::string cn = an.substr(an.find('#')+1);
        Alignment::key_type key = hash32(cn);
        printout(INFO,name,"++ %s %s %s [%08X] -> %s [%08X]",
                 prefix.c_str(), "Alignment:", an.c_str(), key==k.first ? key : k.first,
                 align.name(), k.second.first);
        (*this)(align);
      }
      catch(...)  {
        printout(ERROR,name,"++ %s %s [%08X] -> [%08X]",
                 prefix.c_str(), "FAILED Alignment:", k.first, k.second.first);
      }
    }
    return 1;
  }
  except(name,"Cannot dump alignments container without user-pool.");
  return 0;
}
