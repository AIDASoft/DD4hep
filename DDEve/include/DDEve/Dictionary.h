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
#ifndef DD4HEP_DDEVE_DICTIONARY_H
#define DD4HEP_DDEVE_DICTIONARY_H

// Framework include files
#include "DDEve/Display.h"
#include "DDEve/EventControl.h"
#include "DDEve/EventHandler.h"
#include "DDEve/View3D.h"
#include "DDEve/RhoPhiProjection.h"
#include "DDEve/RhoZProjection.h"
#include "DDEve/CaloLego.h"
#include "DDEve/Calo2DProjection.h"
#include "DDEve/Calo3DProjection.h"
#include "DDEve/MultiView.h"

#include "DDEve/ContextMenu.h"
#include "DDEve/ViewMenu.h"
#include "DDEve/Annotation.h"
#include "DDEve/DD4hepMenu.h"
#include "DDEve/ElementList.h"
#include "DDEve/EveShapeContextMenu.h"
#include "DDEve/EvePgonSetProjectedContextMenu.h"
#include "DDEve/DisplayConfiguration.h"


#include "DDEve/GenericEventHandler.h"
#include "DDEve/DDG4EventHandler.h"

namespace dd4hep {
  void EveDisplay(const char* xmlFile, const char* eventFileName);
  struct DDEve {
    static void run(const char* xmlFile)  {
      EveDisplay(xmlFile, nullptr);
    }
  };
}

#if defined(__CINT__) || defined(__MAKECINT__) || defined(__CLING__) || defined(__ROOTCLING__)
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace dd4hep;

#pragma link C++ function dd4hep::EveDisplay(const char* xmlFile, const char* eventFileName);


#pragma link C++ class dd4hep::DDEve;
#pragma link C++ class dd4hep::Display;
#pragma link C++ class dd4hep::Annotation;
#pragma link C++ class dd4hep::ElementList;
#pragma link C++ class dd4hep::FrameControl;
#pragma link C++ class dd4hep::EventHandler;
#pragma link C++ class dd4hep::EventConsumer;
#pragma link C++ class dd4hep::DDG4EventHandler;
#pragma link C++ class dd4hep::GenericEventHandler;
#pragma link C++ class dd4hep::EventControl;

#pragma link C++ class dd4hep::DisplayConfiguration::Config;
//#pragma link C++ class std::list<dd4hep::DisplayConfiguration::Config>;
#pragma link C++ class dd4hep::DisplayConfiguration::ViewConfig;
//#pragma link C++ class std::list<dd4hep::DisplayConfiguration::ViewConfig>;
#pragma link C++ class dd4hep::DisplayConfiguration;

#pragma link C++ class dd4hep::View;
#pragma link C++ class dd4hep::View3D;
#pragma link C++ class dd4hep::Projection;
#pragma link C++ class dd4hep::RhoZProjection;
#pragma link C++ class dd4hep::RhoPhiProjection;
#pragma link C++ class dd4hep::CaloLego;
#pragma link C++ class dd4hep::Calo2DProjection;
#pragma link C++ class dd4hep::Calo3DProjection;
#pragma link C++ class dd4hep::MultiView;

#pragma link C++ class dd4hep::PopupMenu;
#pragma link C++ class dd4hep::ViewMenu;
#pragma link C++ class dd4hep::dd4hepMenu;
#pragma link C++ class dd4hep::ContextMenu;
#pragma link C++ class dd4hep::ContextMenuHandler;
#pragma link C++ class dd4hep::EveUserContextMenu;
#pragma link C++ class dd4hep::ElementListContextMenu;
#pragma link C++ class dd4hep::EveShapeContextMenu;
#pragma link C++ class dd4hep::EvePgonSetProjectedContextMenu;
//#pragma link C++ class dd4hep::;

#endif

#endif /* DD4HEP_DDEVE_DICTIONARY_H */
