// $Id$
//====================================================================
//  AIDA Detector description
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_FACTORIES_H
#define DD4HEP_FACTORIES_H

// Framework include files
#include "DD4hep/Plugins.h"
#include "DD4hep/Detector.h"
#include "DD4hep/NamedObject.h"
#include "XML/XMLElements.h"
#include <cstdarg>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace XML {
    class Handle_t;
  }
  class NamedObject;

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    // Forward declarations
    class LCDD;
    class SensitiveDetector;
    class DetElement;

    /// Template class with a generic constructor signature
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \date    2012/07/31
     *  \ingroup DD4HEP_GEOMETRY
     */
    template <typename T> class ConstructionFactory {
    public:
      static void* create(const char* arg);
    };

    /// Template class with a generic signature to apply LCDD plugins
    /**
     *  Specialized factory to translate objects, which can be retrieved from LCDD
     *  Example: Translate DD4hep geometry to Geant4
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \date    2012/07/31
     *  \ingroup DD4HEP_GEOMETRY
     */
    template <typename T> class ApplyFactory {
    public:
      static long create(LCDD& lcdd, int argc, char** argv);
    };

    /// Specialized factory to translate objects, which can be retrieved from LCDD
    /**
     *  Example: Translate DD4hep geometry to Geant4
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \date    2012/07/31
     *  \ingroup DD4HEP_GEOMETRY
     */
    template <typename T> class TranslationFactory {
    public:
      static Ref_t create(LCDD& lcdd);
    };

    /// Create an arbitrary object from it's XML representation.
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \date    2012/07/31
     *  \ingroup DD4HEP_GEOMETRY
     */
    template <typename T> class XMLElementFactory {
    public:
      static Ref_t create(LCDD& lcdd, XML::Handle_t e);
    };

    ///  Read an arbitrary XML document and analyze it's content
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \date    2012/07/31
     *  \ingroup DD4HEP_GEOMETRY
     */
    template <typename T> class XMLDocumentReaderFactory {
    public:
      static long create(LCDD& lcdd, XML::Handle_t e);
    };

    /// Read an arbitrary XML document and analyze it's content
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \date    2012/07/31
     *  \ingroup DD4HEP_GEOMETRY
     */
    template <typename T> class XMLConversionFactory {
    public:
      static long create(LCDD& lcdd, Ref_t& handle, XML::Handle_t element);
    };

    /// Standard factory to create Detector elements from the compact XML representation.
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \date    2012/07/31
     *  \ingroup DD4HEP_GEOMETRY
     */
    template <typename T> class DetElementFactory {
    public:
      static Ref_t create(LCDD& lcdd, XML::Handle_t e, Ref_t sens);
    };
  }
}

namespace {
  template <typename P, typename S> class Factory;
  template <typename P> class Factory<P, void*(const char*)> {
  public:
    static void Func(void *ret, void*, const std::vector<void*>& arg, void*) {
      *(void**) ret = DD4hep::Geometry::ConstructionFactory<P>::create((const char*) arg[0]);
    }
  };

  template <typename P> class Factory<P, DD4hep::NamedObject*(DD4hep::Geometry::LCDD*)> {
  public:
    typedef DD4hep::Geometry::LCDD LCDD;
    typedef DD4hep::Geometry::Ref_t Ref_t;
    static void Func(void *retaddr, void*, const std::vector<void*>& arg, void*) {
      LCDD* lcdd = (LCDD*) arg[0];
      Ref_t handle = DD4hep::Geometry::TranslationFactory<P>::create(*lcdd);
      *(void**) retaddr = handle.ptr();
    }
  };

  template <typename P> class Factory<P, long(DD4hep::Geometry::LCDD*, int, char**)> {
  public:
    typedef DD4hep::Geometry::LCDD LCDD;
    static void Func(void *retaddr, void*, const std::vector<void*>& arg, void*) {
      LCDD* lcdd = (LCDD*) arg[0];
      long handle = DD4hep::Geometry::ApplyFactory<P>::create(*lcdd, *(int*) arg[1], (char**) arg[2]);
      new (retaddr) (long)(handle);
    }
  };

