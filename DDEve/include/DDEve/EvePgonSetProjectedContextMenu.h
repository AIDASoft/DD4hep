// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDEVE_EVEPGONSETPROJECTEDCONTEXTMENU_H
#define DD4HEP_DDEVE_EVEPGONSETPROJECTEDCONTEXTMENU_H

// Framework include files
#include "DDEve/EveUserContextMenu.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /** @class EvePgonSetProjectedContextMenu  EvePgonSetProjectedContextMenu.h DDEve/EvePgonSetProjectedContextMenu.h
   *
   * @author  M.Frank
   * @version 1.0
   */
  class EvePgonSetProjectedContextMenu : public EveUserContextMenu  {
  protected:
    /// Initializing constructor
    EvePgonSetProjectedContextMenu(Display* m);
    /// Default destructor
    virtual ~EvePgonSetProjectedContextMenu();
  public:
    /// Instantiator
    static EvePgonSetProjectedContextMenu& install(Display* m);
    /// Root implementation macro
    ClassDef(EvePgonSetProjectedContextMenu,0);
  };


} /* End namespace DD4hep   */


#endif /* DD4HEP_DDEVE_EVEPGONSETPROJECTEDCONTEXTMENU_H */
