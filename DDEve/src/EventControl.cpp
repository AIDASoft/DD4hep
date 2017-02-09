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

// Framework include files
#include "DDEve/Display.h"
#include "DDEve/EventControl.h"
#include "DDEve/EventHandler.h"
#include "DD4hep/InstanceCount.h"

// ROOT include files
#include <TSystem.h>
#include <TGTab.h>
#include <TGLabel.h>
#include <TGFrame.h>
#include <TGButton.h>
#include <TG3DLine.h>
#include "TGFileDialog.h"

#include "TTree.h"
#include <libgen.h>

using namespace std;
using namespace DD4hep;

ClassImp(EventControl)

/// Standard constructor
EventControl::EventControl(Display* display, unsigned int width, unsigned int height) 
: FrameControl(&display->client(), "EventControl GUI", width, height), EventConsumer(), 
  m_display(display), m_dataGroup(0), m_dataFrame(0), m_eventGroup(0),
  m_numEvtFrame(0), m_input1(0), m_input2(0), m_numEvtLabel(0),
  m_open(0), m_prev(0), m_next(0), m_goto(0)
{
  SetWindowName("XX GUI");
  m_display->eventHandler().Subscribe(this);
  InstanceCount::increment(this);
}

/// Default destructor
EventControl::~EventControl()   {
  try {
    m_display->eventHandler().Unsubscribe(this);
  }
  catch(...)  {
  }
  InstanceCount::decrement(this);
}

/// Create the frame for this control structure. Default: create horizontal frame
TGCompositeFrame* EventControl::CreateFrame()    {
  return new TGVerticalFrame(this);
}

/// Load the next event
void EventControl::NextEvent()   {
  m_display->eventHandler().NextEvent();
}

/// Load the previous event
void EventControl::PreviousEvent()   {
  m_display->eventHandler().PreviousEvent();
}

/// Goto a specified event
void EventControl::GotoEvent()   {
  long number = 0;
  m_display->eventHandler().GotoEvent(number);
}

/// Open a new event data file
bool EventControl::Open()   {
  // e- shots:
  //m_display->eventHandler().Open("/home/frankm/SW/DD4hep_head_dbg.root_v5.34.10/build/CLICSiD_2014-06-10_15-26.root");
  // pi- shots:
  //m_display->eventHandler().Open("/home/frankm/SW/DD4hep_head_dbg.root_v5.34.10/build/CLICSiD_2014-06-18_12-48.root");

  std::string fname = m_display->OpenEventFileDialog(".");
  if ( !fname.empty() )  {
    return m_display->eventHandler().Open(m_display->getEventHandlerName(),fname);
  }
  return false;
}

/// EventConsumer overload: New event data file
void EventControl::OnFileOpen(EventHandler& handler)  {
  char text[1024], fname[1024];
  ::strncpy(fname, handler.datasourceName().c_str(), sizeof(fname)-1);
  fname[sizeof(fname)-1] = 0;
  // -----------------------------------------------------------------------------------------
  if ( handler.hasFile() )   {
    ::snprintf(text,sizeof(text),"Number of events: %ld",handler.numEvents());
    m_input1->SetText(::basename(fname));
    m_input2->SetText(text);
  }
  else  {
    ::snprintf(text,sizeof(text),"Currently NO input defined");
    m_input1->SetText("");
    m_input2->SetText(text);
  }
  m_eventGroup->Layout();
  client().NeedRedraw(m_eventGroup,kTRUE);
}

/// Consumer event data
void EventControl::OnNewEvent(EventHandler& handler)   {
  typedef EventHandler::TypedEventCollections Types;
  typedef std::vector<EventHandler::Collection> Collections;
  const Types& types = handler.data();
  size_t cnt = 1;
  m_lines[0].second.first->SetText("Hit collection name");
  m_lines[0].second.second->SetText("No.Hits");
  for(const auto& t : types)  {
    const Collections& colls = t.second;
    Line line  = m_lines[cnt++];
    string cl  = t.first;
    size_t idx = cl.rfind("Geant4");
    if ( idx != string::npos ) { 
      cl = cl.substr(idx);
      cl = cl.substr(0,cl.find('*'));
    }
    else if ( (idx=cl.rfind("::")) != string::npos )  {
      cl = cl.substr(idx+2);
      if ( (idx=cl.rfind('*')) != string::npos ) cl = cl.substr(0,idx);
      if ( (idx=cl.rfind('>')) != string::npos ) cl = cl.substr(0,idx);
    }
    line.second.first->SetTextColor(kRed);
    line.second.second->SetTextColor(kRed);
    line.second.first->SetText(("Coll.Type: "+cl).c_str());
    line.second.second->SetText("");
    for(const auto& c : colls)  {
      char text[132];
      ::snprintf(text,sizeof(text),"%ld",long(c.second));
      line = m_lines[cnt++];
      line.second.first->SetText(c.first);
      line.second.second->SetText(text);
      line.second.first->SetTextColor(kBlack);
      line.second.second->SetTextColor(kBlack);
    }
  }
  for(; cnt<m_lines.size(); )  {
    Line line = m_lines[cnt++];
    line.second.first->SetText("");
    line.second.second->SetText("");
  }
  m_dataGroup->Layout();
  client().NeedRedraw(m_dataGroup,kTRUE);
}

