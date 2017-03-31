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

// Framework include files
#include "XML/XMLTags.h"

// Define unicode tags
#ifndef UNICODE
#define UNICODE(x)  extern const ::DD4hep::XML::Tag_t Unicode_##x
#endif
#include "DDAlign/AlignmentTags.h"
