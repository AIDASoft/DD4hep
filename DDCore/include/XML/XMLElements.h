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
#ifndef DD4HEP_XMLELEMENTS_H
#define DD4HEP_XMLELEMENTS_H

// C/C++ include files
#include <cmath>
#include <string>
#include <vector>
#include <stdexcept>

// Framework include files
#include "XML/config.h"

#ifndef RAD_2_DEGREE
#define RAD_2_DEGREE 57.295779513082320876798154814105
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace xml {

    typedef const XmlAttr* Attribute;

    /// Definition of the XmlString class.
    /**
     *  Definition of the XmlString class.
     *  Unly used to have the same code base as for XercesC,
     *  where this class does the unicode translations and
     *  en/decoding.
     *
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup DD4HEP_XML
     */
    class XmlString {
    public:
      /// Replicate string: internally allocates new string, which must be free'ed with release
      static XmlChar* replicate(const XmlChar* c);
      /// Transcode string
      static XmlChar* transcode(const char* c);
      /// Release string
      static void release(char** p);
      /// String length in native representation
      static size_t length(const char* s);
#ifndef __TIXML__
      /// String length in native representation
      static size_t length(const XmlChar* s);
      /// Transcoded string from unicode to ascii
      static char* transcode(const XmlChar* c);
      /// Release string
      static void release(XmlChar** p);
#endif
    };

#ifdef __TIXML__
    /// Definition of the XmlException class.
    /**
     *  Currently no real use. Present to make Xerces happy,
     *  which has such a class.
     *
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup DD4HEP_XML
     */
    class XmlException {
    public:
      std::string msg;
      XmlException() : msg() {}
      XmlException(const std::string& m) : msg(m) {}
      XmlException(const XmlException& e) : msg(e.msg) {}
      virtual ~XmlException() {}
      XmlException& operator=(const XmlException& c) {
        if ( &c != this ) msg = c.msg;
        return *this;
      }
    };
#endif

    typedef const std::string& CSTR;

    /// Dump DOM tree of a document
    void dumpTree(XmlDocument* doc);

    /// Convert xml attribute to STL string  \ingroup DD4HEP_XML
    std::string _toString(const Attribute attr);
    /// Convert xml string to STL string  \ingroup DD4HEP_XML
    std::string _toString(const XmlChar *toTranscode);
    /// Do-nothing version. Present for completeness and argument interchangeability  \ingroup DD4HEP_XML
    std::string _toString(const char* s);
    /// Do-nothing version. Present for completeness and argument interchangeability  \ingroup DD4HEP_XML
    std::string _toString(const std::string& s);
    /// Format unsigned long integer to string with arbitrary format  \ingroup DD4HEP_XML
    std::string _toString(unsigned long i, const char* fmt = "%lu");
    /// Format unsigned integer (32 bits) to string with arbitrary format  \ingroup DD4HEP_XML
    std::string _toString(unsigned int i, const char* fmt = "%u");
    /// Format signed integer (32 bits) to string with arbitrary format  \ingroup DD4HEP_XML
    std::string _toString(int i, const char* fmt = "%d");
    /// Format signed long integer to string with arbitrary format  \ingroup DD4HEP_XML
    std::string _toString(long i, const char* fmt = "%ld");
    /// Format single procision float number (32 bits) to string with arbitrary format  \ingroup DD4HEP_XML
    std::string _toString(float d, const char* fmt = "%.17e");
    /// Format double procision float number (64 bits) to string with arbitrary format  \ingroup DD4HEP_XML
    std::string _toString(double d, const char* fmt = "%.17e");
    /// Format void pointer (64 bits) to string with arbitrary format  \ingroup DD4HEP_XML
    std::string _ptrToString(const void* p, const char* fmt = "%p");
    /// Format void pointer (64 bits) to string with arbitrary format  \ingroup DD4HEP_XML
    template <typename T> std::string _toString(const T* p, const char* fmt = "%p")
    {      return _ptrToString((void*)p,fmt);       }

    /// Helper function to populate the evaluator dictionary  \ingroup DD4HEP_XML
    void _toDictionary(const XmlChar* name, const XmlChar* value);
    /// Helper function to populate the evaluator dictionary  \ingroup DD4HEP_XML
    template <typename T> void _toDictionary(const XmlChar* name, T value);

    /// Helper function to populate the evaluator dictionary  \ingroup DD4HEP_XML
    void _toDictionary(const XmlChar* name, float  value);
    /// Helper function to populate the evaluator dictionary  \ingroup DD4HEP_XML
    void _toDictionary(const XmlChar* name, double value);

    /// Conversion function from raw unicode string to bool  \ingroup DD4HEP_XML
    bool _toBool(const XmlChar* value);
    /// Conversion function from raw unicode string to int  \ingroup DD4HEP_XML
    int _toInt(const XmlChar* value);
    /// Conversion function from raw unicode string to unsigned int  \ingroup DD4HEP_XML
    unsigned int _toUInt(const XmlChar* value);
    /// Conversion function from raw unicode string to long  \ingroup DD4HEP_XML
    long _toLong(const XmlChar* value);
    /// Conversion function from raw unicode string to unsigned long  \ingroup DD4HEP_XML
    unsigned long _toULong(const XmlChar* value);
    /// Conversion function from raw unicode string to float  \ingroup DD4HEP_XML
    float _toFloat(const XmlChar* value);
    /// Conversion function from raw unicode string to double  \ingroup DD4HEP_XML
    double _toDouble(const XmlChar* value);

    /// Helper function to lookup environment from the expression evaluator
    std::string getEnviron(const std::string& env);
    /// Enable/disable environment resolution when parsing strings
    bool enableEnvironResolution(bool new_value);
    
    /// Helper class to encapsulate a unicode string.
    /**
     *  Simple conversion from ascii strings to unicode strings.
     *  Useful when using XercesC - dummy implementation for TiXml.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_XML
     */
    class Strng_t {
    public:
      /// Pointer to unicode string
      XmlChar* m_xml;
#ifndef __TIXML__
      /// Initializing constructor from unicode string
      Strng_t(const XmlChar* c) {
        m_xml = XmlString::replicate(c);
      }
      /// Assignment opertor from unicode string
      Strng_t& operator=(const XmlChar* s);
#endif
      /// Initializing constructor from ascii string
      Strng_t(const char* c) {
        m_xml = XmlString::transcode(c);
      }
      /// Initializing constructor from STL string
      Strng_t(const std::string& c) {
        m_xml = XmlString::transcode(c.c_str());
      }
      /// Copy constructor
      Strng_t(const Strng_t& c) {
        m_xml = XmlString::replicate(c.m_xml);
      }
      /// Default destructor - release unicode string
      ~Strng_t() {
        if (m_xml)
          XmlString::release(&m_xml);
      }
      /// Auto-conversion to unicode string
      operator const XmlChar*() const {
        return m_xml;
      }
      /// Accessor to unicode string
      const XmlChar* ptr() const {
        return m_xml;
      }
      /// String length in native representation
      size_t length()  const  {
        return XmlString::length(m_xml);
      }
      /// Assignment opertor from ascii string
      Strng_t& operator=(const char* s);
      /// Assignment opertor from helper string
      Strng_t& operator=(const Strng_t& s);
      /// Assignment opertor from STL string
      Strng_t& operator=(const std::string& s);
    };

    /// Unicode string concatenation of a normal ASCII string from right side  \ingroup DD4HEP_XML
    Strng_t operator+(const Strng_t& a, const char* b);
    /// Unicode string concatenation of a encapsulated and an STL string from right side  \ingroup DD4HEP_XML
    Strng_t operator+(const Strng_t& a, const std::string& b);
    /// Unicode string concatenation of a encapsulated and an encapsulated string from right side  \ingroup DD4HEP_XML
    Strng_t operator+(const Strng_t& a, const Strng_t& b);
    /// Unicode string concatenation of a normal ASCII string from left side  \ingroup DD4HEP_XML
    Strng_t operator+(const char* a, const Strng_t& b);
    /// Unicode string concatenation of a STL string and an encapsulated string from the left.  \ingroup DD4HEP_XML
    Strng_t operator+(const std::string& a, const Strng_t& b);

#ifndef __TIXML__
    /// Unicode string concatenation of a encapsulated and raw unicode string from right side  \ingroup DD4HEP_XML
    Strng_t operator+(const Strng_t& a, const XmlChar* b);
    /// Unicode string concatenation of a encapsulated and raw unicode string from left side  \ingroup DD4HEP_XML
    Strng_t operator+(const XmlChar* a, const Strng_t& b);
    /// Unicode string concatenation of a raw unicode and an STL string from the right  \ingroup DD4HEP_XML
    Strng_t operator+(const XmlChar* a, const std::string& b);
    /// Unicode string concatenation of a raw unicode and an STL string from the left  \ingroup DD4HEP_XML
    Strng_t operator+(const std::string& a, const XmlChar* b);

#endif

    /// Class to support both way translation between C++ and XML strings
    /**
     *  Helper class to easily convert between
     *  -- unicode
     *  -- std::string
     *  -- const char*.
     *  Internaly a copy representation as an std::string is kept.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_XML
     */
    class Tag_t : public Strng_t {
    public:
      /// STL string buffer
      std::string m_str;
#ifndef __TIXML__
      /// Constructor from normal ASCII string
      Tag_t(const char* s)
        : Strng_t(s), m_str(s) {
      }
#endif
      /// Constructor from unicode string
      Tag_t(const XmlChar* s)
        : Strng_t(s), m_str(_toString(s)) {
      }
      /// Constructor from internal XML string
      Tag_t(const Strng_t& s)
        : Strng_t(s), m_str(_toString(s)) {
      }
      /// Constructor from STL string
      Tag_t(const std::string& s)
        : Strng_t(s), m_str(s) {
      }
      /// Constructor from STL string with registration.
      /// ONLY to be used for static global entries to protect against duplicated static memory.
      Tag_t(const std::string& v, const std::string& s, void (*register_func)(const std::string&, Tag_t*))
        : Strng_t(s), m_str(s) {
        register_func(v, this);
      }
      /// Copy constructor
      Tag_t(const Tag_t& c)
        : Strng_t(c), m_str(c.m_str) {
      }
      /// Destructor
      ~Tag_t() {
      }

      /// Assignment of a normal ASCII string
      Tag_t& operator=(const char* s);
      /// Assignment of a tag object
      Tag_t& operator=(const Tag_t& s);
      /// Assignment of a internal Xml string
      Tag_t& operator=(const Strng_t& s);
      /// Assignment of a STL string
      Tag_t& operator=(const std::string& s);

      /// Automatic conversion to STL string
      operator const std::string&() const {
        return m_str;
      }
      /// Access writable STL string
      const std::string& str() const {
        return m_str;
      }
      /// Access data buffer of STL string
      const char* c_str() const {
        return m_str.c_str();
      }
    };

    /// Tag/string concatenation with a normal ASCII string from right side  \ingroup DD4HEP_XML
    Tag_t operator+(const Tag_t& a, const char* b);
    /// Tag/string concatenation with a normal ASCII string from left side  \ingroup DD4HEP_XML
    Tag_t operator+(const char* a, const Tag_t& b);
    /// Tag/string concatenation with a unicode string from right side  \ingroup DD4HEP_XML
    Tag_t operator+(const Tag_t& a, const XmlChar* b);
    /// Tag/string concatenation with a internal Xml string from right side  \ingroup DD4HEP_XML
    Tag_t operator+(const Tag_t& a, const Strng_t& b);
    /// Tag/string concatenation with a STL string from right side  \ingroup DD4HEP_XML
    Tag_t operator+(const Tag_t& a, const std::string& b);
    /// Equality operator between tag object and STL string  \ingroup DD4HEP_XML
    inline bool operator==(const std::string& c, const Tag_t& b) {
      return c == b.m_str;
    }

    /// Convert Strng_t to std::string
    std::string _toString(const Strng_t& s);
    /// Convert Tag_t to std::string
    std::string _toString(const Tag_t& s);
    /// Helper function to populate the evaluator dictionary  \ingroup DD4HEP_XML
    void _toDictionary(const XmlChar* name, const Strng_t& s);
    /// Helper function to populate the evaluator dictionary  \ingroup DD4HEP_XML
    void _toDictionary(const XmlChar* name, const Tag_t& t);

    /// Class describing a list of XML nodes
    /**
     *  Definition of a "list" of xml elements hanging of the parent.
     *  The list allows iterations, which can be restarted.
     *  Please not: Copies are not entirely free, since the
     *  string tag need to be replicated using strdup/free
     *  (or when using xerces the corresponding unicode routines).
     *
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup DD4HEP_XML
     */
    class NodeList {
    public:
      Tag_t m_tag;
      XmlElement* m_node;
      mutable XmlElement* m_ptr;

      /// Copy constructor
      NodeList(const NodeList& l);
      /// Initializing constructor
      NodeList(XmlElement* frst, const XmlChar* t);
      /// Default destructor
      ~NodeList();
      /// Reset the nodelist - e.g. restart iteration from beginning
      XmlElement* reset();
      /// Advance to next element
      XmlElement* next() const;
      /// Go back to previous element
      XmlElement* previous() const;
      /// Assignment operator
      NodeList& operator=(const NodeList& l);
    };

    /// Class to easily access the properties of single XmlElements.
    /**
     *  Note: The assignmant operator as well as the copy constructor
     *  do not have to be implemented, they are aut-generated by the compiler!
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_XML
     */
    class Handle_t {
    public:
      // Abbreviation for internal use
      typedef XmlElement* Elt_t;

      /// The pointer to the XmlElement
      mutable Elt_t m_node;

      /// Initializing constructor
      Handle_t(Elt_t e = 0)
        : m_node(e) {
      }
      /// Direct access to the XmlElement using the operator->
      Elt_t operator->() const {
        return m_node;
      }
      /// Direct access to the XmlElement by dereferencing
      operator Elt_t() const {
        return m_node;
      }
      /// Direct access to the XmlElement by function
      Elt_t ptr() const {
        return m_node;
      }
      /// Clone the DOMelement - with the option of a deep copy
      Handle_t clone(XmlDocument* new_doc) const;

      /// Unicode text access to the element's tag. Tis must be wrong ....
      const XmlChar* rawTag() const;
      /// Unicode text access to the element's text
      const XmlChar* rawText() const;
      /// Unicode text access to the element's value
      const XmlChar* rawValue() const;
      /// Text access to the element's tag
      std::string tag() const {
        return _toString(rawTag());
      }
      /// Text access to the element's text
      std::string text() const {
        return _toString(rawText());
      }
      /// Text access to the element's value
      std::string value() const {
        return _toString(rawValue());
      }
      /// Set the element's value
      void setValue(const XmlChar* text) const;
      /// Set the element's value
      void setValue(const std::string& text) const;
      /// Set the element's text
      void setText(const XmlChar* text) const;
      /// Set the element's text
      void setText(const std::string& text) const;

      /*** DOM Attribute handling
       */
      /// Access attribute name (throws exception if not present)
      const XmlChar* attr_name(const Attribute attr) const;
      /// Access attribute value by the attribute  (throws exception if not present)
      const XmlChar* attr_value(const Attribute attr) const;
      /// Access attribute value by the attribute's unicode name (throws exception if not present)
      const XmlChar* attr_value(const XmlChar* attr) const;
      /// Access attribute value by the attribute's unicode name (no exception thrown if not present)
      const XmlChar* attr_value_nothrow(const XmlChar* attr) const;

      /// Access attribute pointer by the attribute's unicode name (throws exception if not present)
      Attribute attr_ptr(const XmlChar* attr) const;
      /// Access attribute pointer by the attribute's unicode name (no exception thrown if not present)
      Attribute attr_nothrow(const XmlChar* tag) const;
      /// Check for the existence of a named attribute
      bool hasAttr(const XmlChar* t) const;
      /// Retrieve a collection of all attributes of this DOM element
      std::vector<Attribute> attributes() const;
      /// Access typed attribute value by the XmlAttr
      template <class T> T attr(const Attribute a) const {
        return this->attr<T>(this->attr_name(a));
      }
      /// Remove all attributes of this element
      void removeAttrs() const;
      /// Set attributes as in argument handle
      void setAttrs(Handle_t e) const;
      /// Access typed attribute value by it's unicode name
      template <class T> T attr(const XmlChar* name) const;
      /// Access typed attribute value by it's unicode name. If not existing returns default value
      template <class T> T attr(const XmlChar* name, T default_value) const;

      /// Generic attribute setter with unicode value
      Attribute setAttr(const XmlChar* t, const XmlChar* v) const;
      /// Generic attribute setter with XmlAttr value
      Attribute setAttr(const XmlChar* t, const Attribute v) const;
      /// Generic attribute setter with integer value
      Attribute setAttr(const XmlChar* t, int val) const;
      /// Generic attribute setter with boolen value
      Attribute setAttr(const XmlChar* t, bool val) const;
      /// Generic attribute setter with floating point value
      Attribute setAttr(const XmlChar* t, float val) const;
      /// Generic attribute setter with double precision floating point value
      Attribute setAttr(const XmlChar* t, double val) const;
      /// Generic attribute setter with string value
      Attribute setAttr(const XmlChar* t, const std::string& val) const;
#ifndef __TIXML__
      /// Check for the existence of a named attribute
      bool hasAttr(const char* t) const {
        return hasAttr(Strng_t(t));
      }
      /// Access typed attribute value by it's name
      template <class T> T attr(const char* name) const {
        return this->attr<T>(Strng_t(name));
      }
      /// Access typed attribute value by it's name
      template <class T> T attr(const char* name, const T& default_value) const {
        Strng_t tag(name);
        return this->hasAttr(tag) ? this->attr<T>(tag) : default_value;
      }
      /// Generic attribute setter with text value
      Attribute setAttr(const XmlChar* t, const char* v) const;
#endif

      /// Add reference child as a new child node. The obj must have the "name" attribute!
      Handle_t setRef(const XmlChar* tag, const XmlChar* ref);
      /// Add reference child as a new child node. The obj must have the "name" attribute!
      Handle_t setRef(const XmlChar* tag, const std::string& ref);

      /*** DOM Element child handling
       */
      /// Check the existence of a child with a given tag name
      bool hasChild(const XmlChar* tag) const;
      /// Access a single child by it's tag name (unicode)
      Handle_t child(const XmlChar* tag, bool throw_exception = true) const;
      /// Access a group of children identified by the same tag name
      NodeList children(const XmlChar* tag) const;
      /// Access the number of children of this DOM element with a given tag name
      size_t numChildren(const XmlChar* tag, bool throw_exception) const;
      /// Remove a single child node identified by it's handle from the tree of the element
      Handle_t remove(Handle_t e) const;
      /// Remove children with a given tag name from the DOM node
      void removeChildren(const XmlChar* tag) const;
      /// Append a DOM element to the current node
      void append(Handle_t e) const;
      /// Access the element's parent element
      Handle_t parent() const;
      /// Checksum (sub-)tree of a xml document/tree. Default will pick up the adler32 checksum.
      unsigned int checksum(unsigned int param, unsigned int (fcn)(unsigned int param, const XmlChar*, size_t)=0) const;
    };

#define INLINE inline
    typedef const XmlChar* cpXmlChar;

    template <> INLINE Attribute Handle_t::attr<Attribute>(const XmlChar* tag_value) const {
      return attr_ptr(tag_value);
    }

    template <> INLINE cpXmlChar Handle_t::attr<cpXmlChar>(const XmlChar* tag_value) const {
      return attr_value(tag_value);
    }

    template <> INLINE bool Handle_t::attr<bool>(const XmlChar* tag_value) const {
      return _toBool(attr_value(tag_value));
    }

    template <> INLINE int Handle_t::attr<int>(const XmlChar* tag_value) const {
      return _toInt(attr_value(tag_value));
    }
    
    template <> INLINE unsigned int Handle_t::attr<unsigned int>(const XmlChar* tag_value) const {
      return _toUInt(attr_value(tag_value));
    }
    
    template <> INLINE long Handle_t::attr<long>(const XmlChar* tag_value) const {
      return _toLong(attr_value(tag_value));
    }

    template <> INLINE unsigned long Handle_t::attr<unsigned long>(const XmlChar* tag_value) const {
      return _toULong(attr_value(tag_value));
    }

    template <> INLINE float Handle_t::attr<float>(const XmlChar* tag_value) const {
      return _toFloat(attr_value(tag_value));
    }

    template <> INLINE double Handle_t::attr<double>(const XmlChar* tag_value) const {
      return _toDouble(attr_value(tag_value));
    }

    template <> INLINE std::string Handle_t::attr<std::string>(const XmlChar* tag_value) const {
      return _toString(attr_value(tag_value));
    }

    template <> INLINE bool Handle_t::attr<bool>(const XmlChar* tag_value, bool default_value) const {
      Attribute a = attr_nothrow(tag_value);
      return a ? _toBool(attr_value(a)) : default_value;
    }

    template <> INLINE int Handle_t::attr<int>(const XmlChar* tag_value, int default_value) const {
      Attribute a = attr_nothrow(tag_value);
      return a ? _toInt(attr_value(a)) : default_value;
    }
    
    template <> INLINE unsigned int Handle_t::attr<unsigned int>(const XmlChar* tag_value, unsigned int default_value) const {
      Attribute a = attr_nothrow(tag_value);
      return a ? _toUInt(attr_value(a)) : default_value;
    }
    
    template <> INLINE long Handle_t::attr<long>(const XmlChar* tag_value, long default_value) const {
      Attribute a = attr_nothrow(tag_value);
      return a ? _toLong(attr_value(a)) : default_value;
    }

    template <> INLINE unsigned long Handle_t::attr<unsigned long>(const XmlChar* tag_value, unsigned long default_value) const {
      Attribute a = attr_nothrow(tag_value);
      return a ? _toULong(attr_value(a)) : default_value;
    }

    template <> INLINE float Handle_t::attr<float>(const XmlChar* tag_value, float default_value) const {
      Attribute a = attr_nothrow(tag_value);
      return a ? _toFloat(attr_value(a)) : default_value;
    }

    template <> INLINE double Handle_t::attr<double>(const XmlChar* tag_value, double default_value) const {
      Attribute a = attr_nothrow(tag_value);
      return a ? _toDouble(attr_value(a)) : default_value;
    }

    template <> INLINE std::string Handle_t::attr<std::string>(const XmlChar* tag_value, std::string default_value) const {
      Attribute a = attr_nothrow(tag_value);
      return a ? _toString(attr_value(a)) : default_value;
    }

#if 0
    template<> INLINE bool Handle_t::attr<bool>(const Attribute tag_value) const
    { return _toBool(attr_value(tag_value));}

    template<> INLINE int Handle_t::attr<int>(const Attribute tag_value) const
    { return _toInt(attr_value(tag_value));}

    template<> INLINE float Handle_t::attr<float>(const Attribute tag_value) const
    { return _toFloat(attr_value(tag_value));}

    template<> INLINE double Handle_t::attr<double>(const Attribute tag_value) const
    { return _toDouble(attr_value(tag_value));}

    template<> INLINE std::string Handle_t::attr<std::string>(const Attribute tag_value) const
    { return _toString(attr_value(tag_value));}
#endif

    /// Class to support the access to collections of XmlNodes (or XmlElements)
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_XML
     */
    class Collection_t : public Handle_t {
    public:
      /// Reference to the list of child nodes
      NodeList m_children;
#ifndef __TIXML__
      /// Constructor over XmlElements with a given tag name
      Collection_t(Handle_t node, const XmlChar* tag);
#endif
      /// Constructor over XmlElements with a given tag name
      Collection_t(Handle_t node, const char* tag);
      /// Constructor over XmlElements in a node list
      Collection_t(NodeList children);
      /// Reset the collection object to restart the iteration
      Collection_t& reset();
      /// Access the collection size. Avoid this call -- sloooow!
      size_t size() const;
      /// Operator to advance the collection (pre increment)
      void operator++() const;
      /// Operator to advance the collection (post increment)
      void operator++(int) const;
      /// Operator to advance the collection (pre decrement)
      void operator--() const;
      /// Operator to advance the collection (post decrement)
      void operator--(int) const;
      /// Access to current element
      Elt_t current() const {
        return m_node;
      }
      /// Helper function to throw an exception
      void throw_loop_exception(const std::exception& e) const;
      /// Loop processor using function object
      template <class T> void for_each(T oper) const {
        try {
          for (const Collection_t& c = *this; c; ++c)
            oper(*this);
        }
        catch (const std::exception& e) {
          throw_loop_exception(e);
        }
      }
      /// Loop processor using function object
      template <class T> void for_each(const XmlChar* tag_name, T oper) const {
        try {
          for (const Collection_t& c = *this; c; ++c)
            Collection_t(c.m_node, tag_name).for_each(oper);
        }
        catch (const std::exception& e) {
          throw_loop_exception(e);
        }
      }
    };

    /// Class supporting the basic functionality of an XML document
    /**
     *  User class encapsulating a DOM document.
     *  Nothing special - normal handle around pointer.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_XML
     */
    class Document {
    public:
      typedef XmlDocument* DOC;
      DOC m_doc;

      /// Default Constructor
      Document() : m_doc(0) {}
      /// Initializing Constructor
      Document(DOC d) : m_doc(d) {}
      /// Copy constructor
      Document(const Document& d) = default;
      /// Assignment
      Document& operator=(const Document& d) = default;
      /// Destructor
      ~Document() = default;
      /// Auto-conversion to DOM document
      operator DOC() const       {        return m_doc;      }
      /// Accessot to DOM document behaviour using arrow operator
      DOC operator->() const     {        return m_doc;      }
      /// Accessot to DOM document behaviour
      DOC ptr() const            {        return m_doc;      }

      /// Access the ROOT eleemnt of the DOM document
      Handle_t root() const;
      /// Create DOM element
      Handle_t createElt(const XmlChar* tag) const;
      /// Clone a DOM element / sub-tree
      Handle_t clone(Handle_t source) const;
      /// Acces the document URI
      std::string uri() const;
    };

    /// Class supporting the basic functionality of an XML document including ownership
    /**
     *  User class encapsulating a DOM document.
     *  Nothing special - normal handle around pointer.
     *  Contrary to the Document class, on destruction the
     *  XML document shall be destroyed and the corresponding
     *  resources released.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_XML
     */
    class DocumentHolder : public Document {
    public:
      /// Default Constructor
      DocumentHolder() = default;
      /// Default Constructor
      DocumentHolder(const DocumentHolder& copy) = delete;
      /// Constructor
      DocumentHolder(DOC d) : Document(d) { }
      /// Assignment operator
      DocumentHolder& operator=(const DocumentHolder& copy) = delete;
      /// Assign new document. Old document is dropped.
      DocumentHolder& assign(DOC d);
      /// Standard destructor - releases the document
      virtual ~DocumentHolder();
    };

    /// User abstraction class to manipulate XML elements within a document
    /**
     *  User class encapsulating a DOM element
     *  using the Handle_t helper.
     *  This is the main class we interact with when
     *  analysing the xml documents for constructing
     *  sub-detectors etc.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_XML
     */
    class Element {
    public:
      /// Simplification type declarations
      typedef Handle_t::Elt_t Elt_t;

      /// The underlying object holding the XmlElement pointer
      Handle_t m_element;

      /// Constructor from XmlElement handle
      Element(const Handle_t& e) : m_element(e)          {      }
      /// Constructor from XmlElement handle
      Element(const Element& e) : m_element(e.m_element) {      }
      /// Constructor from DOM document entity
      Element(const Document& document, const XmlChar* type);
      /// Access the hosting document handle of this DOM element
      Document document() const;

      /// operator bool: check handle validity
      operator bool() const {
        return 0 != m_element.ptr();
      }
      /// operator NOT: check handle validity
      bool operator!() const {
        return 0 == m_element.ptr();
      }
      /// Assignment operator
      Element& operator=(const Element& c)  {
        m_element = c.m_element;
        return *this;
      }
      /// Assignment operator
      Element& operator=(Handle_t handle)  {
        m_element = handle;
        return *this;
      }
      /// Automatic conversion to DOM element handle
      operator Handle_t() const {
        return m_element;
      }
      /// Automatic conversion to XmlElement pointer
      operator Elt_t() const {
        return m_element;
      }
      /// Access to XmlElement pointer
      Elt_t ptr() const {
        return m_element;
      }
      /// Access the XmlElements parent
      Handle_t parent()  const   {
        return m_element.parent();
      }
      /// Access the XmlElements parent
      Elt_t parentElement()  const;
      /// Access the tag name of this DOM element
      std::string tag() const {
        return m_element.tag();
      }
      /// Access the tag name of this DOM element
      const XmlChar* tagName() const {
        return m_element.rawTag();
      }
      /// Access the tag name of this DOM element
      std::string text() const {
        return m_element.text();
      }
      /// Set text attribute to the XML node
      void text(const std::string value) const {
        return m_element.setText(value);
      }
      /// Append a new element to the existing tree
      void append(Handle_t handle) const {
        m_element.append(handle);
      }
      /// Clone the DOMelement
      Handle_t clone(const Document& new_doc) const {
        return new_doc.clone(m_element);
      }
      /// Check for the existence of a named attribute
      bool hasAttr(const XmlChar* name) const {
        return m_element.hasAttr(name);
      }
      /// Access attribute with implicit return type conversion
      template <class T> T attr(const XmlAttr* att) const {
        return m_element.attr<T>(att);
      }
      /// Access attribute with implicit return type conversion
      template <class T> T attr(const XmlChar* tag_value) const {
        return m_element.attr<T>(tag_value);
      }
      /// Access attribute with implicit return type conversion
      template <class T> T attr(const XmlChar* tag_value, T default_value) const {
        return m_element.attr<T>(tag_value, default_value);
      }
#ifndef __TIXML__
      /// Access typed attribute value by it's name
      template <class T> T attr(const char* name) const {
        return this->attr<T>(Strng_t(name));
      }
      /// Access typed attribute value by it's name
      template <class T> T attr(const char* name, T default_value) const {
        return this->attr<T>(Strng_t(name), default_value);
      }
#endif
      /// Access attribute name (throws exception if not present)
      const XmlChar* attr_name(const Attribute a) const {
        return m_element.attr_name(a);
      }
      /// Access attribute value by the attribute  (throws exception if not present)
      const XmlChar* attr_value(const Attribute a) const {
        return m_element.attr_value(a);
      }
      /// Access the number of children of this DOM element with a given tag name
      size_t numChildren(const XmlChar* tag_value, bool exc = true) const {
        return m_element.numChildren(tag_value, exc);
      }
      /// Remove own attributes and copy all attributes from handle 'e'
      void setAttrs(Handle_t e) const {
        return m_element.setAttrs(e);
      }
      /// Remove all attributes of this element
      void removeAttrs() const {
        m_element.removeAttrs();
      }
      /// Retrieve a collection of all attributes of this DOM element
      std::vector<Attribute> attributes() const {
        return m_element.attributes();
      }
      /// Access single attribute by it's name
      Attribute getAttr(const XmlChar* name) const;
      /// Set single attribute
      template <class T>
      Attribute setAttr(const XmlChar* nam, const T& val) const {
        return m_element.setAttr(nam, val);
      }
      /// Set element value
      template <class T> void setValue(const T& val) const {
        m_element.setValue(val);
      }
      /// Clone the DOM element tree
      Handle_t clone(Handle_t h) const;
      /// Add a new child to the DOM node
      Handle_t addChild(const XmlChar* tag) const;
      /// Check if a child with the required tag exists - if not create it and add it to the current node
      Handle_t setChild(const XmlChar* tag) const;
      /// Access child by tag name. Thow an exception if required in case the child is not present
      Handle_t child(const Strng_t& tag_value, bool except = true) const {
        return m_element.child(tag_value, except);
      }
      /// Remove a child node identified by its handle
      Handle_t remove(Handle_t node) const {
        return m_element.remove(node);
      }
      /// Check the existence of a child with a given tag name
      bool hasChild(const XmlChar* tag_value) const {
        return m_element.hasChild(tag_value);
      }
      /// Set the reference attribute to the node (adds attribute ref="ref-name")
      Attribute setRef(const XmlChar* tag, const XmlChar* refname) const;
      /// Set the reference attribute to the node (adds attribute ref="ref-name")
      Attribute setRef(const XmlChar* tag, const std::string& refname) const;
      /// Access the value of the reference attribute of the node (attribute ref="ref-name")
      const XmlChar* getRef(const XmlChar* tag) const;
#ifndef __TIXML__
      /// Add comment node to the element
      void addComment(const XmlChar* text) const;
#endif
      /// Add comment node to the element
      void addComment(const char* text) const;
      /// Add comment node to the element
      void addComment(const std::string& text_value) const;
    };

    /// User abstraction class to manipulate named XML elements (references) within a document
    /**
     *  User class encapsulating a DOM element which can
     *  be referenced. Like the Element, but the "name"
     *  attribute is required. The reference then has the
     *  corresponding "ref" attribute. The value of "name"
     *  and "ref" must match to establish the reference.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_XML
     */
    class RefElement : public Element {
    public:
      /// Attribute holding thre name
      Attribute m_name;
      /// Construction from existing object handle
      RefElement(const Handle_t& e);
      /// Copy constructor
      RefElement(const RefElement& e);
      /// Initializing constructor to create a new XMLElement and add it to the document.
      RefElement(const Document& d, const XmlChar* type, const XmlChar* name);
      /// Assignment operator
      RefElement& operator=(const RefElement& e);
      /// Access the object's name in unicode
      const XmlChar* name() const;
      /// Access the object's reference name in unicode (same as name)
      const XmlChar* refName() const;
      /// Change/set the object's name
      void setName(const XmlChar* new_name);
    };

#undef INLINE

    /// Forward declarations
    class DocumentHandler;

    /// Dump partial or full XML trees to stdout
    void dump_tree(Handle_t elt);
    /// Dump partial or full XML trees
    void dump_tree(Handle_t elt, std::ostream& os);
    /// Dump partial or full XML documents to stdout
    void dump_tree(Document doc);
    /// Dump partial or full XML documents
    void dump_tree(Document doc, std::ostream& os);

  }
} /* End namespace dd4hep   */
#endif    /* DD4HEP_XMLELEMENTS_H   */
