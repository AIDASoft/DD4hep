// $Id$
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
    static long create(lcdd_t& lcdd, int argc, char** argv);
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
    static ref_t create(lcdd_t& lcdd);
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
    static ref_t create(lcdd_t& lcdd, xml_h e);
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
    static long create(lcdd_t& lcdd, xml_h e);
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
    static long create(lcdd_t& lcdd, ref_t& handle, xml_h element);
  };

  /// Standard factory to create Detector elements from the compact XML representation.
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \date    2012/07/31
   *  \ingroup DD4HEP_GEOMETRY
   */
  template <typename T> class DetElementFactory : public PluginFactoryBase {
  public:
    static Geometry::Ref_t create(Geometry::LCDD& lcdd, XML::Handle_t e, Geometry::Ref_t sens);
  };
}

namespace {

  /// Forward declartion of the base factory template
  template <typename P, typename S> class Factory;

  struct ns  {
    typedef DD4hep::NamedObject     Named;
    typedef DD4hep::Geometry::LCDD  LCDD;
    typedef DD4hep::XML::Handle_t   xml_h;
    typedef DD4hep::Geometry::Ref_t ref_t;
  };

  DD4HEP_PLUGIN_FACTORY_ARGS_1(void*,const char*)  
  {    return DD4hep::ConstructionFactory<P>::create(a0);                               }
  DD4HEP_PLUGIN_FACTORY_ARGS_1(ns::Named*,ns::LCDD*)
  {    return DD4hep::TranslationFactory<P>::create(*a0).ptr();                         }
  DD4HEP_PLUGIN_FACTORY_ARGS_3(long,ns::LCDD*,int,char**)
  {    return make_return<long>(DD4hep::ApplyFactory<P>::create(*a0,a1,a2));            }
  DD4HEP_PLUGIN_FACTORY_ARGS_2(ns::Named*,ns::LCDD*,ns::xml_h*)
  {    return DD4hep::XMLElementFactory<P>::create(*a0,*a1).ptr();                      }
  DD4HEP_PLUGIN_FACTORY_ARGS_2(long,ns::LCDD*,ns::xml_h*)
  {    return make_return<long>(DD4hep::XMLDocumentReaderFactory<P>::create(*a0,*a1));  }
  DD4HEP_PLUGIN_FACTORY_ARGS_3(ns::Named*,ns::LCDD*,ns::xml_h*,ns::ref_t*)
  {    return DD4hep::DetElementFactory<P>::create(*a0,*a1,*a2).ptr();                  }
}

#define DECLARE_DETELEMENT_FACTORY(x)               namespace DD4hep    \
  { DD4HEP_PLUGINSVC_FACTORY(x,x,DD4hep::NamedObject*(Geometry::LCDD*,XML::Handle_t*,Geometry::Ref_t*),__LINE__) }
#define DECLARE_NAMESPACE_DETELEMENT_FACTORY(n,x)   namespace DD4hep    \
  { DD4HEP_PLUGINSVC_FACTORY(n::x,x,NamedObject*(Geometry::LCDD*,XML::Handle_t*,Geometry::Ref_t*),__LINE__)      }
#define DECLARE_NAMED_APPLY_FACTORY(n,x)            namespace DD4hep    \
  { DD4HEP_PLUGINSVC_FACTORY(n::x,x,long(Geometry::LCDD*,int, char**),__LINE__) }
#define DECLARE_NAMED_TRANSLATION_FACTORY(n,x)      namespace DD4hep    \
  { DD4HEP_PLUGINSVC_FACTORY(n::x,x,DD4hep::NamedObject*(Geometry::LCDD*),__LINE__) }
#define DECLARE_NAMED_XMLELEMENT_FACTORY(n,x)       namespace DD4hep    \
  { DD4HEP_PLUGINSVC_FACTORY(n::x,x,DD4hep::NamedObject*(Geometry::LCDD*,XML::Handle_t*),__LINE__) }
#define DECLARE_NAMED_DETELEMENT_FACTORY(n,x)       namespace DD4hep    \
  { DD4HEP_PLUGINSVC_FACTORY(n::x,x,DD4hep::NamedObject*(Geometry::LCDD*,XML::Handle_t*,Geometry::Ref_t*),__LINE__) }

#define DECLARE_APPLY(name,func)        DD4HEP_OPEN_PLUGIN(DD4hep,name)   { \
    template <> long ApplyFactory<name>::create(lcdd_t& l,int n,char** a) {return func(l,n,a);} \
    DD4HEP_PLUGINSVC_FACTORY(name,name,long(Geometry::LCDD*,int,char**),__LINE__)}

#define DECLARE_CONSTRUCTOR(name,func)  DD4HEP_OPEN_PLUGIN(DD4hep,name) { \
    template <> void* ConstructionFactory<name>::create(const char* n) { return func(n);} \
    DD4HEP_PLUGINSVC_FACTORY(name,name,void*(const char*),__LINE__) }

#define DECLARE_TRANSLATION(name,func)  DD4HEP_OPEN_PLUGIN(DD4hep,name)  { \
    template <> Geometry::Ref_t TranslationFactory<name>::create(lcdd_t& l) {return func(l);} \
    DECLARE_NAMED_TRANSLATION_FACTORY(Geometry,name)  }

#define DECLARE_XMLELEMENT(name,func)  DD4HEP_OPEN_PLUGIN(DD4hep,xml_element_##name)  {\
    template <> Geometry::Ref_t XMLElementFactory<xml_element_##name>::create(lcdd_t& l,xml_h e) {return func(l,e);} \
    DD4HEP_PLUGINSVC_FACTORY(xml_element_##name,name,NamedObject*(Geometry::LCDD*,XML::Handle_t*),__LINE__)  }

#define DECLARE_XML_DOC_READER(name,func)  DD4HEP_OPEN_PLUGIN(DD4hep,xml_document_##name)  { \
    template <> long XMLDocumentReaderFactory<xml_document_##name>::create(lcdd_t& l,xml_h e) {return func(l,e);} \
    DD4HEP_PLUGINSVC_FACTORY(xml_document_##name,name##_XML_reader,long(Geometry::LCDD*,XML::Handle_t*),__LINE__)  }

#define DECLARE_XML_PROCESSOR_BASIC(name,func,deprecated)  DD4HEP_OPEN_PLUGIN(DD4hep,det_element_##name) {\
    template <> Geometry::Ref_t DetElementFactory< det_element_##name >::create(lcdd_t& l,xml_h e,ref_t h) \
    { if (deprecated) Geometry::warning_deprecated_xml_factory(#name); return func(l,e,h);} \
    DD4HEP_PLUGINSVC_FACTORY(det_element_##name,name,NamedObject*(Geometry::LCDD*,XML::Handle_t*,Geometry::Ref_t*),__LINE__)  }

#define DECLARE_XML_PROCESSOR(name,func)          DECLARE_XML_PROCESSOR_BASIC(name,func,0)
#define DECLARE_SUBDETECTOR(name,func)            DECLARE_XML_PROCESSOR_BASIC(name,func,0)
#define DECLARE_DETELEMENT(name,func)             DECLARE_XML_PROCESSOR_BASIC(name,func,0)
#define DECLARE_DEPRECATED_DETELEMENT(name,func)  DECLARE_XML_PROCESSOR_BASIC(name,func,1)

#endif // DD4HEP_FACTORIES_H

