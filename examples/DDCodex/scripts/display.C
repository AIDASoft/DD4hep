//--------------------------------------------------------------------------
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author  Markus Frank
//  \date    2018-05-31
//  \version 1.0
//
//==========================================================================
#include "TGLViewer.h"
#include "TGLUtil.h"
#include "TSystem.h"
#include <stdexcept>
#include <string>

void apply_draw_opts(TGLClip::EType clip_typ, TGLUtil::EAxesType axis_typ)   {
  TGLViewer *v = (TGLViewer*)gPad->GetViewer3D();
  if ( v )   {
    v->SetCurrentCamera(TGLViewer::kCameraPerspXOZ);
    v->GetClipSet()->SetClipType(clip_typ);
    v->GetClipSet()->SetAutoUpdate(kTRUE);
    v->GetClipSet()->SetShowClip(kTRUE);
    v->SetGuideState(axis_typ, kTRUE, kFALSE, 0);
    v->GetClipSet()->GetCurrentClip()->SetDiffuseColor(0xFFFF,0xff);
  }
}

void clip_plane()    {  apply_draw_opts(TGLClip::kClipPlane, TGLUtil::kAxesOrigin);   }
void clip_box()      {  apply_draw_opts(TGLClip::kClipBox,   TGLUtil::kAxesOrigin);   }
void clip_none()     {  apply_draw_opts(TGLClip::kClipNone,  TGLUtil::kAxesOrigin);   }

std::string install_dir()   {
  const char* env = gSystem->Getenv("DD4hepINSTALL");
  if ( env )   {
    string dir = env;
    dir += "/examples/DDCodex";
    return dir;
  }
  throw std::runtime_error("The environment DD4hepINSTALL is not set. No actions taken!");
}
std::string geo_file()   {
  return install_dir()+"/Upgrade.root";
}
std::string codex_file()   {
  return install_dir()+"/compact/CODEX-b.xml";
}

void load_geo()     {
  std::string fn = geo_file();
  gDD4hepUI->importROOT(geo_file().c_str());
}

void load_codex()   {
  gDD4hepUI->importROOT(geo_file().c_str());
  gDD4hepUI->fromXML(codex_file().c_str());
  gDD4hepUI->draw();
  clip_plane();
}

void dumpVP()    {
  const void* args[] = {
    "-detector", "/world/LHCb/BeforeMagnetRegion/VP",
    "-path",
    "-shapes",
    "-positions",
    "-materials",
    "-topstats", 0};
  gDD4hepUI->dumpVols(sizeof(args)/sizeof(args[0])-1, (char**)args);
}

void display()    {  
}
