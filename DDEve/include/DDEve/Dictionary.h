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
#include "DDEve/ParticleActors.h"
#include "DDEve/HitActors.h"
#include "DDEve/DD4hepMenu.h"

#include "DDEve/GenericEventHandler.h"
#include "DDEve/DDG4EventHandler.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  void EveDisplay(const char* xmlFile, const char* eventFileName);

  /// ROOT interactive interface class for running DDEve
  /* 
   * \author  M.Frank
   * \version 1.0
   * \ingroup DD4HEP_EVE
   */
  struct DDEve {
    /// ROOT interactive entry point for running DDEve
    static void run(const char* xmlFile)  {
      EveDisplay(xmlFile, nullptr);
    }
  };
}

/// Ugly hack to get rid of some rootcling warnings!
namespace  {
  class Dictionary {};
  class DDEveEventData {};
}

#if defined(__CINT__) || defined(__MAKECINT__) || defined(__CLING__) || defined(__ROOTCLING__)
using namespace dd4hep;
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace dd4hep;

//==================================================================================
#pragma link C++ function EveDisplay(const char* xmlFile, const char* eventFileName);

#pragma link C++ class DDEve;
#pragma link C++ class Display;
#pragma link C++ class Annotation;
#pragma link C++ class ElementList;
#pragma link C++ class FrameControl;
#pragma link C++ class EventHandler;
#pragma link C++ class EventConsumer;
#pragma link C++ class DDG4EventHandler;
#pragma link C++ class GenericEventHandler;
#pragma link C++ class EventControl;

#pragma link C++ class DisplayConfiguration::Config;
//#pragma link C++ class std::list<DisplayConfiguration::Config>;
#pragma link C++ class DisplayConfiguration::ViewConfig;
//#pragma link C++ class std::list<DisplayConfiguration::ViewConfig>;
#pragma link C++ class DisplayConfiguration;

#pragma link C++ class View;
#pragma link C++ class View3D;
#pragma link C++ class Projection;
#pragma link C++ class RhoZProjection;
#pragma link C++ class RhoPhiProjection;
#pragma link C++ class CaloLego;
#pragma link C++ class Calo2DProjection;
#pragma link C++ class Calo3DProjection;
#pragma link C++ class MultiView;

#pragma link C++ class PopupMenu;
#pragma link C++ class ViewMenu;
#pragma link C++ class DD4hepMenu;
#pragma link C++ class ContextMenu;
#pragma link C++ class ContextMenuHandler;
#pragma link C++ class EveUserContextMenu;
#pragma link C++ class ElementListContextMenu;
#pragma link C++ class EveShapeContextMenu;
#pragma link C++ class EvePgonSetProjectedContextMenu;


#endif

#endif /* DD4HEP_DDEVE_DICTIONARY_H */
