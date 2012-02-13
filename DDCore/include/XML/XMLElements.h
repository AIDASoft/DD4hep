// $Id:$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4hep_HandleS_H
#define DD4hep_HandleS_H

#include <cmath>
#include <string>
#include <stdexcept>
#include "xercesc/util/XMLString.hpp"
#include "xercesc/dom/DOMElement.hpp"
#include "xercesc/dom/DOMDocument.hpp"
#include "xercesc/dom/DOMNodeList.hpp"

#ifndef RAD_2_DEGREE
  #define RAD_2_DEGREE 57.295779513082320876798154814105
#endif
#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif

/*
 *   Gaudi namespace declaration
 */
namespace DD4hep {

  template<class T> void deletePtr(T*& p) { if(p) delete p; p=0; }
  namespace XML  {

    typedef xercesc::DOMAttr*  Attribute;
    typedef const std::string& CSTR;

    std::string _toString(const Attribute attr);
    std::string _toString(const XMLCh *toTranscode);
    std::string _toString(unsigned long i, const char* fmt="%ul");
    std::string _toString(unsigned int i, const char* fmt="%u");
    std::string _toString(int i,    const char* fmt="%d");
    std::string _toString(float  d, const char* fmt="%f");
    std::string _toString(double d, const char* fmt="%f");
    void        _toDictionary(const XMLCh* name, const XMLCh* value);
    bool   _toBool(const XMLCh* value);
    int    _toInt(const XMLCh* value);
    long   _toLong(const XMLCh* value);
    float  _toFloat(const XMLCh* value);
    double _toDouble(const XMLCh* value);
    void dumpTree(xercesc::DOMDocument* doc);


    struct Strng_t  {
      XMLCh* m_xml;
      Strng_t(const XMLCh* c)        { m_xml=xercesc::XMLString::replicate(c);          }
      Strng_t(const char* c)         { m_xml=xercesc::XMLString::transcode(c);          }
      Strng_t(CSTR c)                { m_xml=xercesc::XMLString::transcode(c.c_str());  }
      ~Strng_t()                     { if (m_xml) xercesc::XMLString::release(&m_xml);  }
      operator const XMLCh*() const { return m_xml;                           }
      const XMLCh* ptr() const      { return m_xml;                           }
      Strng_t& operator=(const char* s);
      Strng_t& operator=(const Strng_t& s);
      Strng_t& operator=(const std::string& s);
    };
    
    Strng_t operator+(const Strng_t& a,     const char* b);
    Strng_t operator+(const char* a,        const Strng_t& b);
    Strng_t operator+(const Strng_t& a,     const XMLCh* b);
    Strng_t operator+(const XMLCh* a,       const Strng_t& b);
    Strng_t operator+(const XMLCh* a,       const std::string& b);
    Strng_t operator+(const std::string& a, const XMLCh* b);

    struct Tag_t : public Strng_t {
      std::string m_str;
      Tag_t(CSTR s) : Strng_t(s), m_str(s) {  }
      Tag_t(const char* s) : Strng_t(s), m_str(s) {  }
      Tag_t(const Strng_t& s) : Strng_t(s), m_str(_toString(s)) { }
      Tag_t(const XMLCh*  s) : Strng_t(s), m_str(_toString(s)) { }
      ~Tag_t()                            {  }
      Tag_t& operator=(const char* s);
      Tag_t& operator=(const Tag_t& s);
      Tag_t& operator=(const Strng_t& s);
      Tag_t& operator=(const std::string& s);
      operator CSTR () const    { return m_str; }
      CSTR str()  const         { return m_str; }
      const char* c_str() const { return m_str.c_str(); }
    };
    Tag_t   operator+(const Tag_t& a, const char* b);
    Tag_t   operator+(const char* a,  const Tag_t& b);
    Tag_t   operator+(const Tag_t& a, const XMLCh* b);
    Tag_t   operator+(const Tag_t& a, const Strng_t& b);
    Tag_t   operator+(const Tag_t& a, const std::string& b);

