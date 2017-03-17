//==========================================================================
//  AIDA Detector description implementation for LCD
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
#ifndef DD4HEP_FACTORIES_H
#define DD4HEP_FACTORIES_H

// Framework include files
#include "DD4hep/Plugins.h"
#include "DD4hep/Detector.h"
#include "DD4hep/NamedObject.h"

// C/C++ include files
#include <cstdarg>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the AIDA detector description toolkit supporting JSON utilities
  namespace JSON {
    class Handle_t;
  }
  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace XML {
    class Handle_t;
    class RefElement;
  }
  class NamedObject;

  /// Namespace describing generic detector segmentations
  namespace DDSegmentation  {
    class BitField64;
  }
  
  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    // Forward declarations
    class LCDD;
    class SensitiveDetector;
    class SegmentationObject;
    class DetElement;
  }
  
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
  template <typename T> class LCDDConstructionFactory : public PluginFactoryBase {
  public:
    static void* create(Geometry::LCDD& lcdd, int argc, char** argv);
  };
  template <typename T> class SegmentationFactory : public PluginFactoryBase {
  public:
    static Geometry::SegmentationObject* create(DDSegmentation::BitField64* decoder);
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
  template <typename T> class ApplyFactory : public PluginFactoryBase {
  public:
    static long create(Geometry::LCDD& lcdd, int argc, char** argv);
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
  template <typename T> class TranslationFactory : public PluginFactoryBase {
  public:
    static ref_t create(Geometry::LCDD& lcdd);
  };

  /// Create an arbitrary object from it's XML representation.
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \date    2012/07/31
   *  \ingroup DD4HEP_GEOMETRY
   */
  template <typename T> class XMLElementFactory : public PluginFactoryBase {
  public:
    static ref_t create(Geometry::LCDD& lcdd, XML::Handle_t e);
  };

  ///  Read an arbitrary XML document and analyze it's content
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \date    2012/07/31
   *  \ingroup DD4HEP_GEOMETRY
   */
  template <typename T> class XMLDocumentReaderFactory : public PluginFactoryBase {
  public:
    static long create(Geometry::LCDD& lcdd, XML::Handle_t e);
  };

  /// Read an arbitrary XML document and analyze it's content
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \date    2012/07/31
   *  \ingroup DD4HEP_GEOMETRY
   */
  template <typename T> class XMLConversionFactory : public PluginFactoryBase {
  public:
    static long create(Geometry::LCDD& lcdd, XML::RefElement& handle, XML::Handle_t element);
  };

  /// Standard factory to create Detector elements from the compact XML representation.
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \date    2012/07/31
   *  \ingroup DD4HEP_GEOMETRY
   */
  template <typename T> class XmlDetElementFactory : public PluginFactoryBase {
  public:
    static Geometry::Ref_t create(Geometry::LCDD& lcdd, XML::Handle_t e, Geometry::Ref_t sens);
  };

  /// Standard factory to create Detector elements from the compact XML representation.
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \date    2012/07/31
   *  \ingroup DD4HEP_GEOMETRY
   */
  template <typename T> class JsonDetElementFactory : public PluginFactoryBase {
  public:
    static Geometry::Ref_t create(Geometry::LCDD& lcdd, JSON::Handle_t e, Geometry::Ref_t sens);
  };
}

namespace {

  /// Forward declartion of the base factory template
  template <typename P, typename S> class Factory;

  struct ns  {
    typedef DD4hep::NamedObject       Named;
    typedef DD4hep::Geometry::LCDD    LCDD;
    typedef DD4hep::XML::Handle_t     xml_h;
    typedef DD4hep::JSON::Handle_t    json_h;
    typedef DD4hep::Geometry::Ref_t   ref_t;
    typedef DD4hep::Geometry::SegmentationObject SegmentationObject;
    typedef DD4hep::DDSegmentation::BitField64   BitField64;
  };

  DD4HEP_PLUGIN_FACTORY_ARGS_1(void*,const char*)  
  {    return DD4hep::ConstructionFactory<P>::create(a0);                               }

  DD4HEP_PLUGIN_FACTORY_ARGS_1(ns::Named*,ns::LCDD*)
  {    return DD4hep::TranslationFactory<P>::create(*a0).ptr();                         }

  DD4HEP_PLUGIN_FACTORY_ARGS_1(ns::SegmentationObject*,ns::BitField64*)
  {    return DD4hep::SegmentationFactory<P>::create(a0);                               }

  DD4HEP_PLUGIN_FACTORY_ARGS_3(void*,ns::LCDD*,int,char**)
  {    return DD4hep::LCDDConstructionFactory<P>::create(*a0,a1,a2);                    }

  DD4HEP_PLUGIN_FACTORY_ARGS_3(long,ns::LCDD*,int,char**)
  {    return make_return<long>(DD4hep::ApplyFactory<P>::create(*a0,a1,a2));            }

  DD4HEP_PLUGIN_FACTORY_ARGS_2(ns::Named*,ns::LCDD*,ns::xml_h*)
  {    return DD4hep::XMLElementFactory<P>::create(*a0,*a1).ptr();                      }

  DD4HEP_PLUGIN_FACTORY_ARGS_2(long,ns::LCDD*,ns::xml_h*)
  {    return make_return<long>(DD4hep::XMLDocumentReaderFactory<P>::create(*a0,*a1));  }

  DD4HEP_PLUGIN_FACTORY_ARGS_3(ns::Named*,ns::LCDD*,ns::xml_h*,ns::ref_t*)
  {    return DD4hep::XmlDetElementFactory<P>::create(*a0,*a1,*a2).ptr();               }

  DD4HEP_PLUGIN_FACTORY_ARGS_3(ns::Named*,ns::LCDD*,ns::json_h*,ns::ref_t*)
  {    return DD4hep::JsonDetElementFactory<P>::create(*a0,*a1,*a2).ptr();              }
}

#define DECLARE_DETELEMENT_FACTORY(x)               namespace DD4hep    \
  { DD4HEP_PLUGINSVC_FACTORY(x,x,DD4hep::NamedObject*(ns::LCDD*,XML::Handle_t*,Geometry::Ref_t*),__LINE__) }
#define DECLARE_NAMESPACE_DETELEMENT_FACTORY(n,x)   namespace DD4hep    \
  { DD4HEP_PLUGINSVC_FACTORY(n::x,x,NamedObject*(ns::LCDD*,XML::Handle_t*,Geometry::Ref_t*),__LINE__)      }
#define DECLARE_NAMED_APPLY_FACTORY(n,x)            namespace DD4hep    \
  { DD4HEP_PLUGINSVC_FACTORY(n::x,x,long(ns::LCDD*,int, char**),__LINE__) }
#define DECLARE_NAMED_TRANSLATION_FACTORY(n,x)      namespace DD4hep    \
  { DD4HEP_PLUGINSVC_FACTORY(n::x,x,DD4hep::NamedObject*(ns::LCDD*),__LINE__) }
#define DECLARE_NAMED_XMLELEMENT_FACTORY(n,x)       namespace DD4hep    \
  { DD4HEP_PLUGINSVC_FACTORY(n::x,x,DD4hep::NamedObject*(ns::LCDD*,XML::Handle_t*),__LINE__) }
#define DECLARE_NAMED_DETELEMENT_FACTORY(n,x)       namespace DD4hep    \
  { DD4HEP_PLUGINSVC_FACTORY(n::x,x,DD4hep::*(),__LINE__) }

// Call function of the type [Geometry::SegmentationObject (*func)(ns::LCDD*,DDSegmentation::BitField64*)]
#define DECLARE_SEGMENTATION(name,func)        DD4HEP_OPEN_PLUGIN(DD4hep,name)   { \
    template <> Geometry::SegmentationObject*                           \
      SegmentationFactory<name>::create(DDSegmentation::BitField64* d) { return func(d); } \
    DD4HEP_PLUGINSVC_FACTORY(name,segmentation_constructor__##name,     \
                             Geometry::SegmentationObject*(DDSegmentation::BitField64*),__LINE__)}