  template <typename P> class Factory<P, DD4hep::NamedObject*(DD4hep::Geometry::LCDD*, DD4hep::XML::Handle_t*)> {
  public:
    typedef DD4hep::Geometry::LCDD LCDD;
    typedef DD4hep::XML::Handle_t xml_h;
    typedef DD4hep::Geometry::Ref_t Ref_t;
    static void Func(void *retaddr, void*, const std::vector<void*>& arg, void*) {
      LCDD* lcdd = (LCDD*) arg[0];
      xml_h* elt = (xml_h*) arg[1];
      Ref_t handle = DD4hep::Geometry::XMLElementFactory<P>::create(*lcdd, *elt);
      *(void**) retaddr = handle.ptr();
    }
  };

  template <typename P> class Factory<P, long(DD4hep::Geometry::LCDD*, DD4hep::XML::Handle_t*)> {
  public:
    typedef DD4hep::Geometry::LCDD LCDD;
    typedef DD4hep::XML::Handle_t xml_h;
    static void Func(void *retaddr, void*, const std::vector<void*>& arg, void*) {
      LCDD* lcdd = (LCDD*) arg[0];
      xml_h* elt = (xml_h*) arg[1];
      long ret = DD4hep::Geometry::XMLDocumentReaderFactory<P>::create(*lcdd, *elt);
      new (retaddr) (long)(ret);
    }
  };

  template <typename P> class Factory<P, DD4hep::NamedObject*(DD4hep::Geometry::LCDD*, DD4hep::XML::Handle_t*, DD4hep::Geometry::Ref_t*)> {
  public:
    typedef DD4hep::Geometry::LCDD LCDD;
    typedef DD4hep::XML::Handle_t xml_h;
    typedef DD4hep::Geometry::Ref_t Ref_t;
    static void Func(void *retaddr, void*, const std::vector<void*>& arg, void*) {
      LCDD* lcdd = (LCDD*) arg[0];
      xml_h* elt = (xml_h*) arg[1];
      Ref_t* sens = (Ref_t*) arg[2];
      Ref_t handle = DD4hep::Geometry::DetElementFactory<P>::create(*lcdd, *elt, *sens);
      *(void**) retaddr = handle.ptr();
    }
  };
}

#define DECLARE_DETELEMENT_FACTORY(x)                                   \
  DD4HEP_PLUGINSVC_FACTORY(x,x,DD4hep::NamedObject*(DD4hep::Geometry::LCDD*,DD4hep::XML::Handle_t*,DD4hep::Geometry::Ref_t*),__LINE__)
#define DECLARE_NAMESPACE_DETELEMENT_FACTORY(n,x)                       \
  DD4HEP_PLUGINSVC_FACTORY(n::x,x,DD4hep::NamedObject*(DD4hep::Geometry::LCDD*,DD4hep::XML::Handle_t*,DD4hep::Geometry::Ref_t*),__LINE__)
#define DECLARE_NAMED_APPLY_FACTORY(n,x)                                \
  DD4HEP_PLUGINSVC_FACTORY(n::x,x,long(DD4hep::Geometry::LCDD*,int, char**),__LINE__)
#define DECLARE_NAMED_TRANSLATION_FACTORY(n,x)                          \
  DD4HEP_PLUGINSVC_FACTORY(n::x,x,DD4hep::NamedObject*(DD4hep::Geometry::LCDD*),__LINE__)
#define DECLARE_NAMED_XMLELEMENT_FACTORY(n,x)                           \
  DD4HEP_PLUGINSVC_FACTORY(n::x,x,DD4hep::NamedObject*(DD4hep::Geometry::LCDD*,DD4hep::XML::Handle_t*),__LINE__)
#define DECLARE_NAMED_DETELEMENT_FACTORY(n,x)                           \
  DD4HEP_PLUGINSVC_FACTORY(n::x,x,DD4hep::NamedObject*(DD4hep::Geometry::LCDD*,DD4hep::XML::Handle_t*,DD4hep::Geometry::Ref_t*),__LINE__)

