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
// DDCMS is a detector description convention developed by the CMS experiment.
//
//==========================================================================

// Framework include files
#include "XML/XMLTags.h"

// Define unicode tags
#define UNICODE(x)  extern const ::dd4hep::xml::Tag_t Unicode_##x ( #x )
#include "DDCMS/DDCMSTags.h"
