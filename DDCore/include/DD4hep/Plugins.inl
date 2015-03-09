// $Id: Readout.cpp 590 2013-06-03 17:02:43Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_PLUGINS_INL
#define DD4HEP_PLUGINS_INL

#include "DD4hep/Plugins.h"
#include "RVersion.h"

#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
#include "DD4hep/Plugins_ROOT5.inl"
#elif ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0) and ROOT_VERSION_CODE < ROOT_VERSION(7,0,0)
#include "DD4hep/Plugins_ROOT6.inl"
#else
#pragma error "Unsupported ROOT version required."
#endif

#endif  // DD4HEP_PLUGINS_INL
