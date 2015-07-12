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

#include "TEveManager.h"
#include <iostream>
#include "TEveStraightLineSet.h"
#include "TRandom.h"

TEveStraightLineSet* lineset(Int_t nlines = 40, Int_t nmarkers = 4) ;

void next_event(){

  static int count = 1 ;

  std::cout <<  " next_event called - just draw some random lines ... " << std::endl ;

  TEveElementList* tevent = (TEveElementList* ) gEve->GetCurrentEvent() ;
  
  if( tevent ) 
    tevent->DestroyElements() ;
  
  
  lineset( count, count ) ;
  
  gEve->Redraw3D();
  
  count += 3 ;
}


//=====================================================================================
TEveStraightLineSet* lineset(Int_t nlines, Int_t nmarkers )
{
  TEveManager::Create();

  TRandom r(0);
  Float_t s = 100;

  TEveStraightLineSet* ls = new TEveStraightLineSet();

  for(Int_t i = 0; i<nlines; i++)
  {
    ls->AddLine( r.Uniform(-s,s), r.Uniform(-s,s), r.Uniform(-s,s),
                 r.Uniform(-s,s), r.Uniform(-s,s), r.Uniform(-s,s));
    // add random number of markers
    Int_t nm = Int_t(nmarkers* r.Rndm());
    for(Int_t m = 0; m < nm; m++) {
      ls->AddMarker(i, r.Rndm());
    }
  }

  ls->SetMarkerSize(1.5);
  ls->SetMarkerStyle(4);

  gEve->AddElement(ls);
  gEve->Redraw3D();

  return ls;
}
//=====================================================================================
