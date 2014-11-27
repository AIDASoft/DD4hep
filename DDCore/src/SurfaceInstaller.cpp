// $Id: SiTrackerBarrel_geo.cpp 1360 2014-10-27 16:32:06Z Nikiforos.Nikiforou@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/Printout.h"
#include "DD4hep/SurfaceInstaller.h"

// C/C++ include files
#include <stdexcept>
#include "TClass.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

typedef DetElement::Children _C;

/// Initializing constructor
SurfaceInstaller::SurfaceInstaller(LCDD& lcdd, const std::string& det_name) 
  : m_lcdd(lcdd), m_det()
{
  string n = det_name[0] == '-' ? det_name.substr(1) : det_name;
  m_det = lcdd.detector(n);
  if ( !m_det.isValid() )   {
    stringstream err;
    err << "The subdetector " << det_name << " is not known to the geometry.";
    printout(INFO,"SurfaceInstaller",err.str().c_str());
    throw runtime_error(err.str());
  }
  printout(INFO,m_det.name(),"+++ Processing SurfaceInstaller for subdetector: '%s'",m_det.name());
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
    log << pv->GetVolume()->GetShape()->IsA()->GetName() << " ";
    for(PlacementPath::const_reverse_iterator i=all_nodes.rbegin(); i!=all_nodes.rend(); ++i)  {
      log << (void*)((*i)->GetMatrix()) << "  ";
      if ( i+1 == all_nodes.rend() ) log << "( -> " << (*i)->GetName() << ")";
    }
    // Get the module element:
    TClass* cl = pv->GetVolume()->GetShape()->IsA();
    printout(INFO,m_det.name(),log.str());
    log.str("");
    log << "       "
	<< " Sensitive:" << (pv.volume().isSensitive() ? "YES" : "NO ") 
	<< " Shape: " << cl->GetName();
    TGeoBBox* box = (TGeoBBox*)pv.volume()->GetShape();
    if ( cl == TGeoBBox::Class() )   {
      log << " ["  << (void*)box << "]"
	  << " x:" << box->GetDX() 
	  << " y:" << box->GetDY()
	  << " z:" << box->GetDZ();
    }
  }
}

/// Scan through tree of volume placements
void SurfaceInstaller::scan(DetElement e)  {
  const _C& children = e.children();  
  install(e,e.placement());
  for (_C::const_iterator i=children.begin(); i!=children.end(); ++i)
    scan((*i).second);
}

/// Scan through tree of volume placements
void SurfaceInstaller::scan()  {
  scan(m_det);
}

typedef DD4hep::SurfaceInstaller TestSurfacesPlugin;
DECLARE_SURFACE_INSTALLER(TestSurfaces,TestSurfacesPlugin)
