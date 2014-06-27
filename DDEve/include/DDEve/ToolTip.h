// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDEVE_TOOLTIP_H
#define DD4HEP_DDEVE_TOOLTIP_H

#include <string>

// ROOT include files
#include "TGShapedFrame.h"

// Forward declarations
class TRootEmbeddedCanvas;
class TH1;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /** @class ToolTip  ToolTip.h DDEve/ToolTip.h
   *
   *
   * Original author: Bertrand Bellenot CERN/SFT
   *
   * @author  M.Frank
   * @version 1.0
   */
  class ToolTip : public TGShapedFrame  {

private:
   ToolTip(const ToolTip&); // Not implemented
   ToolTip& operator=(const ToolTip&); // Not implemented

protected:
   int                   m_textX, m_textY, m_textH;
   std::string           m_textCol;

   TRootEmbeddedCanvas  *m_canvas;   // embedded canvas for histogram
   TH1                  *m_hist;     // user histogram
   std::string           m_text;     // info (as tool tip) text

   /// Redraw the window with current attributes.
   virtual void          DoRedraw() {}

public:
   ToolTip(const char *picname, int cx=0, int cy=0, int cw=0, 
	   int ch=0, int tx=0, int ty=0, int th=0, 
	   const char *col="#ffffff");
   /// Destructor.
   virtual ~ToolTip();

   /// Close shaped window.
   virtual void   CloseWindow();

   /// Create the embedded canvas
   void           CreateCanvas(int cx, int cy, int cw, int ch);
   /// Create the embedded canvas
   void           CreateCanvas(int cw, int ch, TGLayoutHints *hints);

   TH1           *GetHisto() const { return m_hist; }
   const char    *GetText() const  { return m_text.c_str(); }
   void           Refresh();
   /// Set which histogram has to be displayed in the embedded canvas.
   void           SetHisto(TH1 *hist);
   void           SetText(const char *text);
   void           SetTextColor(const char *col);
   void           SetTextAttributes(int tx, int ty, int th, const char *col=0);
   void           Show(int x, int y, const char *text = 0, TH1 *hist = 0);

   ClassDef(ToolTip, 0) // Shaped composite frame
  };


} /* End namespace DD4hep   */


#endif /* DD4HEP_DDEVE_TOOLTIP_H */

