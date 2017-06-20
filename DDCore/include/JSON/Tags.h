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
#ifndef DD4HEP_JSON_TAGS_H
#define DD4HEP_JSON_TAGS_H

#define DECLARE_UNICODE_TAG(x)  

// Framework include files
#include "JSON/Elements.h"

// Helpers to access tags and attributes quickly without specifying explicitly namespaces
#define _dd4hep_Unicode_Item(a) #a
#define _U(a) #a
#define _Unicode(a) #a

#endif // DD4HEP_JSON_TAGS_H