/// User callback to add elements to the control
void EventControl::OnBuild()   {
  string icondir = TString::Format("%s/icons/", gSystem->Getenv("ROOTSYS")).Data();
  TGGroupFrame* group = new TGGroupFrame(m_frame,"Event I/O Control");
  TGCompositeFrame* top = new TGHorizontalFrame(group);
  TGPictureButton* b = 0;
  char text[1024];
  group->SetTitlePos(TGGroupFrame::kLeft);
  m_frame->AddFrame(group, new TGLayoutHints(kLHintsExpandX|kLHintsCenterX, 2, 2, 2, 2));
  m_eventGroup = group;
  // -----------------------------------------------------------------------------------------
  m_numEvtFrame = new TGVerticalFrame(group);
  if ( m_display->eventHandler().hasFile() )   {
    ::snprintf(text,sizeof(text),"Number of events: %ld",m_display->eventHandler().numEvents());
    m_input1 = new TGLabel(m_numEvtFrame,m_display->eventHandler().datasourceName().c_str());
    m_input2 = new TGLabel(m_numEvtFrame,text);
  }
  else  {
    ::snprintf(text,sizeof(text),"Currently NO input defined");
    m_input1 = new TGLabel(m_numEvtFrame,"");
    m_input2 = new TGLabel(m_numEvtFrame,text);
  }
  m_numEvtFrame->AddFrame(m_input1, new TGLayoutHints(kLHintsNormal, 2, 0, 2, 2));
  m_numEvtFrame->AddFrame(m_input2, new TGLayoutHints(kLHintsNormal, 2, 0, 2, 2));
  group->AddFrame(m_numEvtFrame, new TGLayoutHints(kLHintsExpandX|kLHintsCenterX, 2, 2, 2, 2));
  // -----------------------------------------------------------------------------------------
  top->AddFrame(new TGLabel(top,"Open event file:"), new TGLayoutHints(kLHintsLeft|kLHintsCenterY, 2, 2, 2, 2));
  m_open = b = new TGPictureButton(top, LoadPicture((icondir+"bld_open.png")));
  b->Connect("Clicked()", "DD4hep::EventControl", this, "Open()");
  b->SetSize(TGDimension(32,32));
  top->AddFrame(b, new TGLayoutHints(kLHintsRight, 2, 2, 2, 2));
  group->AddFrame(top, new TGLayoutHints(kLHintsExpandX|kLHintsCenterX, 2, 2, 2, 2));
  // -----------------------------------------------------------------------------------------
  top = new TGHorizontalFrame(group);
  top->AddFrame(new TGLabel(top,"Previous:"), new TGLayoutHints(kLHintsLeft|kLHintsCenterY, 2, 2, 2, 2));
  m_prev = b = new TGPictureButton(top, LoadPicture((icondir+"bld_undo.png")));
  b->Connect("Clicked()", "DD4hep::EventControl", this, "PreviousEvent()");
  b->SetSize(TGDimension(32,32));
  top->AddFrame(b, new TGLayoutHints(kLHintsLeft, 2, 2, 2, 2));

  top->AddFrame(new TGLabel(top,""), new TGLayoutHints(kLHintsExpandX|kLHintsCenterY, 2, 2, 2, 2));
  top->AddFrame(new TGLabel(top,"Next:"), new TGLayoutHints(kLHintsLeft|kLHintsCenterY, 2, 2, 2, 2));
  m_next = b = new TGPictureButton(top, LoadPicture((icondir+"bld_redo.png")));
  b->Connect("Clicked()", "DD4hep::EventControl", this, "NextEvent()");
  b->SetSize(TGDimension(32,32));
  top->AddFrame(b, new TGLayoutHints(kLHintsRight, 2, 2, 2, 2));
  group->AddFrame(top,new TGLayoutHints(kLHintsExpandX|kLHintsCenterX, 2, 2, 2, 2));
  // -----------------------------------------------------------------------------------------
  top = new TGHorizontalFrame(group);
  top->AddFrame(new TGLabel(top,"Goto event:"), new TGLayoutHints(kLHintsLeft|kLHintsCenterY, 2, 2, 2, 2));
  m_goto = b = new TGPictureButton(top, LoadPicture((icondir+"ed_goto.png")));
  b->Connect("Clicked()", "DD4hep::EventControl", this, "GotoEvent()");
  b->SetSize(TGDimension(32,32));
  top->AddFrame(b, new TGLayoutHints(kLHintsRight, 2, 2, 2, 2));
  group->AddFrame(top,new TGLayoutHints(kLHintsExpandX|kLHintsCenterX, 2, 2, 2, 2));
  // -----------------------------------------------------------------------------------------
  group = new TGGroupFrame(m_frame,"Event data",200);
  m_frame->AddFrame(group,new TGLayoutHints(kLHintsLeft|kLHintsExpandX|kLHintsExpandY, 0, 0, 2, 2));
  m_dataFrame = new TGVerticalFrame(group);
  for( int i=0; i<NUM_DATA_LINES; ++i )    {
    Line line;
    TGCompositeFrame* fr = new TGHorizontalFrame(m_dataFrame);
    fr->AddFrame(line.second.first=new TGLabel(fr,""), new TGLayoutHints(kLHintsNormal, 2, 0, 2, 2));
    fr->AddFrame(line.second.second=new TGLabel(fr,""), new TGLayoutHints(kLHintsRight, 20, 1, 2, 2));
    line.first = fr;
    m_lines.push_back(line);
    m_dataFrame->AddFrame(fr,new TGLayoutHints(kLHintsExpandX));
  }
  group->AddFrame(m_dataFrame, new TGLayoutHints(kLHintsNormal|kLHintsExpandX|kLHintsExpandY));
  m_dataGroup = group;
  // -----------------------------------------------------------------------------------------
}

