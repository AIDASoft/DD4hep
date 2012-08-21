// $Id:$
//====================================================================
//  AIDA Detector description 
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_FACTORIES_H
#define DD4HEP_FACTORIES_H

#ifndef __CINT__
#include "Reflex/PluginService.h"
#endif
#include "RVersion.h"
#include "DD4hep/Detector.h"
#include "XML/XMLElements.h"

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

    /** @class TranslationFactory Factories.h DDCore/Factories.h
     *  Specialized factory to translate objects, which can be retrieved from LCDD
     *  Example: Translate DD4hep geometry to Geant4
     *
     *  @author  M.Frank
     *  @version 1.0
     *  @date    2012/07/31
     */
    template <typename T> class TranslationFactory  {
    public:
      static Ref_t create(LCDD& lcdd);
    };

    /** @class XMLElementFactory Factories.h DDCore/Factories.h
     *  Create an arbitrary object from it's XML representation.
     *
     *  @author  M.Frank
     *  @version 1.0
     *  @date    2012/07/31
     */
    template <typename T> class XMLElementFactory  {
    public:
      static Ref_t create(LCDD& lcdd, const XML::Handle_t& e);
    };

    /** @class XMLDocumentReaderFactory Factories.h DDCore/Factories.h
     *  Read an arbitrary XML document and analyze it's content
     *
     *  @author  M.Frank
     *  @version 1.0
     *  @date    2012/07/31
     */
    template <typename T> class XMLDocumentReaderFactory  {
    public:
      static long create(LCDD& lcdd, const XML::Handle_t& e);
    };

    /** @class DetElementFactory Factories.h DDCore/Factories.h
     *  Standard factory to create Detector elements from the compact
     *  XML representation.
     *
     *  @author  M.Frank
     *  @version 1.0
     *  @date    2012/07/31
     */
    template <typename T> class DetElementFactory  {
    public:
      static Ref_t create(LCDD& lcdd, const XML::Handle_t& e, SensitiveDetector& sens);
    };
  }  
}

namespace {
  template < typename P > class Factory<P, TNamed*(DD4hep::Geometry::LCDD*)> {
  public:
    typedef DD4hep::Geometry::LCDD  LCDD;
    typedef DD4hep::Geometry::Ref_t Ref_t;
    static void Func(void *retaddr, void*, const std::vector<void*>& arg, void*) {
      LCDD*  lcdd = (LCDD* )arg[0];
      Ref_t handle = DD4hep::Geometry::TranslationFactory<P>::create(*lcdd);
      *(void**)retaddr = handle.ptr();
    }
  };


  template < typename P > class Factory<P, TNamed*(DD4hep::Geometry::LCDD*,const DD4hep::XML::Handle_t*)> {
  public:
    typedef DD4hep::Geometry::LCDD  LCDD;
    typedef DD4hep::XML::Handle_t   xml_h;
    typedef DD4hep::Geometry::Ref_t Ref_t;
    static void Func(void *retaddr, void*, const std::vector<void*>& arg, void*) {
      LCDD*  lcdd = (LCDD* )arg[0];
      xml_h* elt  = (xml_h*)arg[1];
      Ref_t handle = DD4hep::Geometry::XMLElementFactory<P>::create(*lcdd,*elt);
      *(void**)retaddr = handle.ptr();
    }
  };


  template < typename P > class Factory<P, long(DD4hep::Geometry::LCDD*,const DD4hep::XML::Handle_t*)> {
  public:
    typedef DD4hep::Geometry::LCDD  LCDD;
    typedef DD4hep::XML::Handle_t   xml_h;
    static void Func(void *retaddr, void*, const std::vector<void*>& arg, void*) {
      LCDD*  lcdd = (LCDD* )arg[0];
      xml_h* elt  = (xml_h*)arg[1];
      long ret = DD4hep::Geometry::XMLDocumentReaderFactory<P>::create(*lcdd,*elt);
      new(retaddr) (long)(ret);
    }
  };


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

#define DECLARE_DETELEMENT_FACTORY(x) \
  PLUGINSVC_FACTORY(x,TNamed*(DD4hep::Geometry::LCDD*,const DD4hep::XML::Handle_t*,DD4hep::Geometry::SensitiveDetector*))
#define DECLARE_NAMESPACE_DETELEMENT_FACTORY(n,x)  using n::x; \
  PLUGINSVC_FACTORY(x,TNamed*(DD4hep::Geometry::LCDD*,const DD4hep::XML::Handle_t*,DD4hep::Geometry::SensitiveDetector*))

#define DECLARE_NAMED_TRANSLATION_FACTORY(n,x)  using n::x;\
  PLUGINSVC_FACTORY_WITH_ID(x,std::string(#x),TNamed*(DD4hep::Geometry::LCDD*))
#define DECLARE_NAMED_XMLELEMENT_FACTORY(n,x)  using n::x;\
  PLUGINSVC_FACTORY_WITH_ID(x,std::string(#x),TNamed*(DD4hep::Geometry::LCDD*,const DD4hep::XML::Handle_t*))
#define DECLARE_NAMED_DETELEMENT_FACTORY(n,x)  using n::x;\
  PLUGINSVC_FACTORY_WITH_ID(x,std::string(#x),TNamed*(DD4hep::Geometry::LCDD*,const DD4hep::XML::Handle_t*,DD4hep::Geometry::SensitiveDetector*))

#define DECLARE_TRANSLATION(name,func) \
  namespace DD4hep { namespace Geometry { namespace { struct name {}; }            \
  template <> Ref_t TranslationFactory<name>::create(LCDD& l) {return func(l);} }} \
  DECLARE_NAMED_TRANSLATION_FACTORY(DD4hep::Geometry,name)

#define DECLARE_XMLELEMENT(name,func) \
  namespace DD4hep { namespace Geometry { namespace { struct xml_element_##name {}; }                \
  using DD4hep::Geometry::xml_element_##name;					                     \
  template <> Ref_t XMLElementFactory<xml_element_##name>::create(LCDD& l,const XML::Handle_t& e) {return func(l,e);} }}\
  PLUGINSVC_FACTORY_WITH_ID(xml_element_##name,std::string(#name),TNamed*(DD4hep::Geometry::LCDD*,const DD4hep::XML::Handle_t*))

#define DECLARE_XML_DOC_READER(name,func) \
  namespace DD4hep { namespace Geometry { namespace { struct xml_document_##name {}; }               \
  using DD4hep::Geometry::xml_document_##name;					                     \
  template <> long XMLDocumentReaderFactory<xml_document_##name>::create(LCDD& l,const XML::Handle_t& e) {return func(l,e);} }}\
  PLUGINSVC_FACTORY_WITH_ID(xml_document_##name,std::string(#name "_XML_reader"),long(DD4hep::Geometry::LCDD*,const DD4hep::XML::Handle_t*))

#define DECLARE_DETELEMENT(name,func) \
  namespace DD4hep { namespace Geometry { namespace { struct det_element_##name {}; }                                       \
  using DD4hep::Geometry::det_element_##name;                                                                               \
  template <> Ref_t DetElementFactory<det_element_##name>::create(LCDD& l,const XML::Handle_t& e,SensitiveDetector& s){return func(l,e,s);}}}\
  PLUGINSVC_FACTORY_WITH_ID(det_element_##name,std::string(#name),TNamed*(DD4hep::Geometry::LCDD*,const DD4hep::XML::Handle_t*,DD4hep::Geometry::SensitiveDetector*))

#endif // DD4HEP_FACTORIES_H
