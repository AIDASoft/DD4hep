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
#ifndef JSON_DETECTOR_H
#define JSON_DETECTOR_H

// Framework include files
#include "JSON/Dimension.h"

/// Include the implementation in the proper namespace
#define DD4HEP_DIMENSION_NS json
#include "Parsers/detail/Detector.h"
#undef DD4HEP_DIMENSION_NS

#endif // JSON_DETECTOR_H
