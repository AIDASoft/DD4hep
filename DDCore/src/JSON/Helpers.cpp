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

// Framework include files
#include "JSON/Detector.h"
#include "JSON/Dimension.inl"
#include "JSON/ChildValue.inl"

// Instantiate here the concrete implementations
#define DD4HEP_DIMENSION_NS json
using namespace dd4hep::DD4HEP_DIMENSION_NS;

#include "XML/detail/Detector.imp"
#include "XML/detail/Dimension.imp"
#include "XML/detail/ChildValue.imp"
