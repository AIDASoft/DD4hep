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
#ifndef DDCORE_DD4HEP_JSON_ELEMENTS_H
#define DDCORE_DD4HEP_JSON_ELEMENTS_H

// C/C++ include files
#include <cmath>
#include <string>
#include <vector>
#include <stdexcept>

// Framework include files
#include "JSON/config.h"

#ifndef RAD_2_DEGREE
#define RAD_2_DEGREE 57.295779513082320876798154814105
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the AIDA detector description toolkit supporting JSON utilities
  namespace JSON {

    typedef const JsonAttr* Attribute;

    typedef const std::string& CSTR;

    /// Dump DOM tree of a document
    void dumpTree(JsonDocument* doc);

    /// Convert json attribute to STL string  \ingroup DD4HEP_JSON
    std::string _toString(const Attribute attr);
    /// Convert json string to STL string  \ingroup DD4HEP_JSON
    std::string _toString(const JsonChar *toTranscode);
    /// Do-nothing version. Present for completeness and argument interchangeability  \ingroup DD4HEP_JSON
    std::string _toString(const char* s);
    /// Do-nothing version. Present for completeness and argument interchangeability  \ingroup DD4HEP_JSON
    std::string _toString(const std::string& s);
    /// Format unsigned long integer to string with arbitrary format  \ingroup DD4HEP_JSON
    std::string _toString(unsigned long i, const char* fmt = "%lu");
    /// Format unsigned integer (32 bits) to string with arbitrary format  \ingroup DD4HEP_JSON
    std::string _toString(unsigned int i, const char* fmt = "%u");
    /// Format signed integer (32 bits) to string with arbitrary format  \ingroup DD4HEP_JSON
    std::string _toString(int i, const char* fmt = "%d");
    /// Format signed long integer to string with arbitrary format  \ingroup DD4HEP_JSON
    std::string _toString(long i, const char* fmt = "%ld");
    /// Format single procision float number (32 bits) to string with arbitrary format  \ingroup DD4HEP_JSON
    std::string _toString(float d, const char* fmt = "%.17e");
    /// Format double procision float number (64 bits) to string with arbitrary format  \ingroup DD4HEP_JSON
    std::string _toString(double d, const char* fmt = "%.17e");
    /// Format void pointer (64 bits) to string with arbitrary format  \ingroup DD4HEP_JSON
    std::string _ptrToString(const void* p, const char* fmt = "%p");
    /// Format void pointer (64 bits) to string with arbitrary format  \ingroup DD4HEP_JSON
    template <typename T> std::string _toString(const T* p, const char* fmt = "%p")
    {      return _ptrToString((void*)p,fmt);       }

    /// Helper function to populate the evaluator dictionary  \ingroup DD4HEP_JSON
    void _toDictionary(const JsonChar* name, const JsonChar* value);
    /// Helper function to populate the evaluator dictionary  \ingroup DD4HEP_JSON
    void _toDictionary(const std::string& name, const JsonChar* value)
    {  _toDictionary(name, value);  }
    /// Helper function to populate the evaluator dictionary  \ingroup DD4HEP_JSON
    template <typename T> void _toDictionary(const JsonChar* name, T value);

    /// Helper function to populate the evaluator dictionary  \ingroup DD4HEP_JSON
    void _toDictionary(const JsonChar* name, float  value);
    /// Helper function to populate the evaluator dictionary  \ingroup DD4HEP_JSON
    void _toDictionary(const JsonChar* name, double value);
    /// Helper function to lookup environment from the expression evaluator
    std::string getEnviron(const std::string& env);

    /// Conversion function from raw unicode string to bool  \ingroup DD4HEP_JSON
    bool _toBool(const JsonChar* value);
    /// Conversion function from raw unicode string to int  \ingroup DD4HEP_JSON
    int _toInt(const JsonChar* value);
    /// Conversion function from raw unicode string to long  \ingroup DD4HEP_JSON
    long _toLong(const JsonChar* value);
    /// Conversion function from raw unicode string to float  \ingroup DD4HEP_JSON
    float _toFloat(const JsonChar* value);
    /// Conversion function from raw unicode string to double  \ingroup DD4HEP_JSON
    double _toDouble(const JsonChar* value);

    /// Class describing a list of JSON nodes
    /**
     *  Definition of a "list" of json elements hanging of the parent.
     *  The list allows iterations, which can be restarted.
     *  Please not: Copies are not entirely free, since the
     *  string tag need to be replicated using strdup/free
     *  (or when using xerces the corresponding unicode routines).
     *
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup DD4HEP_JSON
     */
    class NodeList {
    public:
      typedef std::pair<JsonElement::second_type::assoc_iterator,JsonElement::second_type::assoc_iterator> iter_t;
      std::string    m_tag;
      JsonElement*   m_node;
      mutable iter_t m_ptr;

      /// Copy constructor
      NodeList(const NodeList& l);
      /// Initializing constructor
      NodeList(JsonElement* frst, const std::string& tag);
      /// Default destructor
      ~NodeList();
      /// Reset the nodelist - e.g. restart iteration from beginning
      JsonElement* reset();
      /// Advance to next element
      JsonElement* next() const;
      /// Go back to previous element
      JsonElement* previous() const;
      /// Assignment operator
      NodeList& operator=(const NodeList& l);
    };

    /// Class to easily access the properties of single JsonElements.
    /**
     *  Note: The assignmant operator as well as the copy constructor
     *  do not have to be implemented, they are aut-generated by the compiler!
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_JSON
     */
    class Handle_t {
    public:
      // Abbreviation for internal use
      typedef JsonElement* Elt_t;

      /// The pointer to the JsonElement
      mutable Elt_t m_node;

      /// Initializing constructor
      Handle_t(Elt_t e = 0)
        : m_node(e) {
      }
      /// Direct access to the JsonElement using the operator->
      Elt_t operator->() const {
        return m_node;
      }
      /// Direct access to the JsonElement by dereferencing
      operator Elt_t() const {
        return m_node;
      }
      /// Direct access to the JsonElement by function
      Elt_t ptr() const {
        return m_node;
      }
      /// Clone the DOMelement - with the option of a deep copy
      Handle_t clone(JsonDocument* new_doc) const;

      /// Unicode text access to the element's tag. Tis must be wrong ....
      const JsonChar* rawTag() const;
      /// Unicode text access to the element's text
      const JsonChar* rawText() const;
      /// Unicode text access to the element's value
      const JsonChar* rawValue() const;
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

      /*** DOM Attribute handling
       */
      /// Access attribute name (throws exception if not present)
      const JsonChar* attr_name(const Attribute attr) const;
      /// Access attribute value by the attribute  (throws exception if not present)
      const JsonChar* attr_value(const Attribute attr) const;
      /// Access attribute value by the attribute's unicode name (throws exception if not present)
      const JsonChar* attr_value(const JsonChar* attr) const;
      /// Access attribute value by the attribute's unicode name (no exception thrown if not present)
      const JsonChar* attr_value_nothrow(const JsonChar* attr) const;

      /// Access attribute pointer by the attribute's unicode name (throws exception if not present)
      Attribute attr_ptr(const JsonChar* attr) const;
      /// Access attribute pointer by the attribute's unicode name (no exception thrown if not present)
      Attribute attr_nothrow(const JsonChar* tag) const;
      /// Check for the existence of a named attribute
      bool hasAttr(const JsonChar* t) const;
      /// Retrieve a collection of all attributes of this DOM element
      std::vector<Attribute> attributes() const;
      /// Access typed attribute value by the JsonAttr
      template <class T> T attr(const Attribute a) const {
        return this->attr<T>(this->attr_name(a));
      }
      /// Access typed attribute value by it's unicode name
      template <class T> T attr(const JsonChar* name) const;

      /*** DOM Element child handling
       */
      /// Check the existence of a child with a given tag name
      bool hasChild(const JsonChar* tag) const;
      /// Access a single child by it's tag name (unicode)
      Handle_t child(const JsonChar* tag, bool throw_exception = true) const;
      /// Access a group of children identified by the same tag name
      NodeList children(const JsonChar* tag) const;
      /// Access the number of children of this DOM element with a given tag name
      size_t numChildren(const JsonChar* tag, bool throw_exception) const;
      /// Access the element's parent element
      //Handle_t parent() const;
    };

#define INLINE inline
    typedef const JsonChar* cpJsonChar;

    template <> INLINE Attribute Handle_t::attr<Attribute>(const JsonChar* tag_value) const {
      return attr_ptr(tag_value);
    }

    template <> INLINE cpJsonChar Handle_t::attr<cpJsonChar>(const JsonChar* tag_value) const {
      return attr_value(tag_value);
    }

    template <> INLINE bool Handle_t::attr<bool>(const JsonChar* tag_value) const {
      return _toBool(attr_value(tag_value));
    }

    template <> INLINE int Handle_t::attr<int>(const JsonChar* tag_value) const {
      return _toInt(attr_value(tag_value));
    }
    
    template <> INLINE long Handle_t::attr<long>(const JsonChar* tag_value) const {
      return _toLong(attr_value(tag_value));
    }

    template <> INLINE float Handle_t::attr<float>(const JsonChar* tag_value) const {
      return _toFloat(attr_value(tag_value));
    }

    template <> INLINE double Handle_t::attr<double>(const JsonChar* tag_value) const {
      return _toDouble(attr_value(tag_value));
    }

    template <> INLINE std::string Handle_t::attr<std::string>(const JsonChar* tag_value) const {
      return _toString(attr_value(tag_value));
    }

    /// Class to support the access to collections of JsonNodes (or JsonElements)
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_JSON
     */
    class Collection_t : public Handle_t {
    public:
      /// Reference to the list of child nodes
      NodeList m_children;
      /// Constructor over JsonElements with a given tag name
      Collection_t(Handle_t node, const char* tag);
      /// Constructor over JsonElements in a node list
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
      template <class T> void for_each(const JsonChar* tag_name, T oper) const {
        try {
          for (const Collection_t& c = *this; c; ++c)
            Collection_t(c.m_node, tag_name).for_each(oper);
        }
        catch (const std::exception& e) {
          throw_loop_exception(e);
        }
      }
    };

    /// Class supporting the basic functionality of an JSON document
    /**
     *  User class encapsulating a DOM document.
     *  Nothing special - normal handle around pointer.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_JSON
     */
    class Document {
    public:
      typedef JsonDocument* DOC;
      DOC m_doc;

      /// Constructor
      Document(DOC d) : m_doc(d) {
      }
      /// Auto-conversion to DOM document
      operator DOC() const {
        return m_doc;
      }
      /// Accessot to DOM document behaviour using arrow operator
      DOC operator->() const {
        return m_doc;
      }
      /// Accessot to DOM document behaviour
      DOC ptr() const {
        return m_doc;
      }
    };

    /// Class supporting the basic functionality of an JSON document including ownership
    /**
     *  User class encapsulating a DOM document.
     *  Nothing special - normal handle around pointer.
     *  Contrary to the Document class, on destruction the
     *  JSON document shall be destroyed and the corresponding
     *  resources released.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_JSON
     */
    class DocumentHolder : public Document {
    public:
      /// Default Constructor
      DocumentHolder() : Document(0) {
      }
      /// Constructor
      DocumentHolder(DOC d) : Document(d) {
      }
      /// Assign new document. Old document is dropped.
      DocumentHolder& assign(DOC d);
      /// Standard destructor - releases the document
      virtual ~DocumentHolder();
    };

    /// User abstraction class to manipulate JSON elements within a document
    /**
     *  User class encapsulating a DOM element
     *  using the Handle helper.
     *  This is the main class we interact with when
     *  analysing the json documents for constructing
     *  sub-detectors etc.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_JSON
     */
    class Element {
    public:
      /// Simplification type declarations
      typedef Handle_t::Elt_t Elt_t;

      /// The underlying object holding the JsonElement pointer
      Handle_t m_element;

      /// Constructor from JsonElement handle
      Element(const Handle_t& e) : m_element(e)          {      }
      /// Constructor from JsonElement handle
      Element(const Element& e) : m_element(e.m_element) {      }
      /// Constructor from DOM document entity
      //Element(const Document& document, const JsonChar* type);
      /// Access the hosting document handle of this DOM element
      //Document document() const;

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
      /// Automatic conversion to JsonElement pointer
      operator Elt_t() const {
        return m_element;
      }
      /// Access to JsonElement pointer
      Elt_t ptr() const {
        return m_element;
      }
#if 0
      /// Access the JsonElements parent
      Handle_t parent()  const   {
        return m_element.parent();
      }
      /// Access the JsonElements parent
      Elt_t parentElement()  const;
#endif
      /// Access the tag name of this element
      std::string tag() const {
        return m_element.tag();
      }
      /// Access the tag name of this element
      const JsonChar* tagName() const {
        return m_element.rawTag();
      }
      /// Access the tag name of this element
      std::string text() const {
        return m_element.text();
      }
      /// Check for the existence of a named attribute
      bool hasAttr(const JsonChar* name) const {
        return m_element.hasAttr(name);
      }
      /// Access attribute with implicit return type conversion
      template <class T> T attr(const JsonChar* tag_value) const {
        return m_element.attr<T>(tag_value);
      }
      /// Access attribute name (throws exception if not present)
      const JsonChar* attr_name(const Attribute a) const {
        return m_element.attr_name(a);
      }
      /// Access attribute value by the attribute  (throws exception if not present)
      const JsonChar* attr_value(const Attribute a) const {
        return m_element.attr_value(a);
      }
      /// Access the number of children of this element with a given tag name
      size_t numChildren(const JsonChar* tag_value, bool exc = true) const {
        return m_element.numChildren(tag_value, exc);
      }
      /// Retrieve a collection of all attributes of this element
      std::vector<Attribute> attributes() const {
        return m_element.attributes();
      }
      /// Access single attribute by it's name
      Attribute getAttr(const JsonChar* name) const;
     /// Access child by tag name. Thow an exception if required in case the child is not present
      Handle_t child(const JsonChar* tag_value, bool except = true) const {
        return m_element.child(tag_value, except);
      }
      /// Check the existence of a child with a given tag name
      bool hasChild(const JsonChar* tag_value) const {
        return m_element.hasChild(tag_value);
      }
    };


#undef INLINE

    /// Forward declarations
    class DocumentHandler;

    /// Dump partial or full JSON trees to stdout
    void dump_tree(Handle_t elt);
    /// Dump partial or full JSON trees
    void dump_tree(Handle_t elt, std::ostream& os);
    /// Dump partial or full JSON documents to stdout
    void dump_tree(Document doc);
    /// Dump partial or full JSON documents
    void dump_tree(Document doc, std::ostream& os);

  }       /* End namespace XML               */
}         /* End namespace DD4hep            */
#endif    /* DDCORE_DD4HEP_JSON_ELEMENTS_H   */
