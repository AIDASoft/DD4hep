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
#include <DD4hep/Shapes.h>
#include <DD4hep/Printout.h>
#include <DD4hep/SurfaceInstaller.h>

// ROOT includes
#include <TClass.h>

using namespace dd4hep;

typedef DetElement::Children _C;

/// Initializing constructor
SurfaceInstaller::SurfaceInstaller(Detector& description, int argc, char** argv)
  : m_detDesc(description), m_det(), m_stopScanning(false)
{
  if ( argc > 0 )  {
    std::string det_name = argv[0];
    std::string n = det_name[0] == '-' ? det_name.substr(1) : det_name;
    m_det = description.detector(n);
    if ( !m_det.isValid() )   {
      std::stringstream err;
      err << "The subdetector " << det_name << " is not known to the geometry.";
      printout(INFO,"SurfaceInstaller",err.str().c_str());
      except(det_name, err.str());
    }
    printout(INFO,m_det.name(), "+++ Processing SurfaceInstaller for subdetector: '%s'",m_det.name());
    return;
  }
  except("SurfaceInstaller", "The plugin takes at least one argument. No argument supplied");
}

/// Indicate error message and throw exception
void SurfaceInstaller::invalidInstaller(const std::string& msg)   const  {
  const char* det = m_det.isValid() ? m_det.name() : "<UNKNOWN>";
  std::string typ = m_det.isValid() ? m_det.type() : std::string("<UNKNOWN>");
  printout(FATAL,"SurfaceInstaller","+++ Surfaces for: %s",det);
  printout(FATAL,"SurfaceInstaller","+++ %s.",msg.c_str());
  printout(FATAL,"SurfaceInstaller","+++ You sure you apply the correct plugin to generate");
  printout(FATAL,"SurfaceInstaller","+++ surfaces for a detector of type %s",typ.c_str());
  except("SurfaceInstaller", "+++ Failed to install Surfaces to detector "+std::string(det));
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
    std::stringstream log;
    PlacementPath     all_nodes;
    ElementPath       det_elts;
    detail::tools::elementPath(component,det_elts);
    detail::tools::placementPath(component,all_nodes);
    std::string elt_path  = detail::tools::elementPath(det_elts);
    std::string node_path = detail::tools::placementPath(all_nodes);

    log << "Lookup " << " Detector[" << det_elts.size() << "]: " << elt_path;
    printout(INFO,m_det.name(), log.str());
    log.str("");
    log << "       " << " Places[" <<  all_nodes.size()  << "]:   " << node_path;
    printout(INFO,m_det.name(), log.str());
    log.str("");
    log << "       " << " detail::matrix[" <<  all_nodes.size()  << "]: ";
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
  std::cout << component.name() << ": " << pv.name() << std::endl;
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

