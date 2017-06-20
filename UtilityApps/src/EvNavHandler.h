//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================

#ifndef EvNavHandler_h
#define EvNavHandler_h


void next_event() ;

class EvNavHandler {
public:
  void Fwd() {
    next_event();
  }
  void Bck() {}
};


#endif