    inline bool operator==(CSTR c, const Tag_t& b)         {  return c == b.m_str;                       }

    struct Handle_t  {
      mutable xercesc::DOMElement* m_node;
      Handle_t(xercesc::DOMElement* e=0) : m_node(e) {}
      xercesc::DOMElement* operator->() const               { return m_node;                              }
      operator xercesc::DOMElement* () const                { return m_node;                              }
      xercesc::DOMElement* ptr() const                      { return m_node;                              }
      Handle_t clone(xercesc::DOMDocument* new_doc, bool deep) const;
      bool hasAttr(const XMLCh* t) const                    { return 0 != m_node->getAttributeNode(t);    }
      bool hasAttr(const char* t) const                     { return 0 != m_node->getAttributeNode(Strng_t(t));}
      template <class T> T attr(const XMLCh* t)  const;
      template <class T> T attr(const char* t)  const       { return this->attr<T>(Strng_t(t));            }
      void setAttrs(Handle_t e) const;
      Attribute setAttr(const XMLCh* t, const XMLCh* v) const;
      Attribute setAttr(const XMLCh* t, const Attribute v) const;
      Attribute setAttr(const XMLCh* t, int val)  const;
      Attribute setAttr(const XMLCh* t, bool val)  const;
      Attribute setAttr(const XMLCh* t, float val)  const;
      Attribute setAttr(const XMLCh* t, double val)  const;
      Attribute setAttr(const XMLCh* t, const char* v) const;

      const XMLCh* rawTag() const                           { return m_node->getTagName(); }
      const XMLCh* rawText() const                          { return m_node->getTextContent(); }
      const XMLCh* rawValue() const                         { return m_node->getNodeValue(); }
      std::string tag() const                               { return _toString(rawTag());     }
      std::string text() const                              { return _toString(rawText()); }
      std::string value() const                             { return _toString(rawValue());       }

      bool hasChild(const XMLCh* tag) const;
      Handle_t firstChild(const XMLCh* tag) const;
      Handle_t child(const XMLCh* tag, bool throw_exception=true) const;
      xercesc::DOMNodeList* children(const XMLCh* tag) const{ return m_node->getElementsByTagName (tag);   }
      size_t numChildren(const XMLCh* tag, bool throw_exception) const;
      Handle_t remove(xercesc::DOMElement* node)  const;
      void removeChildren(const XMLCh* tag)  const;
      void append(xercesc::DOMElement* e) const { m_node->appendChild(e); }
      const XMLCh* attr_value(const XMLCh* attr)  const;
      Attribute attr_ptr(const XMLCh* attr)  const;
      Attribute attr_nothrow(const XMLCh* tag)  const { return m_node->getAttributeNode(tag);  }
      const XMLCh* attr_value_nothrow(const XMLCh* attr)  const;
    };

#define INLINE inline
    template <> INLINE Attribute Handle_t::attr<Attribute>(const XMLCh* tag) const {
      return attr_ptr(tag);
    }

    typedef const XMLCh* cpXMLCh;
    template<> INLINE cpXMLCh Handle_t::attr<cpXMLCh>(const XMLCh* tag)  const  {
      return attr_value(tag);
    }

    template<> INLINE bool Handle_t::attr<bool>(const XMLCh* tag)  const  {
      return _toBool(attr_value_nothrow(tag));
    }

    template<> INLINE int Handle_t::attr<int>(const XMLCh* tag)  const  {
      return _toInt(attr_value(tag));
    }

    template<> INLINE float Handle_t::attr<float>(const XMLCh* tag)  const  {
      return _toFloat(attr_value(tag));
    }

    template<> INLINE double Handle_t::attr<double>(const XMLCh* tag)  const  {
      return _toDouble(attr_value(tag));
    }

    template<> INLINE std::string Handle_t::attr<std::string>(const XMLCh* tag)  const  {
      return _toString(attr_nothrow(tag));
    }

    struct Collection_t : public Handle_t {
      mutable XMLSize_t     m_index;
      xercesc::DOMElement*  m_element;
      xercesc::DOMNodeList* m_children;

