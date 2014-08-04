// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//  Original Author: Matevz Tadel 2009 (MultiView.C)
//
//====================================================================
#ifndef DD4HEP_DDEVE_ELEMENTLIST_H
#define DD4HEP_DDEVE_ELEMENTLIST_H

// Framework include files
#include "DDEve/View.h"
#include "DDEve/EveUserContextMenu.h"

// ROOT include files
#include "TEveElement.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /** @class ElementList  ElementList.h DDEve/ElementList.h
   *
   *  I do not want to override the TEveElementList context menu....
   *  Hence a new class is needed.
   *  
   * @author  M.Frank
   * @version 1.0
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
    /// Clone object
    TEveElement* CloneElement();
    /// Root implementation macro
    ClassDef(ElementList,0);
  };

  /** @class ElementListContextMenu  ElementListContextMenu.h DDEve/ElementListContextMenu.h
   *
   * @author  M.Frank
   * @version 1.0
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

} /* End namespace DD4hep   */


#endif /* DD4HEP_DDEVE_ELEMENTLIST_H */
