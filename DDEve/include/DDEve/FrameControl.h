//==========================================================================
//  AIDA Detector description implementation for LCD
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
#ifndef DD4HEP_DDEVE_FRAMECONTROL_H
#define DD4HEP_DDEVE_FRAMECONTROL_H

// ROOT include files
#include "TGFrame.h"

// C/C++ include files
#include <string>

// Forward declarations
class TGClient;
class TGPicture;
class TGCompositeFrame;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// class FrameControl  FrameControl.h DDEve/FrameControl.h
  /*
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_EVE
   */
  class FrameControl : public TGMainFrame  {

  protected:
    /// Reference to TEve manager
    TGClient* m_client;
    /// Reference to control's frame
    TGCompositeFrame* m_frame;

  public:
    /// Standard constructor
    FrameControl(TGClient* client, const std::string& name, unsigned int width, unsigned int height);
    /// Default destructor
    virtual ~FrameControl();
    /// Access to the windows client
    TGClient& client() const  {  return *m_client; }
    /// Access to the top level frame
    TGCompositeFrame* frame() const {  return m_frame; }
    /// Build the control
    virtual void Build();
    /// User callback to add elements to the control
    virtual void OnBuild();

    /// Create the frame for this control structure. Default: create horizontal frame
    virtual TGCompositeFrame* CreateFrame();

    /** Helper functions to fill the graphics pane(s)   */

    /// Helper: Load picture using path name
    virtual const TGPicture* LoadPicture(const std::string& path);

    ClassDefOverride(FrameControl,0)  // Top level window frame
  };
}      /* End namespace DD4hep        */
#endif /* DD4HEP_DDEVE_FRAMECONTROL_H */

