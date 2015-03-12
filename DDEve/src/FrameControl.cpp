// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DDEve/FrameControl.h"
#include "DD4hep/Printout.h"

// ROOT include files
#include <TSystem.h>
#include <TGTab.h>
#include <TGButton.h>

#include <stdexcept>

using namespace std;
using namespace DD4hep;

ClassImp(FrameControl)

/// Standard constructor
FrameControl::FrameControl(TGClient* cl, const std::string& name, unsigned int width, unsigned int height) 
: TGMainFrame(cl->GetRoot(), width, height), m_client(cl), m_frame(0)
{
  SetWindowName(name.c_str());
  SetCleanup(kDeepCleanup);
}

/// Default destructor
FrameControl::~FrameControl()   {
  if ( m_frame )  {
    m_frame->DestroySubwindows();
    m_frame->RemoveAll();
  }
}

/// Build the control
void FrameControl::Build()   {
  m_frame = CreateFrame();
  AddFrame(m_frame, new TGLayoutHints(kLHintsNormal|kLHintsExpandY|kLHintsExpandX));
  OnBuild();
  MapSubwindows();
  Resize();
  MapWindow();
}

/// User callback to add elements to the control
void FrameControl::OnBuild()   {
}

/// Create the frame for this control structure. Default: create horizontal frame
TGCompositeFrame* FrameControl::CreateFrame()    {
  return new TGHorizontalFrame(this);
}

/// Helper: Load picture using path name
const TGPicture* FrameControl::LoadPicture(const std::string& path)   {
  const TGPicture* pic = m_client->GetPicture(path.c_str());
  if ( !pic )   {
    printout(ERROR,"FrameControl","+++ loadPicture: Failed to load picture: %s",path.c_str());
    throw std::runtime_error("FrameControl::loadPicture: Failed to load picture:"+path);
  }
  return pic;
}



