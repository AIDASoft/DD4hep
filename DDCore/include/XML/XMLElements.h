// $Id:$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_XMLELEMENTS_H
#define DD4HEP_XMLELEMENTS_H

#include <cmath>
#include <string>
#include <vector>
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

    typedef xercesc::DOMNodeList* NodeList;
    typedef xercesc::DOMAttr*     Attribute;
    typedef const std::string&    CSTR;

    /// Dump DOM tree of a document
    void dumpTree(xercesc::DOMDocument* doc);

    std::string _toString(const Attribute attr);
    std::string _toString(const XMLCh *toTranscode);
    std::string _toString(unsigned long i, const char* fmt="%ul");
    std::string _toString(unsigned int i, const char* fmt="%u");
    std::string _toString(int i,    const char* fmt="%d");
    std::string _toString(float  d, const char* fmt="%f");
    std::string _toString(double d, const char* fmt="%f");
    void        _toDictionary(const XMLCh* name, const XMLCh* value);

    /// Conversion function from raw unicode string to bool
    bool        _toBool(const XMLCh* value);
    /// Conversion function from raw unicode string to int
    int         _toInt(const XMLCh* value);
    /// Conversion function from raw unicode string to long
    long        _toLong(const XMLCh* value);
    /// Conversion function from raw unicode string to float
    float       _toFloat(const XMLCh* value);
    /// Conversion function from raw unicode string to double
    double      _toDouble(const XMLCh* value);

    /** @class Strng_t XMLElements.h XML/XMLElements.h
     * 
     *  Helper class to encapsulate a unicode string
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Strng_t  {
      XMLCh* m_xml;
      Strng_t(const XMLCh* c)        { m_xml=xercesc::XMLString::replicate(c);          }
      Strng_t(const char* c)         { m_xml=xercesc::XMLString::transcode(c);          }
      Strng_t(const std::string& c)  { m_xml=xercesc::XMLString::transcode(c.c_str());  }
      ~Strng_t()                     { if (m_xml) xercesc::XMLString::release(&m_xml);  }
      operator const XMLCh*() const  { return m_xml;                                    }
      const XMLCh* ptr() const       { return m_xml;                                    }
      Strng_t& operator=(const char* s);
      Strng_t& operator=(const Strng_t& s);
      Strng_t& operator=(const std::string& s);
    };
    
    /// Unicode string concatenation of a normal ASCII string from right side
    Strng_t operator+(const Strng_t& a,     const char* b);
    /// Unicode string concatenation of a normal ASCII string from left side
    Strng_t operator+(const char* a,        const Strng_t& b);
    /// Unicode string concatenation of a encapsulated and raw unicode string from right side
    Strng_t operator+(const Strng_t& a,     const XMLCh* b);
    /// Unicode string concatenation of a encapsulated and raw unicode string from left side
    Strng_t operator+(const XMLCh* a,       const Strng_t& b);
    /// Unicode string concatenation of a raw unicode and an STL string from the right
    Strng_t operator+(const XMLCh* a,       const std::string& b);
    /// Unicode string concatenation of a raw unicode and an STL string from the left
    Strng_t operator+(const std::string& a, const XMLCh* b);

    /** @class Tag_t XMLElements.h XML/XMLElements.h
     * 
     *  Helper class to easily convert between 
     *  -- unicode
     *  -- std::string
     *  -- const char*.
     *  Internally a copy representation as an std::string is kept.
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Tag_t : public Strng_t {
      /// STL string buffer
      std::string m_str;

      /// Constructor from normal ASCII string
      Tag_t(const char* s) : Strng_t(s), m_str(s) {               }
      /// Constructor from unicode string
      Tag_t(const XMLCh*  s) : Strng_t(s), m_str(_toString(s)) {  }
      /// Constructor from internal XML string
      Tag_t(const Strng_t& s) : Strng_t(s), m_str(_toString(s)) { }
      /// Constructor from STL string
      Tag_t(const std::string& s) : Strng_t(s), m_str(s) {        }
      /// Destructor
      ~Tag_t()                            {                       }

      /// Assignment of a normal ASCII string
      Tag_t& operator=(const char* s);
      /// Assignment of a tag object
      Tag_t& operator=(const Tag_t& s);
      /// Assignment of a internal XML string
      Tag_t& operator=(const Strng_t& s);
      /// Assignment of a STL string
      Tag_t& operator=(const std::string& s);

      /// Automatic conversion to STL string
      operator const std::string& () const    { return m_str; }
      /// Access writable STL string
      const std::string& str()  const         { return m_str; }
      /// Access data buffer of STL string
      const char* c_str() const { return m_str.c_str(); }
    };
    
    /// Tag/string concatenation with a normal ASCII string from right side
    Tag_t   operator+(const Tag_t& a, const char* b);
    /// Tag/string concatenation with a normal ASCII string from left side
    Tag_t   operator+(const char* a,  const Tag_t& b);
    /// Tag/string concatenation with a unicode string from right side
    Tag_t   operator+(const Tag_t& a, const XMLCh* b);
    /// Tag/string concatenation with a internal XML string from right side
    Tag_t   operator+(const Tag_t& a, const Strng_t& b);
    /// Tag/string concatenation with a STL string from right side
    Tag_t   operator+(const Tag_t& a, const std::string& b);
    /// Equality operator between tag object and STL string
    inline bool operator==(const std::string& c, const Tag_t& b)  {  return c == b.m_str;                }

    /** @class Handle_t XMLElements.h XML/XMLElements.h
     * 
     *  Helper class to easily access the properties of single DOMElements.
     *
     *  Note: The assignmant operator as well as the copy constructor
     *  do not have to be implemented, they are aut-generated by the compiler!
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Handle_t  {
      // Abbreviation for internal use
      typedef  xercesc::DOMElement* Elt_t;

      /// The pointer to the DOMElement
      mutable Elt_t m_node;

      /// Initializing constructor
      Handle_t(Elt_t e=0) : m_node(e) {}
      /// Direct access to the DOMElement using the operator->
      Elt_t operator->() const                              { return m_node;                              }
      /// Direct access to the DOMElement by dereferencing
      operator Elt_t() const                                { return m_node;                              }
      /// Direct access to the DOMElement by function
      Elt_t ptr() const                                     { return m_node;                              }
      /// Clone the DOMelement - with the option of a deep copy
      Handle_t clone(xercesc::DOMDocument* new_doc, bool deep) const;

      /// Unicode text access to the element's tag
      const XMLCh* rawTag() const                           { return m_node->getTagName();                }
      /// Unicode text access to the element's text
      const XMLCh* rawText() const                          { return m_node->getTextContent();            }
      /// Unicode text access to the element's value
      const XMLCh* rawValue() const                         { return m_node->getNodeValue();              }
      /// Text access to the element's tag
      std::string tag() const                               { return _toString(rawTag());                 }
      /// Text access to the element's text
      std::string text() const                              { return _toString(rawText());                }
      /// Text access to the element's value
      std::string value() const                             { return _toString(rawValue());               }

      /*** DOM Attribute handling 
       */
      /// Access attribute name (throws exception if not present)
      const XMLCh* attr_name(const Attribute attr)  const;
      /// Access attribute value by the attribute  (throws exception if not present)
      const XMLCh* attr_value(const Attribute attr)  const;
      /// Access attribute value by the attribute's unicode name (throws exception if not present)
      const XMLCh* attr_value(const XMLCh* attr)  const;
      /// Access attribute value by the attribute's unicode name (no exception thrown if not present)
      const XMLCh* attr_value_nothrow(const XMLCh* attr)  const;

      /// Access attribute pointer by the attribute's unicode name (throws exception if not present)
      Attribute attr_ptr(const XMLCh* attr)  const;
      /// Access attribute pointer by the attribute's unicode name (no exception thrown if not present)
      Attribute attr_nothrow(const XMLCh* tag)  const       { return m_node->getAttributeNode(tag);       }

      /// Retrieve a collection of all attributes of this DOM element
      std::vector<Attribute> attributes()  const;
      /// Check for the existence of a named attribute
      bool hasAttr(const XMLCh* t) const                    { return 0 != m_node->getAttributeNode(t);    }
      /// Check for the existence of a named attribute
      bool hasAttr(const char* t) const                     { return hasAttr(Strng_t(t));                 }
      /// Access typed attribute value by the DOMAttr
      template <class T> T attr(const Attribute a)  const   { return this->attr<T>(this->attr_name(a));   }
      /// Access typed attribute value by it's name      
      template <class T> T attr(const char* name)   const   { return this->attr<T>(Strng_t(name));        }
      /// Access typed attribute value by it's unicode name
      template <class T> T attr(const XMLCh* name)  const;

      void setAttrs(Handle_t e) const;

      /// Generic attribute setter with unicode value
      Attribute setAttr(const XMLCh* t, const XMLCh* v) const;
      /// Generic attribute setter with DOMAttr value
      Attribute setAttr(const XMLCh* t, const Attribute v) const;
      /// Generic attribute setter with integer value
      Attribute setAttr(const XMLCh* t, int val)  const;
      /// Generic attribute setter with boolen value
      Attribute setAttr(const XMLCh* t, bool val)  const;
      /// Generic attribute setter with floating point value
      Attribute setAttr(const XMLCh* t, float val)  const;
      /// Generic attribute setter with double precision floating point value
      Attribute setAttr(const XMLCh* t, double val)  const;
      /// Generic attribute setter with text value
      Attribute setAttr(const XMLCh* t, const char* v) const;

      /*** DOM Element child handling 
       */
      /// Check the existence of a child with a given tag name
      bool hasChild(const XMLCh* tag) const;
      /// Access a single child by it's tag name (unicode)
      Handle_t child(const XMLCh* tag, bool throw_exception=true) const;
      /// Access a group of children identified by the same tag name
      NodeList children(const XMLCh* tag) const{ return m_node->getElementsByTagName (tag);   }
      /// Access the number of children of this DOM element with a given tag name
      size_t numChildren(const XMLCh* tag, bool throw_exception) const;
      /// Remove a single child node identified by it's handle from the tree of the element
      Handle_t remove(Handle_t e)  const;
      /// Remove children with a given tag name from the DOM node
      void removeChildren(const XMLCh* tag)  const;
      /// Append a DOM element to the current node
      void append(Handle_t e) const { m_node->appendChild(e.ptr()); }

    };

