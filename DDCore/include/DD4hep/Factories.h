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
#ifndef DD4HEP_FACTORIES_H
#define DD4HEP_FACTORIES_H

// Framework include files
#include "DD4hep/Plugins.h"
#include "DD4hep/DetElement.h"
#include "DD4hep/NamedObject.h"

// C/C++ include files
#include <cstdarg>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  // Forward declarations
  class Detector;
  class NamedObject;
  class SensitiveDetector;
  class SegmentationObject;
  class DetElement;

  /// Namespace for the AIDA detector description toolkit supporting JSON utilities
  namespace json {
    class Handle_t;
  }
  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace xml {
    class Handle_t;
    class RefElement;
  }

  /// Namespace describing generic detector segmentations
  namespace DDSegmentation  {
    class BitFieldCoder;
  }
  
  
  /// Template class with a generic constructor signature
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    2012/07/31
   *  \ingroup DD4HEP_CORE
   */
  template <typename T> class ConstructionFactory {
  public:
    static void* create(const char* arg);
  };

  /// Template class for a generic dd4hep object constructor
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    2012/07/31
   *  \ingroup DD4HEP_CORE
   */
  template <typename T> class DetectorConstructionFactory : public PluginFactoryBase {
  public:
    static void* create(Detector& description, int argc, char** argv);
  };

  /// Template class for a generic segmentation object constructor
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    2012/07/31
   *  \ingroup DD4HEP_CORE
   */
  template <typename T> class SegmentationFactory : public PluginFactoryBase {
  public:
    static SegmentationObject* create(const DDSegmentation::BitFieldCoder* decoder);
  };

  /// Template class with a generic signature to apply Detector plugins
  /**
   *  Specialized factory to translate objects, which can be retrieved from Detector
   *  Example: Translate dd4hep geometry to Geant4
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \date    2012/07/31
   *  \ingroup DD4HEP_CORE
   */
  template <typename T> class ApplyFactory : public PluginFactoryBase {
  public:
    static long create(Detector& description, int argc, char** argv);
  };

  /// Specialized factory to translate objects, which can be retrieved from Detector
  /**
   *  Example: Translate dd4hep geometry to Geant4
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \date    2012/07/31
   *  \ingroup DD4HEP_CORE
   */
  template <typename T> class TranslationFactory : public PluginFactoryBase {
  public:
    static Handle<NamedObject> create(Detector& description);
  };

  /// Create an arbitrary object from it's XML representation.
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \date    2012/07/31
   *  \ingroup DD4HEP_CORE
   */
  template <typename T> class XMLElementFactory : public PluginFactoryBase {
  public:
    static Handle<NamedObject> create(Detector& description, xml::Handle_t e);
  };

  /// Standard factory to create ROOT objects from an XML representation.
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \date    2012/07/31
   *  \ingroup DD4HEP_CORE
   */
  template <typename T> class XMLObjectFactory : public PluginFactoryBase {
  public:
    static Handle<TObject> create(Detector& description, xml::Handle_t e);
  };
  
  ///  Read an arbitrary XML document and analyze it's content
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \date    2012/07/31
   *  \ingroup DD4HEP_CORE
   */
  template <typename T> class XMLDocumentReaderFactory : public PluginFactoryBase {
  public:
    static long create(Detector& description, xml::Handle_t e);
  };

  /// Read an arbitrary XML document and analyze it's content
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \date    2012/07/31
   *  \ingroup DD4HEP_CORE
   */
  template <typename T> class XMLConversionFactory : public PluginFactoryBase {
  public:
    static long create(Detector& description, xml::RefElement& handle, xml::Handle_t element);
  };

  /// Standard factory to create Detector elements from an XML representation.
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \date    2012/07/31
   *  \ingroup DD4HEP_CORE
   */
  template <typename T> class XmlDetElementFactory : public PluginFactoryBase {
  public:
    static Handle<NamedObject> create(Detector& description, xml::Handle_t e, Handle<NamedObject> sens);
  };

  /// Standard factory to create Detector elements from the compact XML representation.
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \date    2012/07/31
   *  \ingroup DD4HEP_CORE
   */
  template <typename T> class JsonDetElementFactory : public PluginFactoryBase {
  public:
    static Handle<NamedObject> create(Detector& description, json::Handle_t e, Handle<NamedObject> sens);
  };
}

namespace {

  /// Forward declartion of the base factory template
  template <typename P, typename S> class Factory;

  /// Helper structure to shortcut type definitions for the factories
  namespace ns {
    typedef dd4hep::NamedObject         Named;
    typedef dd4hep::xml::Handle_t       xml_h;
    typedef dd4hep::json::Handle_t      json_h;
    typedef dd4hep::Handle<Named>       ref_t;
    typedef dd4hep::SegmentationObject  SegmentationObject;
    typedef dd4hep::DDSegmentation::BitFieldCoder   BitFieldCoder;
  }

  DD4HEP_PLUGIN_FACTORY_ARGS_1(void*,const char*)  
  {    return dd4hep::ConstructionFactory<P>::create(a0);                               }

