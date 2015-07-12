// $Id: Handle.h 570 2013-05-17 07:47:11Z markus.frank $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include "DD4hep/Shapes.h"
#include "DD4hep/Printout.h"
#include "DD4hep/SurfaceInstaller.h"

// C/C++ include files
#include <stdexcept>

// ROOT includes
#include "TClass.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

typedef DetElement::Children _C;

/// Initializing constructor
SurfaceInstaller::SurfaceInstaller(LCDD& lcdd, int argc, char** argv)
  : m_lcdd(lcdd), m_det(), m_stopScanning(false)
{
  if ( argc > 0 )  {
    string det_name = argv[0];
    string n = det_name[0] == '-' ? det_name.substr(1) : det_name;
    m_det = lcdd.detector(n);
    if ( !m_det.isValid() )   {
      stringstream err;
      err << "The subdetector " << det_name << " is not known to the geometry.";
      printout(INFO,"SurfaceInstaller",err.str().c_str());
      throw runtime_error(err.str());
    }
    printout(INFO,m_det.name(),"+++ Processing SurfaceInstaller for subdetector: '%s'",m_det.name());
    return;
  }
  throw runtime_error("The plugin takes at least one argument. No argument supplied");
}

/// Indicate error message and throw exception
void SurfaceInstaller::invalidInstaller(const std::string& msg)   const  {
  const char* det = m_det.isValid() ? m_det.name() : "<UNKNOWN>";
  const char* typ = m_det.isValid() ? m_det.type().c_str() : "<UNKNOWN>";
  printout(FATAL,"SurfaceInstaller","+++ Surfaces for: %s",det);
  printout(FATAL,"SurfaceInstaller","+++ %s.",msg.c_str());
  printout(FATAL,"SurfaceInstaller","+++ You sure you apply the correct plugin to generate");
  printout(FATAL,"SurfaceInstaller","+++ surfaces for a detector of type %s",typ);
  throw std::runtime_error("+++ Failed to install Surfaces to detector "+string(det));
}

/// Shortcut to access the parent detectorelement's volume
Volume SurfaceInstaller::parentVolume(DetElement component)  const  {
  DetElement module = component.parent();
  if ( module.isValid() )   {
    return module.placement().volume();
  }
  return Volume();
}

/// Shortcut to access the translation vector of a given component
const double* SurfaceInstaller::placementTranslation(DetElement component)  const  {
  TGeoMatrix* mat = component.placement()->GetMatrix();
  const double* trans = mat->GetTranslation();
  return trans;
}

/// Printout volume information
void SurfaceInstaller::install(DetElement component, PlacedVolume pv)   {
  if ( pv.volume().isSensitive() )  {
    stringstream log;
    PlacementPath all_nodes;
    ElementPath   det_elts;
    DetectorTools::elementPath(component,det_elts);
    DetectorTools::placementPath(component,all_nodes);
    string elt_path  = DetectorTools::elementPath(det_elts);
    string node_path = DetectorTools::placementPath(all_nodes);

    log << "Lookup " << " Detector[" << det_elts.size() << "]: " << elt_path;
    printout(INFO,m_det.name(),log.str());
    log.str("");
    log << "       " << " Places[" <<  all_nodes.size()  << "]:   " << node_path;
    printout(INFO,m_det.name(),log.str());
    log.str("");
    log << "       " << " Matrices[" <<  all_nodes.size()  << "]: ";
    for(PlacementPath::const_reverse_iterator i=all_nodes.rbegin(); i!=all_nodes.rend(); ++i)  {
      PlacedVolume placed = *i;
      log << (void*)(placed->GetMatrix()) << " ";
      if ( placed->GetUserExtension() )  {
        const PlacedVolume::VolIDs& vid = placed.volIDs();
        for(PlacedVolume::VolIDs::const_iterator j=vid.begin(); j!=vid.end(); ++j)  {
          log << (*j).first << ":" << (*j).second << " ";
        }
      }
      log << " ";
      if ( i+1 == all_nodes.rend() ) log << "( -> " << placed->GetName() << ")";
    }
    // Get the module element:
    printout(INFO,m_det.name(),log.str());
    log.str("");
    Volume vol = pv.volume();
    log << "       "
        << " Sensitive:   " << (vol.isSensitive() ? "YES" : "NO ")
        << " Volume: " << (void*)vol.ptr() << " "
        << " Shape: "  << vol.solid().toString();
    printout(INFO,m_det.name(),log.str());
    return;
  }
  cout << component.name() << ": " << pv.name() << endl;
}

/// Scan through tree of volume placements
void SurfaceInstaller::scan(DetElement e)  {
  const _C& children = e.children();
  install(e,e.placement());
  for (_C::const_iterator i=children.begin(); !m_stopScanning && i!=children.end(); ++i)
    scan((*i).second);
}

/// Scan through tree of volume placements
void SurfaceInstaller::scan()  {
  scan(m_det);
}