#define INLINE inline
    template <> INLINE Attribute Handle_t::attr<Attribute>(const XMLCh* tag) const 
      {      return attr_ptr(tag);                       }

    typedef const XMLCh* cpXMLCh;
    template<> INLINE cpXMLCh Handle_t::attr<cpXMLCh>(const XMLCh* tag)  const  
      {      return attr_value(tag);                     }

    template<> INLINE bool Handle_t::attr<bool>(const XMLCh* tag)  const  
      {      return _toBool(attr_value_nothrow(tag));    }

    template<> INLINE int Handle_t::attr<int>(const XMLCh* tag)  const  
      {      return _toInt(attr_value(tag));             }

    template<> INLINE float Handle_t::attr<float>(const XMLCh* tag)  const
      {      return _toFloat(attr_value(tag));           }

    template<> INLINE double Handle_t::attr<double>(const XMLCh* tag)  const  
      {      return _toDouble(attr_value(tag));          }

    template<> INLINE std::string Handle_t::attr<std::string>(const XMLCh* tag)  const  
      {      return _toString(attr_nothrow(tag));        }
#if 0
    template<> INLINE bool Handle_t::attr<bool>(const Attribute tag)  const  
      {      return _toBool(attr_value(tag));            }

    template<> INLINE int Handle_t::attr<int>(const Attribute tag)  const  
      {      return _toInt(attr_value(tag));             }

    template<> INLINE float Handle_t::attr<float>(const Attribute tag)  const  
      {      return _toFloat(attr_value(tag));           }

    template<> INLINE double Handle_t::attr<double>(const Attribute tag)  const  
      {      return _toDouble(attr_value(tag));          }

    template<> INLINE std::string Handle_t::attr<std::string>(const Attribute tag)  const  
      {      return _toString(attr_value(tag));          }