  DD4HEP_PLUGIN_FACTORY_ARGS_1(ns::Named*,dd4hep::Detector*)
  {    return dd4hep::TranslationFactory<P>::create(*a0).ptr();                         }

  DD4HEP_PLUGIN_FACTORY_ARGS_1(ns::SegmentationObject*,const ns::BitFieldCoder*)
  {    return dd4hep::SegmentationFactory<P>::create(a0);                               }

  DD4HEP_PLUGIN_FACTORY_ARGS_3(void*,dd4hep::Detector*,int,char**)
  {    return dd4hep::DetectorConstructionFactory<P>::create(*a0,a1,a2);                }

  DD4HEP_PLUGIN_FACTORY_ARGS_3(long,dd4hep::Detector*,int,char**)
  {    return make_return<long>(dd4hep::ApplyFactory<P>::create(*a0,a1,a2));            }

  DD4HEP_PLUGIN_FACTORY_ARGS_2(ns::Named*,dd4hep::Detector*,ns::xml_h*)
  {    return dd4hep::XMLElementFactory<P>::create(*a0,*a1).ptr();                      }

  DD4HEP_PLUGIN_FACTORY_ARGS_2(TObject*,dd4hep::Detector*,ns::xml_h*)
  {    return dd4hep::XMLObjectFactory<P>::create(*a0,*a1).ptr();                       }

  DD4HEP_PLUGIN_FACTORY_ARGS_2(long,dd4hep::Detector*,ns::xml_h*)
  {    return make_return<long>(dd4hep::XMLDocumentReaderFactory<P>::create(*a0,*a1));  }

  DD4HEP_PLUGIN_FACTORY_ARGS_3(ns::Named*,dd4hep::Detector*,ns::xml_h*,ns::ref_t*)
  {    return dd4hep::XmlDetElementFactory<P>::create(*a0,*a1,*a2).ptr();               }

  DD4HEP_PLUGIN_FACTORY_ARGS_3(ns::Named*,dd4hep::Detector*,ns::json_h*,ns::ref_t*)
  {    return dd4hep::JsonDetElementFactory<P>::create(*a0,*a1,*a2).ptr();              }
}

#define DECLARE_DETELEMENT_FACTORY(x)               namespace dd4hep    \
  { DD4HEP_PLUGINSVC_FACTORY(x,x,dd4hep::NamedObject*(dd4hep::Detector*,xml::Handle_t*,Ref_t*),__LINE__) }
#define DECLARE_NAMESPACE_DETELEMENT_FACTORY(n,x)   namespace dd4hep    \
  { DD4HEP_PLUGINSVC_FACTORY(n::x,x,NamedObject*(dd4hep::Detector*,xml::Handle_t*,Ref_t*),__LINE__)      }
#define DECLARE_NAMED_APPLY_FACTORY(n,x)            namespace dd4hep    \
  { DD4HEP_PLUGINSVC_FACTORY(n::x,x,long(dd4hep::Detector*,int, char**),__LINE__) }
#define DECLARE_NAMED_TRANSLATION_FACTORY(n,x)      namespace dd4hep    \
  { DD4HEP_PLUGINSVC_FACTORY(n::x,x,dd4hep::NamedObject*(dd4hep::Detector*),__LINE__) }
#define DECLARE_NAMED_XMLELEMENT_FACTORY(n,x)       namespace dd4hep    \
  { DD4HEP_PLUGINSVC_FACTORY(n::x,x,dd4hep::NamedObject*(dd4hep::Detector*,xml::Handle_t*),__LINE__) }
#define DECLARE_NAMED_DETELEMENT_FACTORY(n,x)       namespace dd4hep    \
  { DD4HEP_PLUGINSVC_FACTORY(n::x,x,dd4hep::*(),__LINE__) }

// Call function of the type [SegmentationObject (*func)(dd4hep::Detector*,DDSegmentation::BitFieldCoder*)]
#define DECLARE_SEGMENTATION(name,func) DD4HEP_OPEN_PLUGIN(dd4hep,name)   { \
    template <> SegmentationObject*                           \
      SegmentationFactory<name>::create(const DDSegmentation::BitFieldCoder* d) { return func(d); } \
    DD4HEP_PLUGINSVC_FACTORY(name,segmentation_constructor__##name,     \
                             SegmentationObject*(const DDSegmentation::BitFieldCoder*),__LINE__)}

// Call function of the type [long (*func)(const char* arg)]
#define DECLARE_APPLY(name,func)        DD4HEP_OPEN_PLUGIN(dd4hep,name)   { \
    template <> long ApplyFactory<name>::create(dd4hep::Detector& l,int n,char** a) {return func(l,n,a);} \
    DD4HEP_PLUGINSVC_FACTORY(name,name,long(dd4hep::Detector*,int,char**),__LINE__)}

