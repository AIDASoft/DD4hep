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
#include "XML/XMLDetector.h"
#include "XML/XMLDimension.inl"
#include "XML/XMLChildValue.inl"

using namespace DD4hep::XML;

// Instantiate here the concrete implementations
#define DD4HEP_DIMENSION_NS XML
#include "XML/detail/Detector.imp"
#include "XML/detail/Dimension.imp"
#include "XML/detail/ChildValue.imp"
