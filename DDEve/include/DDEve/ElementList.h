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
#ifndef DDEVE_ELEMENTLIST_H
#define DDEVE_ELEMENTLIST_H

// Framework include files
#include "DDEve/View.h"
#include "DDEve/EveUserContextMenu.h"

// ROOT include files
#include "TEveElement.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Local implementation with overrides of the TEveElementList
  /*
   *  I do not want to override the TEveElementList context menu....
   *  Hence a new class is needed.
   *  
   * \author  M.Frank
   * \version 1.0
   * \ingroup DD4HEP_EVE
   */
  class ElementList : public TEveElementList  {
  public:
    /// Default constructor
    ElementList();
    /// Initializing constructor
    ElementList(const std::string& name, const std::string& title="", bool doCol=false, bool doTrans=false);
    /// Copy constructor
    ElementList(const ElementList& l);
    /// Default destructor
    virtual ~ElementList();
    /// Clone object: Overload from TEveElementList
    virtual TEveElementList* CloneElement()  const  override;
    /// Root implementation macro
    ClassDefOverride(ElementList,0);
  };

  /// DDEve context menu class
  /*
   * \author  M.Frank
   * \version 1.0
   * \ingroup DD4HEP_EVE
   */
  class ElementListContextMenu : public EveUserContextMenu  {
  protected:
    /// Initializing constructor
    ElementListContextMenu(Display* m);
    /// Default destructor
    virtual ~ElementListContextMenu();
  public:
    /// Instantiator
    static ElementListContextMenu& install(Display* m);
    /// Root implementation macro
    ClassDef(ElementListContextMenu,0);
  };

}      /* End namespace dd4hep       */
#endif // DDEVE_ELEMENTLIST_H