// Call function of the type [void* (*func)(const char* arg)]
#define DECLARE_CONSTRUCTOR(name,func)  DD4HEP_OPEN_PLUGIN(dd4hep,name) { \
    template <> void* ConstructionFactory<name>::create(const char* n) { return func(n);} \
    DD4HEP_PLUGINSVC_FACTORY(name,name,void*(const char*),__LINE__) }

// Call function of the type [void* (*func)(dd4hep::Detector& description, int argc,char** argv)]
#define DECLARE_DD4HEP_CONSTRUCTOR(name,func)  DD4HEP_OPEN_PLUGIN(dd4hep,name) { \
    template <> void* DetectorConstructionFactory<name>::create(dd4hep::Detector& l, int n,char** a) { return func(l,n,a);} \
    DD4HEP_PLUGINSVC_FACTORY(name,name,void*(dd4hep::Detector*,int,char**),__LINE__) }

// Call function of the type [void* (*func)(dd4hep::Detector& description)]
#define DECLARE_TRANSLATION(name,func)  DD4HEP_OPEN_PLUGIN(dd4hep,name)  { \
    template <> Ref_t TranslationFactory<name>::create(dd4hep::Detector& l) {return func(l);} \
    DECLARE_NAMED_TRANSLATION_FACTORY(Geometry,name)  }

// Call function of the type [void* (*func)(dd4hep::Detector& description, xml_h handle)]
#define DECLARE_XMLELEMENT(name,func)  DD4HEP_OPEN_PLUGIN(dd4hep,xml_element_##name)  {\
    template <> Ref_t XMLElementFactory<xml_element_##name>::create(dd4hep::Detector& l,ns::xml_h e) {return func(l,e);} \
    DD4HEP_PLUGINSVC_FACTORY(xml_element_##name,name,NamedObject*(dd4hep::Detector*,ns::xml_h*),__LINE__)  }

// Call function of the type [void* (*func)(dd4hep::Detector& description, xml_h handle)]
#define DECLARE_XML_SHAPE(name,func)  DD4HEP_OPEN_PLUGIN(dd4hep,xml_element_##name)  {\
    template <> Handle<TObject> XMLObjectFactory<xml_element_##name>::create(dd4hep::Detector& l,ns::xml_h e) {return func(l,e);} \
    DD4HEP_PLUGINSVC_FACTORY(xml_element_##name,name,TObject*(dd4hep::Detector*,ns::xml_h*),__LINE__)  }

// Call function of the type [long (*func)(dd4hep::Detector& description, xml_h handle)]
#define DECLARE_XML_DOC_READER(name,func)  DD4HEP_OPEN_PLUGIN(dd4hep,xml_document_##name)  { \
    template <> long XMLDocumentReaderFactory<xml_document_##name>::create(dd4hep::Detector& l,ns::xml_h e) {return func(l,e);} \
    DD4HEP_PLUGINSVC_FACTORY(xml_document_##name,name##_XML_reader,long(dd4hep::Detector*,ns::xml_h*),__LINE__)  }

// Call function of the type [NamedObject* (*func)(dd4hep::Detector& description, xml_h handle, ref_t reference)]
#define DECLARE_XML_PROCESSOR_BASIC(name,func,deprecated)  DD4HEP_OPEN_PLUGIN(dd4hep,det_element_##name) {\
    template <> Ref_t XmlDetElementFactory< det_element_##name >::create(dd4hep::Detector& l,ns::xml_h e,ns::ref_t h) \
    { if (deprecated) warning_deprecated_xml_factory(#name); return func(l,e,h);} \
    DD4HEP_PLUGINSVC_FACTORY(det_element_##name,name,NamedObject*(dd4hep::Detector*,ns::xml_h*,Ref_t*),__LINE__)  }

// Call function of the type [NamedObject* (*func)(dd4hep::Detector& description, json_h handle, ref_t reference)]
#define DECLARE_JSON_PROCESSOR_BASIC(name,func)  DD4HEP_OPEN_PLUGIN(dd4hep,det_element_##name) { \
    template <> Ref_t JsonDetElementFactory< det_element_##name >::create(dd4hep::Detector& l,ns::json_h e,ns::ref_t h) \
    { return func(l,e,h);}                                              \
    DD4HEP_PLUGINSVC_FACTORY(det_element_##name,name,NamedObject*(dd4hep::Detector*,ns::json_h*,ns::ref_t*),__LINE__)  }

#define DECLARE_XML_PROCESSOR(name,func)          DECLARE_XML_PROCESSOR_BASIC(name,func,0)
#define DECLARE_SUBDETECTOR(name,func)            DECLARE_XML_PROCESSOR_BASIC(name,func,0)
#define DECLARE_DETELEMENT(name,func)             DECLARE_XML_PROCESSOR_BASIC(name,func,0)
#define DECLARE_DEPRECATED_DETELEMENT(name,func)  DECLARE_XML_PROCESSOR_BASIC(name,func,1)

#define DECLARE_JSON_DETELEMENT(name,func)        DECLARE_JSON_PROCESSOR_BASIC(name,func)

#endif // DD4HEP_FACTORIES_H