#endif


    /** @class Collection_t XMLElements.h XML/XMLElements.h
     * 
     *  Helper class to easily access collections of DOMNodes (or DOMElements)
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Collection_t : public Handle_t {
      /// Iteration type. See NodeType enumeration for details
      xercesc::DOMNode::NodeType m_type;
      /// Current iteration index
      mutable XMLSize_t          m_index;
      /// Reference to the list of child nodes
      NodeList                   m_children;

      /// Constructor to initialize loop over children with a given node type
      Collection_t(Elt_t node, xercesc::DOMNode::NodeType typ);
      /// Constructor over DOMElements with a given tag name
      Collection_t(Elt_t node, const XMLCh* tag);
      /// Constructor over DOMElements with a given tag name
      Collection_t(Elt_t node, const char* tag);
      /// Constructor over DOMElements in a node list
      Collection_t(NodeList children);
      /// Constructor over arbitrary DOMNodes in a node list
      Collection_t(NodeList children, xercesc::DOMNode::NodeType typ);
      /// Reset the collection object to restart the iteration
      Collection_t& reset();
      /// Access the collection size
      size_t size()  const;
      /// Operator to advance the collection (pre increment)
      void operator++()  const;
      /// Operator to advance the collection (post increment)
      void operator++(int)  const;
      /// Operator to advance the collection (pre decrement)
      void operator--()  const;
      /// Operator to advance the collection (post decrement)
      void operator--(int)  const;
      /// Access to current element
      Elt_t current() const { return m_node; }
      /// Loop processor using function object
      template <class T> void for_each(T oper) const  {
        for(const Collection_t& c=*this; c; ++c)
          oper(*this);
      }
      /// Loop processor using function object
      template <class T> void for_each(const XMLCh* tag, T oper) const  {
        for(const Collection_t& c=*this; c; ++c)
          Collection_t(c.m_node, tag).for_each(oper);
      }
    };


    /** @class Document XMLElements.h XML/XMLElements.h
     * 
     *  User class encapsulating a DOM document
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Document  {
      typedef xercesc::DOMDocument* DOC;
      DOC m_doc;

      /// Constructor
      Document(DOC d) : m_doc(d) {}
      /// Auto-conversion to DOM document
      operator DOC() const   {  return m_doc; }
      /// Accessot to DOM document behaviour using arrow operator
      DOC operator->() const {  return m_doc; }
      /// Access the ROOT eleemnt of the DOM document
      Handle_t  root() const;

      /// Create DOM element
      Handle_t  createElt(const XMLCh* tag) const;
      /// Create DOM attribute (name/value pair)
      Attribute createAttr(const XMLCh* name, const XMLCh* value)  const;
      /// Clone a DOM element / sub-tree depending on deep_copy flag
      Handle_t  clone(Handle_t source, bool deep_copy)  const;
    };

    struct DocumentHolder : public Document {
      DocumentHolder(DOC d) : Document(d) {}
      virtual ~DocumentHolder();
    };

    /** @class Document XMLElements.h XML/XMLElements.h
     * 
     *  User class encapsulating a DOM element 
     *  using the Handle helper
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Element  {
      typedef Handle_t::Elt_t Elt_t;
      Handle_t m_element;

      /// Constructor from DOMElement handle
      Element(const Handle_t& e) : m_element(e) {                 }
      /// Constructor from DOM document entity
      Element(const Document& document, const XMLCh* type);
      /// Access the hosting document handle of this DOM element
      Document document() const;

      /// operator bool: check handle validity
      operator bool() const                                     {  return 0 != m_element.ptr();           }
      /// operator NOT: check handle validity
      bool operator!() const                                    {  return 0 == m_element.ptr();           }
      /// Automatic conversion to DOM element handle
      operator Handle_t () const                                {  return m_element;                      }
      /// Automatic conversion to DOMElement pointer
      operator Elt_t() const                                    {  return m_element;                      }
      /// Access to DOMElement pointer
      Elt_t ptr() const                                         {  return m_element;                      }

      /// Access the tag name of this DOM element
      const XMLCh* tagName() const                              {  return m_element.rawTag();             }
      /// Append a new element to the existing tree
      void append(Handle_t handle) const                        {  m_element.append(handle);              }
      /// Clone the DOMelement - with the option of a deep copy
      Handle_t clone(const Document& new_doc, bool deep) const  {  return new_doc.clone(m_element,deep);  }
      /// Access the number of children of this DOM element with a given tag name
      size_t numChildren(const XMLCh* tag, bool exc=true) const {  return m_element.numChildren(tag,exc); }
      /// Check for the existence of a named attribute
      bool hasAttr(const XMLCh* name) const                 {  return m_element.hasAttr(name);            }

      template <class T> T attr(const XMLCh* tag) const     {  return m_element.attr<T>(tag);             }
      void setAttrs(Handle_t e) const                       {  return m_element.setAttrs(e);              }
      template <class T> 
      Attribute setAttr(const XMLCh* nam,const T& val) const{  return m_element.setAttr(nam,val);         }
      /// Retrieve a collection of all attributes of this DOM element
      std::vector<Attribute> attributes()  const            {  return m_element.attributes();             }
      Attribute getAttr(const XMLCh* name)  const;

      Handle_t clone(Handle_t h, bool deep) const;
      Handle_t addChild(const XMLCh* tag)  const;
      Handle_t setChild(const XMLCh* tag)  const;
      Handle_t child(const Strng_t& tag, bool throw_exception=true) const;
      Handle_t remove(Handle_t node)  const                 { return m_element.remove(node);              }
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
#endif    /* DD4HEP_XMLELEMENTS_H   */