// Call function of the type [long (*func)(const char* arg)]
#define DECLARE_APPLY(name,func)        DD4HEP_OPEN_PLUGIN(DD4hep,name)   { \
    template <> long ApplyFactory<name>::create(ns::LCDD& l,int n,char** a) {return func(l,n,a);} \
    DD4HEP_PLUGINSVC_FACTORY(name,name,long(ns::LCDD*,int,char**),__LINE__)}

// Call function of the type [void* (*func)(const char* arg)]
#define DECLARE_CONSTRUCTOR(name,func)  DD4HEP_OPEN_PLUGIN(DD4hep,name) { \
    template <> void* ConstructionFactory<name>::create(const char* n) { return func(n);} \
    DD4HEP_PLUGINSVC_FACTORY(name,name,void*(const char*),__LINE__) }

// Call function of the type [void* (*func)(ns::LCDD& lcdd, int argc,char** argv)]
#define DECLARE_LCDD_CONSTRUCTOR(name,func)  DD4HEP_OPEN_PLUGIN(DD4hep,name) { \
    template <> void* LCDDConstructionFactory<name>::create(ns::LCDD& l, int n,char** a) { return func(l,n,a);} \
    DD4HEP_PLUGINSVC_FACTORY(name,name,void*(ns::LCDD*,int,char**),__LINE__) }

