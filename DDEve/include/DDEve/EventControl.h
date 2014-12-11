// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDEVE_EVENTCONTROL_H
#define DD4HEP_DDEVE_EVENTCONTROL_H

// Framework include files
#include "DDEve/FrameControl.h"
#include "DDEve/GenericEventHandler.h"

// Forward declarations
class TGPictureButton;
class TGLabel;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  // Forward declarations
  class Display;

  /** EventControl  EventControl.h DDEve/EventControl.h
   *
   * \author  M.Frank
   * \version 1.0
   * \ingroup DD4HEP_EVE
   */
  class EventControl : public FrameControl, public EventConsumer  {
    enum { NUM_DATA_LINES = 40 };
    /// Refernece to the display object
    Display          *m_display;
    /// Data frame
    TGGroupFrame     *m_dataGroup;
    TGCompositeFrame *m_dataFrame;
    /// Data frame
    TGGroupFrame     *m_eventGroup;
    TGCompositeFrame *m_numEvtFrame;
    TGLabel          *m_input1, *m_input2;
    TGLabel          *m_numEvtLabel;
    TGPictureButton  *m_open, *m_prev, *m_next, *m_goto;
    typedef std::pair<TGLabel*,TGLabel*> Labels;
    typedef std::pair<TGCompositeFrame*,Labels> Line;
    typedef std::vector<Line > Lines;
    Lines   m_lines;
  public:
    /// Standard constructor
    EventControl(Display* disp, unsigned int width, unsigned int height);
    /// Default destructor
    virtual ~EventControl();

    /// User callback to add elements to the control
    virtual void OnBuild();
    /// Create the frame for this control structure. Default: create horizontal frame
    TGCompositeFrame* CreateFrame();

    /// Open a new event data file
    virtual bool Open();
    /// Load the next event
    virtual void NextEvent();
    /// Load the previous event
    virtual void PreviousEvent();
    /// Goto a specified event
    virtual void GotoEvent();


    /// EventConsumer overload: New consumer event data
    virtual void OnNewEvent(EventHandler* handler);

    /// EventConsumer overload: New event data file
    virtual void OnFileOpen(EventHandler* handler);

    ClassDef(EventControl,0)  // Top level window frame
  };


} /* End namespace DD4hep   */


#endif /* DD4HEP_DDEVE_EVENTCONTROL_H */

