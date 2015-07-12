// $Id: run_plugin.h 1663 2015-03-20 13:54:53Z gaede $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include "DD4hep/Printout.h"
#include "DDEve/Projection.h"
#include "DDEve/Display.h"
#include "DDEve/Utilities.h"

// Eve include files
#include <TEveManager.h>
#include <TEveBrowser.h>
#include <TEveWindow.h>
#include <TGLViewer.h>

using namespace std;
using namespace DD4hep;

/// Initializing constructor
Projection::Projection(Display* eve, const string& nam)
  : View(eve, nam), m_projMgr(0), m_axis(0)
{
}

/// Default destructor
Projection::~Projection() {
}

void Projection::SetDepth(Float_t d)  {
  // Set current depth on all projection managers.
  m_projMgr->SetCurrentDepth(d);
}

/// Call an element to a geometry element list
TEveElement* Projection::ImportGeoTopic(TEveElement*, TEveElementList*)   {
  return 0;
}

/// Call an element to a event element list
TEveElement* Projection::ImportElement(TEveElement* el, TEveElementList* list)  {
  TEveElementList* unprojected = &GetGeoTopic("Unprojected");
  for(Topics::iterator i=m_geoTopics.begin(); i!=m_geoTopics.end(); ++i)  {
    if ( el == unprojected )   {
      m_projMgr->AddElement(el);
      return 0;
    }
  }
  TEveElement* e = m_projMgr->ImportElements(el, list);
  printout(INFO,"Projection","ImportElement %s [%s] into list: %s Projectable:%s [%p]",
           Utilities::GetName(el),el->IsA()->GetName(),list->GetName(),
           dynamic_cast<TEveProjectable*>(list) ? "true" : "false", e);

  unprojected->AddElement(el);
  if ( list != m_geoScene && list != m_eveScene )   {
    TEveElement::List_ci ci = std::find(m_geoScene->BeginChildren(),m_geoScene->EndChildren(),list);
    if ( ci == m_geoScene->EndChildren() )  {
      m_geoScene->AddElement(list);
    }
  }
  return e;
}

/// Call an element to a geometry element list
TEveElement* Projection::ImportGeoElement(TEveElement* element, TEveElementList* list)  {
  return element ? ImportElement(element, list) : 0;
}

/// Call an element to a event element list
TEveElement* Projection::ImportEventElement(TEveElement* element, TEveElementList* list)  {
  return element ? ImportElement(element, list) : 0;
}

/// Add axis to the projection view
Projection& Projection::AddAxis()    {
  TEveProjectionAxes* a = new TEveProjectionAxes(m_projMgr);
  a->SetMainColor(kWhite);
  a->SetTitle("R-Phi");
  a->SetTitleSize(0.05);
  a->SetTitleFont(102);
  a->SetLabelSize(0.025);
  a->SetLabelFont(102);
  m_geoScene->AddElement(a);
  m_axis = a;
  return *this;
}

/// Create Rho-Phi projection 
Projection& Projection::CreateRhoPhiProjection()  {
  // RhoPhi Projection manager
  m_projMgr = new TEveProjectionManager(TEveProjection::kPT_RPhi);
  //m_ev.AddToListTree(m_projMgr, kFALSE);
  AddToGlobalItems(name())->AddElement(m_projMgr);
  return *this;
}

/// Create Rho-Z projection
Projection& Projection::CreateRhoZProjection()  {
  // RhoZ Projection manager
  m_projMgr = new TEveProjectionManager(TEveProjection::kPT_RhoZ);
  //m_eve->manager().AddToListTree(m_projMgr, kFALSE);
  AddToGlobalItems(name())->AddElement(m_projMgr);
  return *this;
}

/// Map the projection view to the slot
View& Projection::Map(TEveWindow* slot)  {
  View::Map(slot);
  m_view->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
  return *this;
}
