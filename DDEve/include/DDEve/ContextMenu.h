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
#ifndef DDEVE_CONTEXTMENU_H
#define DDEVE_CONTEXTMENU_H

// Framework include files
#include "DD4hep/Callback.h"

// ROOT include files
#include "TClass.h"

// C/C++ include files
#include <string>

/*
 *   dd4hep namespace declaration
 */
namespace dd4hep {

  /// DD4hep specific handler for TEve context menus
  /** @class ContextMenuHandler  ContextMenu.h DDEve/ContextMenu.h
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_EVE
   */
  class ContextMenuHandler : public TObject {
  protected:
    /// User callback
    Callback m_call;
    /// User parameter
    void* m_param;
  public:
    /// Initializing constructor
    ContextMenuHandler(Callback cb, void* param);
    /// Default destructor
    virtual ~ContextMenuHandler();
    /// User parameter hook
    void *param() const { return m_param; }
    /// Callback
    void Context(TObject* target);
    /// Root implementation macro
    ClassDefOverride(ContextMenuHandler,0);    
  };


  /// DD4hep specific TEve context menu
  /** @class ContextMenu  ContextMenu.h DDEve/ContextMenu.h
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_EVE
   */
  class ContextMenu  {
    typedef std::vector<ContextMenuHandler*> Handlers;
    /// Callback map
    Handlers m_calls;
    /// List of menu entries
    TClass* m_class;

    /// Initializing constructor
    ContextMenu(TClass* cl);
    /// Default destructor
    virtual ~ContextMenu();
    /// Add user callback 
    ContextMenu& Add(const std::string& title, Callback cb, void* ud);
  public:
    /// Instantiator
    static ContextMenu& instance(TClass* cl);
    /// Clear all existing items
    ContextMenu& Clear();
    /// Add a separator
    ContextMenu& AddSeparator();
    /// Add a user item to the menu
    template <typename Q,typename T> 
    ContextMenu& Add(const std::string& title, Q* ptr, void (T::*pmf)(TObject*, void* ud), void* ud=0)   {
      return Add(title, Callback(ptr).make(pmf), ud);
    }
    /// Root implementation macro
    ClassDef(ContextMenu,0);
  };


} /* End namespace dd4hep   */


#endif // DDEVE_CONTEXTMENU_H
