// $Id:$
//====================================================================
//  AIDA Detector description 
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4hep_FACTORIES_H
#define DD4hep_FACTORIES_H

#ifndef __CINT__
#include "Reflex/PluginService.h"
#endif
#include "RVersion.h"
#include "DD4hep/Detector.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {
  /*
   *   XML sub-namespace declaration
   */
  namespace XML {
    struct Handle_t;
  }

  /*
   *   Geometry sub-namespace declaration
   */
  namespace Geometry {

    // Forward declarations
    struct LCDD;
    struct SensitiveDetector;
    struct DetElement;


    template <typename T> class DetElementFactory  {
    public:
      static Ref_t create(LCDD& lcdd, const XML::Handle_t& e, SensitiveDetector& sens);
    };
  }  
}

namespace {
  template < typename P > class Factory<P, TNamed*(DD4hep::Geometry::LCDD*,const DD4hep::XML::Handle_t*,DD4hep::Geometry::SensitiveDetector*)> {
  public:
    typedef DD4hep::Geometry::LCDD  LCDD;
    typedef DD4hep::XML::Handle_t   xml_h;
    typedef DD4hep::Geometry::Ref_t Ref_t;
    static void Func(void *retaddr, void*, const std::vector<void*>& arg, void*) {
      LCDD*  lcdd = (LCDD* )arg[0];
      xml_h* elt  = (xml_h*)arg[1];
      DD4hep::Geometry::SensitiveDetector* sens = (DD4hep::Geometry::SensitiveDetector*)arg[2];
      Ref_t handle = DD4hep::Geometry::DetElementFactory<P>::create(*lcdd,*elt,*sens);
      *(void**)retaddr = handle.ptr();
    }
  };
}

#define DECLARE_DETELEMENT_FACTORY(x)  PLUGINSVC_FACTORY(x,TNamed*(DD4hep::Geometry::LCDD*,const DD4hep::XML::Handle_t*,DD4hep::Geometry::SensitiveDetector*))

#define DECLARE_NAMESPACE_DETELEMENT_FACTORY(n,x)  using n::x; PLUGINSVC_FACTORY(x,TNamed*(DD4hep::Geometry::LCDD*,const DD4hep::XML::Handle_t*,DD4hep::Geometry::SensitiveDetector*))

#define DECLARE_NAMED_DETELEMENT_FACTORY(n,x)  using n::x; PLUGINSVC_FACTORY_WITH_ID(x,std::string(#x),TNamed*(DD4hep::Geometry::LCDD*,const DD4hep::XML::Handle_t*,DD4hep::Geometry::SensitiveDetector*))

#endif // DD4hep_FACTORIES_H
