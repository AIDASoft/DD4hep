// $Id$
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
#include "DDEve/Annotation.h"
#include "DD4hep/InstanceCount.h"
#include "TEveViewer.h"
#include "TGLViewer.h"

// C/C++ include files

// Forward declarations

using namespace DD4hep;

ClassImp(Annotation)

/// Standard constructor with initialization
Annotation::Annotation(TEveViewer* v, const std::string& text, float x, float y, Color_t c)  
: TGLAnnotation(v->GetGLViewer(), text.c_str(), x, y)
{
  SetTextSize(DefaultTextSize());
  SetUseColorSet(kFALSE);
  SetTextColor(c);
  InstanceCount::increment(this);
}

/// Default destructor
Annotation::~Annotation() {
  InstanceCount::decrement(this);
}

/// Mouse has entered overlay area.
Bool_t Annotation::MouseEnter(TGLOvlSelectRecord& )   {
  fActive = kFALSE;
  return kTRUE;
}

/// Default text size
float Annotation::DefaultTextSize() {
  return 0.04; 
}
/// Default margin for placement in bottom left corner
float Annotation::DefaultMargin()   { 
  return 0.01; 
}