      Collection_t(xercesc::DOMElement* node, const XMLCh* tag);
      Collection_t(xercesc::DOMElement* node, const char* tag);
      Collection_t(xercesc::DOMElement* node, xercesc::DOMNodeList* children);
      Collection_t& reset();
      size_t size()  const;
      void operator++()  const;
      xercesc::DOMElement* current() const           { return m_node;                              }
      template <class T> void for_each(T oper) const  {
        for(const Collection_t& c=*this; c; ++c)
          oper(*this);
      }
      template <class T> void for_each(const XMLCh* tag, T oper) const  {
        for(const Collection_t& c=*this; c; ++c)
          Collection_t(c.m_node, tag).for_each(oper);
      }
    };


    struct Document  {
      xercesc::DOMDocument     *m_doc;
      Document(xercesc::DOMDocument* d) : m_doc(d) {}
      operator xercesc::DOMDocument*() const   {  return m_doc; }
      xercesc::DOMDocument* operator->() const {  return m_doc; }
      xercesc::DOMElement* createElt(const XMLCh* tag) const;
      Attribute createAttr(const XMLCh* name, const XMLCh* value)  const;
      Handle_t clone(Handle_t source,bool deep)  const;
      Handle_t root() const;
    };

    struct DocumentHolder : public Document {
      DocumentHolder(xercesc::DOMDocument* d) : Document(d) {}
      virtual ~DocumentHolder();
    };

    struct Element  {
      Handle_t m_element;
      Element(const Handle_t& e) : m_element(e) {                 }
      Element(const Document& document, const XMLCh* type);
      operator bool() const                  {  return 0 != m_element.ptr(); }
      bool operator!() const                 {  return 0 == m_element.ptr(); }
      operator Handle_t () const             {  return m_element; }
      operator xercesc::DOMElement*() const  {  return m_element; }
      xercesc::DOMElement* ptr() const       {  return m_element; }
      Document document() const;
      const XMLCh* tagName() const  
      { return m_element.rawTag();              }
      void append(Handle_t handle) const  
      {  m_element.append(handle);              }
      Handle_t clone(const Document& new_doc, bool deep) const  
      {  return new_doc.clone(m_element,deep);  }
      size_t numChildren(const XMLCh* tag, bool throw_exception=true) const
      {  return m_element.numChildren(tag,throw_exception);     }
      bool hasAttr(const XMLCh* name) const   
      {  return m_element.hasAttr(name);        }
      template <class T> T attr(const XMLCh* tag) const 
      { return m_element.attr<T>(tag);          }
      void setAttrs(Handle_t e) const
      {  return m_element.setAttrs(e);          }
      template <class T> Attribute setAttr(const XMLCh* name,const T& value)  const
      {  return m_element.setAttr(name,value);  }
      Attribute getAttr(const XMLCh* name)  const;
      Handle_t clone(Handle_t h, bool deep) const;
      Handle_t addChild(const XMLCh* tag)  const;
      Handle_t setChild(const XMLCh* tag)  const;
      Handle_t child(const Strng_t& tag, bool throw_exception=true) const;
      Handle_t remove(Handle_t node)  const     
      { return m_element.remove(node);          }
      Attribute setRef(const XMLCh* tag, const XMLCh* refname)  const;
      const XMLCh* getRef(const XMLCh* tag)  const;
    };

    struct RefElement : public Element  {
      Attribute  m_name;
      RefElement(const Handle_t& e);
      RefElement(const RefElement& e);
      RefElement(const Document& d, const XMLCh* type, const XMLCh* name);
      const XMLCh* name() const;
      const XMLCh* refName() const;
      void setName(const XMLCh* new_name);
    };

     extern const Tag_t Attr_id;
     extern const Tag_t Attr_type;
     extern const Tag_t Attr_name;
     extern const Tag_t Attr_value;
     extern const Tag_t Attr_ref;
     extern const Tag_t Attr_NULL;
#undef INLINE

  }
}         /* End namespace DD4hep   */
#endif    /* DD4hep_HandleS_H    */
