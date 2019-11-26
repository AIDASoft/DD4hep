//==========================================================================
//  AIDA Detector description implementation 
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
#include <DDDigi/DigiSegmentation.h>
#include <DD4hep/Segmentations.h>
#include <DD4hep/Printout.h>
#include <DD4hep/Plugins.h>
#include <DD4hep/Shapes.h>


std::shared_ptr<dd4hep::digi::DigiCellScanner>
dd4hep::digi::create_cell_scanner(Solid solid, Segmentation segment)   {
  std::string typ = "DigiCellScanner" +
    std::string("_") + segment.type() +
    std::string("_") + solid.title();
  return create_cell_scanner(typ, segment);
}

std::shared_ptr<dd4hep::digi::DigiCellScanner>
dd4hep::digi::create_cell_scanner(const std::string& typ, Segmentation segment)   {
  using namespace dd4hep;
  SegmentationObject* seg = segment.ptr();
  DigiCellScanner*   scan = PluginService::Create<DigiCellScanner*>(typ, seg);
  if ( !scan )   {
    PluginDebug dbg;
    PluginService::Create<DigiCellScanner*>(typ, seg);
    except("create_cell_scanner",
           "Failed to create cell scanner of type %s [%s]",
           typ.c_str(),dbg.missingFactory(typ).c_str());
  }
  return std::shared_ptr<DigiCellScanner>(scan);
}
