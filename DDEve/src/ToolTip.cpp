// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DDEve/ToolTip.h"

// ROOT include files
#include "TRootEmbeddedCanvas.h"
#include "TCanvas.h"
#include "TEnv.h"
#include "TH1.h"

#include <cstdlib>

using namespace DD4hep;

//______________________________________________________________________________
ToolTip::ToolTip(const char *pname, int cx, int cy, int cw, 
		 int ch, int tx, int ty, int th, 
		 const char *col) : 
  TGShapedFrame(pname, gClient->GetDefaultRoot(), 400, 300, kTempFrame | kHorizontalFrame), m_canvas(0), m_hist(0)
{
  // Shaped window constructor
  m_textX = tx; m_textY = ty; m_textH = th;
  if (col)
    m_textCol = col;
  else
    m_textCol = "0x000000";

  // create the embedded canvas
  if ((cx > 0) && (cy > 0) && (cw > 0) && (ch > 0)) {
    int lhRight  = fWidth-cx-cw;
    int lhBottom = fHeight-cy-ch;
    m_canvas = new TRootEmbeddedCanvas("ec", this, cw, ch, 0);
    AddFrame(m_canvas, new TGLayoutHints(kLHintsTop | kLHintsLeft, cx, lhRight, cy, lhBottom));
  }
  MapSubwindows();
  Resize();
  Resize(fBgnd->GetWidth(), fBgnd->GetHeight());
}

/// Destructor.
ToolTip::~ToolTip()   {
  if (m_hist)
    delete m_hist;
  if (m_canvas)
    delete m_canvas;
}

/// Close shaped window.
void ToolTip::CloseWindow()    {
  DeleteWindow();
}

/// Redraw the window with current attributes.
void ToolTip::Refresh()  {
  const char *str = m_text.c_str();
  char *string = strdup(str);
  int nlines = 0, size = m_textH;
  TString fp = gEnv->GetValue("Root.TTFontPath", "");
  TString ar = fp + "/arial.ttf";
  char *s = strtok((char *)string, "\n");
  TImage *img = (TImage*)fImage->Clone("img");
  img->DrawText(m_textX, m_textY+(nlines*size), s, size, m_textCol.c_str(), ar);
  while ((s = strtok(0, "\n"))) {
    nlines++;
    img->DrawText(m_textX, m_textY+(nlines*size), s, size, m_textCol.c_str(), ar);
  }
  img->PaintImage(fId, 0, 0, 0, 0, 0, 0, "opaque");
  free(string);
  delete img;
  gVirtualX->Update();
}

/// Create the embedded canvas
void ToolTip::CreateCanvas(int cx, int cy, int cw, int ch)  {
  int lhRight  = fWidth-cx-cw;
  int lhBottom = fHeight-cy-ch;
  m_canvas = new TRootEmbeddedCanvas("ec", this, cw, ch, 0);
  AddFrame(m_canvas, new TGLayoutHints(kLHintsTop | kLHintsLeft, cx, lhRight, cy, lhBottom));
  MapSubwindows();
  Resize();
  Resize(fBgnd->GetWidth(), fBgnd->GetHeight());
  if (IsMapped()) {
    Refresh();
  }
}

/// Create the embedded canvas
void ToolTip::CreateCanvas(int cw, int ch, TGLayoutHints *hints)  {
  m_canvas = new TRootEmbeddedCanvas("ec", this, cw, ch, 0);
  AddFrame(m_canvas, hints);
  MapSubwindows();
  Resize();
  Resize(fBgnd->GetWidth(), fBgnd->GetHeight());
  if (IsMapped()) {
    Refresh();
  }
}

/// Set which histogram has to be displayed in the embedded canvas.
void ToolTip::SetHisto(TH1 *hist)   {
  if (hist) {
    if (m_hist) {
      delete m_hist;
      if (m_canvas)
	m_canvas->GetCanvas()->Clear();
    }
    m_hist = (TH1 *)hist->Clone();
    if (m_canvas) {
      m_canvas->GetCanvas()->SetBorderMode(0);
      m_canvas->GetCanvas()->SetFillColor(10);
      m_canvas->GetCanvas()->cd();
      m_hist->Draw();
      m_canvas->GetCanvas()->Update();
    }
  }
}

//______________________________________________________________________________
void ToolTip::SetText(const char *text)
{
  // Set which text has to be displayed.

  if (text) {
    m_text = text;
  }
  if (IsMapped())
    Refresh();
}

//______________________________________________________________________________
void ToolTip::SetTextColor(const char *col)
{
  // Set text color.

  m_textCol = col;
  if (IsMapped())
    Refresh();
}

//______________________________________________________________________________
void ToolTip::SetTextAttributes(int tx, int ty, int th, const char *col)
{
  // Set text attributes (position, size and color).

  m_textX = tx; m_textY = ty; m_textH = th;
  if (col)
    m_textCol = col;
  if (IsMapped())
    Refresh();
}

//______________________________________________________________________________
void ToolTip::Show(int x, int y, const char *text, TH1 *hist)
{
  // Show (popup) the shaped window at location x,y and possibly
  // set the text and histogram to be displayed.

  Move(x, y);
  MapWindow();

  if (text)
    SetText(text);
  if (hist)
    SetHisto(hist);
  // end of demo code -------------------------------------------
  if (m_hist) {
    m_canvas->GetCanvas()->SetBorderMode(0);
    m_canvas->GetCanvas()->SetFillColor(10);
    m_canvas->GetCanvas()->cd();
    m_hist->Draw();
    m_canvas->GetCanvas()->Update();
  }
  Refresh();
}