// Call function of the type [void* (*func)(ns::LCDD& lcdd)]
#define DECLARE_TRANSLATION(name,func)  DD4HEP_OPEN_PLUGIN(DD4hep,name)  { \
    template <> Geometry::Ref_t TranslationFactory<name>::create(ns::LCDD& l) {return func(l);} \
    DECLARE_NAMED_TRANSLATION_FACTORY(Geometry,name)  }

// Call function of the type [void* (*func)(ns::LCDD& lcdd, xml_h handle)]
#define DECLARE_XMLELEMENT(name,func)  DD4HEP_OPEN_PLUGIN(DD4hep,xml_element_##name)  {\
    template <> Geometry::Ref_t XMLElementFactory<xml_element_##name>::create(ns::LCDD& l,ns::xml_h e) {return func(l,e);} \
    DD4HEP_PLUGINSVC_FACTORY(xml_element_##name,name,NamedObject*(ns::LCDD*,ns::xml_h*),__LINE__)  }

// Call function of the type [long (*func)(ns::LCDD& lcdd, xml_h handle)]
#define DECLARE_XML_DOC_READER(name,func)  DD4HEP_OPEN_PLUGIN(DD4hep,xml_document_##name)  { \
    template <> long XMLDocumentReaderFactory<xml_document_##name>::create(ns::LCDD& l,ns::xml_h e) {return func(l,e);} \
    DD4HEP_PLUGINSVC_FACTORY(xml_document_##name,name##_XML_reader,long(ns::LCDD*,ns::xml_h*),__LINE__)  }

// Call function of the type [NamedObject* (*func)(ns::LCDD& lcdd, xml_h handle, ref_t reference)]
#define DECLARE_XML_PROCESSOR_BASIC(name,func,deprecated)  DD4HEP_OPEN_PLUGIN(DD4hep,det_element_##name) {\
    template <> Geometry::Ref_t XmlDetElementFactory< det_element_##name >::create(ns::LCDD& l,ns::xml_h e,ns::ref_t h) \
    { if (deprecated) warning_deprecated_xml_factory(#name); return func(l,e,h);} \
    DD4HEP_PLUGINSVC_FACTORY(det_element_##name,name,NamedObject*(ns::LCDD*,ns::xml_h*,Geometry::Ref_t*),__LINE__)  }

// Call function of the type [NamedObject* (*func)(ns::LCDD& lcdd, json_h handle, ref_t reference)]
#define DECLARE_JSON_PROCESSOR_BASIC(name,func)  DD4HEP_OPEN_PLUGIN(DD4hep,det_element_##name) { \
    template <> Geometry::Ref_t JsonDetElementFactory< det_element_##name >::create(ns::LCDD& l,ns::json_h e,ns::ref_t h) \
    { return func(l,e,h);}                                              \
    DD4HEP_PLUGINSVC_FACTORY(det_element_##name,name,NamedObject*(ns::LCDD*,ns::json_h*,ns::ref_t*),__LINE__)  }

#define DECLARE_XML_PROCESSOR(name,func)          DECLARE_XML_PROCESSOR_BASIC(name,func,0)
#define DECLARE_SUBDETECTOR(name,func)            DECLARE_XML_PROCESSOR_BASIC(name,func,0)
#define DECLARE_DETELEMENT(name,func)             DECLARE_XML_PROCESSOR_BASIC(name,func,0)
#define DECLARE_DEPRECATED_DETELEMENT(name,func)  DECLARE_XML_PROCESSOR_BASIC(name,func,1)

#define DECLARE_JSON_DETELEMENT(name,func)        DECLARE_JSON_PROCESSOR_BASIC(name,func)

#endif // DD4HEP_FACTORIES_H