#define DECLARE_APPLY(name,func)                                        \
  namespace DD4hep { namespace Geometry { namespace { struct name {}; } \
      template <> long ApplyFactory<name>::create(DD4hep::Geometry::LCDD& l,int n,char** a) {return func(l,n,a);} }} \
  DD4HEP_PLUGINSVC_FACTORY(DD4hep::Geometry::name,name,long(DD4hep::Geometry::LCDD*,int,char**),__LINE__)

#define DECLARE_CONSTRUCTOR(name,func)                                  \
  namespace DD4hep { namespace Geometry { namespace { struct name {}; } \
      template <> void* ConstructionFactory<name>::create(const char* n) { return func(n);}}} \
  DD4HEP_PLUGINSVC_FACTORY(DD4hep::Geometry::name,name,void*(const char*),__LINE__)

#define DECLARE_TRANSLATION(name,func)                                  \
  namespace DD4hep { namespace Geometry { namespace { struct name {}; } \
      template <> DD4hep::Geometry::Ref_t TranslationFactory<name>::create(DD4hep::Geometry::LCDD& l) {return func(l);} }} \
  using DD4hep::Geometry::name;                                         \
  DECLARE_NAMED_TRANSLATION_FACTORY(DD4hep::Geometry,name)

#define DECLARE_XMLELEMENT(name,func)                                   \
  namespace DD4hep { namespace Geometry { namespace { struct xml_element_##name {}; } \
      template <> DD4hep::Geometry::Ref_t XMLElementFactory<DD4hep::Geometry::xml_element_##name>::create(DD4hep::Geometry::LCDD& l,DD4hep::XML::Handle_t e) {return func(l,e);} }} \
  using DD4hep::Geometry::xml_element_##name;                           \
  DD4HEP_PLUGINSVC_FACTORY(xml_element_##name,name,DD4hep::NamedObject*(DD4hep::Geometry::LCDD*,DD4hep::XML::Handle_t*),__LINE__)

#define DECLARE_XML_DOC_READER(name,func)                               \
  namespace DD4hep { namespace Geometry { namespace { struct xml_document_##name {}; } \
      template <> long XMLDocumentReaderFactory<DD4hep::Geometry::xml_document_##name>::create(DD4hep::Geometry::LCDD& l,DD4hep::XML::Handle_t e) {return func(l,e);} }} \
  using DD4hep::Geometry::xml_document_##name;                          \
  DD4HEP_PLUGINSVC_FACTORY(xml_document_##name,name##_XML_reader,long(DD4hep::Geometry::LCDD*,DD4hep::XML::Handle_t*),__LINE__)

#define DECLARE_XML_PROCESSOR_BASIC(name,func,deprecated)               \
  namespace DD4hep { namespace Geometry { struct det_element_##name {}; \
      template <> DD4hep::Geometry::Ref_t DetElementFactory< DD4hep::Geometry::det_element_##name >::create(DD4hep::Geometry::LCDD& l,DD4hep::XML::Handle_t e,DD4hep::Geometry::Ref_t h){ if (deprecated) warning_deprecated_xml_factory(#name); return func(l,e,h);}}  } \
  using DD4hep::Geometry::det_element_##name;                           \
  DD4HEP_PLUGINSVC_FACTORY(det_element_##name,name,DD4hep::NamedObject*(DD4hep::Geometry::LCDD*,DD4hep::XML::Handle_t*,DD4hep::Geometry::Ref_t*),__LINE__)

#define DECLARE_XML_PROCESSOR(name,func)          DECLARE_XML_PROCESSOR_BASIC(name,func,0)
#define DECLARE_SUBDETECTOR(name,func)            DECLARE_XML_PROCESSOR_BASIC(name,func,0)
#define DECLARE_DETELEMENT(name,func)             DECLARE_XML_PROCESSOR_BASIC(name,func,0)
#define DECLARE_DEPRECATED_DETELEMENT(name,func)  DECLARE_XML_PROCESSOR_BASIC(name,func,1)

#endif // DD4HEP_FACTORIES_H
