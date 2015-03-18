// $Id: XMLDetector.cpp 1479 2014-12-18 16:00:45Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
// Framework include files
#include "XML/Utilities.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Plugins.h"
#include "DD4hep/LCDD.h"

using namespace std;
using namespace DD4hep;

/// Create a solid shape using the plugin mechanism from the attributes of the XML element
Geometry::Solid 
DD4hep::XML::createShape(Geometry::LCDD& lcdd, const std::string& shape_type, XML::Element element)   {
  string fac  = shape_type + "__shape_constructor";
  XML::Handle_t solid_elt = element;
  Geometry::Solid solid = Geometry::Ref_t(PluginService::Create<NamedObject*>(fac, &lcdd, &solid_elt));
  if ( !solid.isValid() )  {
    PluginDebug dbg;
    PluginService::Create<NamedObject*>(shape_type, &lcdd, &solid_elt);
    except("XML::createShape","Failed to create solid of type %s [%s]", 
	   shape_type.c_str(),dbg.missingFactory(shape_type).c_str());
  }
  return solid;
}


/// Create a solid shape using the plugin mechanism from the attributes of the XML element
Geometry::Volume
DD4hep::XML::createVolume(Geometry::LCDD& lcdd, const std::string& volume_type, XML::Element element)   {
  string fac  = volume_type + "_volume";
  XML::Handle_t volume_elt = element;
  Geometry::Volume volume = Geometry::Ref_t(PluginService::Create<TGeoVolume*>(fac, &lcdd, &volume_elt));
  if ( !volume.isValid() )  {
    PluginDebug dbg;
    PluginService::Create<NamedObject*>(volume_type, &lcdd, &volume_elt);
    except("XML::createVolume","Failed to create volume of type %s [%s]", 
	   volume_type.c_str(),dbg.missingFactory(fac).c_str());
  }
  return volume;
}
