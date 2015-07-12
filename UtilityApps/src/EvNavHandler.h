// $Id: Handle.h 570 2013-05-17 07:47:11Z markus.frank $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
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


