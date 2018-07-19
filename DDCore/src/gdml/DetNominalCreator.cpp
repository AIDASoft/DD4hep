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
//
// Specialized generic detector constructor
// 
//==========================================================================
#ifndef DD4HEP_DETNOMINALCREATOR_H
#define DD4HEP_DETNOMINALCREATOR_H

// Framework include files
#include "DD4hep/DetElement.h"

namespace dd4hep {
  
  /// DD4hep DetElement creator for the CMS geometry.
  /*  Heuristically assign DetElement structures to the sensitive volume pathes.
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CORE
   */
  class DetNominalCreator  {
    Detector& description;
  public:
    DetNominalCreator(Detector& d) : description(d) {}
    DetNominalCreator(const DetNominalCreator& c) : description(c.description) {}
    virtual ~DetNominalCreator() = default;
    int operator()(DetElement de, int /* level */)  const  {
      if ( de.nominal().isValid() )  return 1;
      return 0;
    }
  };
}
#endif   /* DD4HEP_DETNOMINALCREATOR_H  */

// Framework include files
#include "DD4hep/DetectorProcessor.h"
#include "DD4hep/DetFactoryHelper.h"

// C/C++ include files


using namespace std;
using namespace dd4hep;

static void* create_object(Detector& description, int argc, char** argv)   {
  for(int i = 0; i < argc && argv[i]; ++i)  {  }
  shared_ptr<DetNominalCreator> obj(new DetNominalCreator(description));
  DetectorProcessorShared<DetNominalCreator>* proc =
    new DetectorProcessorShared<DetNominalCreator>(obj);
  return (void*)proc;
}

// first argument is the type from the xml file
DECLARE_DD4HEP_CONSTRUCTOR(DD4hep_DetNominalCreator,create_object)
