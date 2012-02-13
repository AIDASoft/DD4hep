// $Id:$
//====================================================================
//  AIDA Detector description 
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4hep_DETECTOR_DETFACTORYHELPER_H
#define DD4hep_DETECTOR_DETFACTORYHELPER_H

#include "XML/XMLDetector.h"
#include "DD4hep/LCDD.h"
#include "DD4hep/Factories.h"

// Helpers to access tags and attributes quickly without specifying explicitly namespaces
#define _X(a) DD4hep::XML::Tag_##a
#define _A(a) DD4hep::XML::Attr_##a

// Shortcuts to elements of the XML namespace
typedef DD4hep::XML::Collection_t  xml_coll_t;
typedef DD4hep::XML::Handle_t      xml_h;
typedef DD4hep::XML::RefElement    xml_ref_t;
typedef DD4hep::XML::DetElement    xml_det_t;
typedef xml_det_t::Component        xml_comp_t;
typedef DD4hep::XML::Dimension     xml_dim_t;
typedef DD4hep::Geometry::LCDD     lcdd_t;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep { 

  /*
   *   Geometry sub-namespace declaration
   */
  namespace Geometry {
    static inline std::string _toString(const XMLCh* value)  
      {
	return XML::_toString(value); 
      }

    template <typename T> inline std::string _toString(T value, const char* fmt)
      {
	return XML::_toString(value, fmt); 
      }
  }
}

#endif // DD4hep_DETECTOR_DETFACTORYHELPER_H
