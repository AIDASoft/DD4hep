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
#ifndef DD4HEP_HANDLE_H
#define DD4HEP_HANDLE_H

// Framework include files
#include "DD4hep/Primitives.h"

#include <string>
#include <typeinfo>
#include <stdexcept>
#include <type_traits>

// Conversion factor from radians to degree: 360/(2*PI)
#ifndef RAD_2_DEGREE
#define RAD_2_DEGREE 57.295779513082320876798154814105
#endif
#ifndef DEGREE_2_RAD
#define DEGREE_2_RAD 0.0174532925199432957692369076848
#endif
#ifndef CM_2_MM
#define CM_2_MM 10.0
#endif
#ifndef MM_2_CM
#define MM_2_CM 0.1
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  // Forward declarations
  class NamedObject;

  long num_object_validations();
  void increment_object_validations();
  /// Function tp print warning about deprecated factory usage. Used by Plugin mechanism.
  void warning_deprecated_xml_factory(const char* name);


  /// Access to the magic word, which is protecting some objects against memory corruptions  \ingroup DD4HEP_CORE
  inline unsigned long long int magic_word() {
    return 0xFEEDAFFEDEADFACEULL;
  }

  /// Handle: a templated class like a shared pointer, which allows specialized access to tgeometry objects.
  /**
   * The Handle is the base class to access all objects in dd4hep.
   * Objects, which consist ONLY of data, which are NEVER passed directly.
   * They are ALWAYS passed using handles. Such handles are 'handy' ;-).
   * Assignment is to and from different handles is possible using concrete
   * type checking.
   *
   * Real benefits can result from sophisticated handle subclasses, which can
   * implement any desired user functionality with out compromising the
   * object's data content. This leads to very flexible implementations,
   * where the same data may be shared by many handle implementations
   * providing different functionality to the clients.
   *
   * In this sense, is the consequent use of handles to access data nothing
   * else then the consequent application of component oriented programming
   * using local objects from the heap.
   *
   * Note:
   * If you cannot live with this approach, it is better you get hands of this
   * software package, because you will try to consequently fight the framework,
   * which will frustrate you (and also me).
   *
   * \author  M.Frank
   * \version 1.0
   * \ingroup DD4HEP_CORE
   */
  template <typename T> class Handle {
  public:
    /** Type definitions and class specific abbreviations and forward declarations */
    /// Extern accessible definition of the contained element type
    typedef T Object;

    /// Single and only data member: Reference to the actual element.
    T* m_element = 0;

    /// Default constructor
    Handle() = default;
    /// Copy constructor
    Handle(const Handle<T>& element) = default;
    /// Initializing constructor from pointer
    Handle(T* element) : m_element(element)   {            }
    /// Initializing constructor from unrelated pointer with type checking
    template <typename Q> Handle(Q* element)
      : m_element(element ? detail::safe_cast<T>::cast(element) : 0)
    {             }
    /// Initializing constructor from unrelated handle with type checking
    template <typename Q> Handle(const Handle<Q>& element)
      : m_element(element.m_element ? detail::safe_cast<T>::cast(element.m_element) : 0)
    {             }
    /// Assignment operator
    Handle<T>& operator=(const Handle<T>& element) = default;
    /// Boolean operator == used for RB tree insertions
    bool operator==(const Handle<T>& element)  const {
      return m_element == element.m_element;
    }
    /// Boolean operator < used for RB tree insertions
    bool operator<(const Handle<T>& element)  const  {
      return m_element < element.m_element;
    }
    /// Boolean operator > used for RB tree insertions
    bool operator>(const Handle<T>& element)  const  {
      return m_element > element.m_element;
    }
    /// Check the validity of the object held by the handle
    bool isValid() const   {
      return 0 != m_element;
    }
    /// Check the validity of the object held by the handle
    bool operator!() const   {
      return 0 == m_element;
    }
    /// Release the object held by the handle
    Handle<T>& clear() {
      m_element = 0;
      return *this;
    }
    /// Access the held object using the -> operator
    T* operator->() const {
      return m_element;
    }
    /// Automatic type conversion to an object references
    operator T&() const {
      return *m_element;
    }
    /// Access the held object using the * operator
    T& operator*() const {
      return *m_element;
    }
    /// Access to the held object
    T* ptr() const {
      return m_element;
    }
    /// Access to an unrelated object type
    template <typename Q> Q* _ptr() const {
      return (Q*) m_element;
    }
    /// Access to an unrelated object type
    template <typename Q> Q* data() const {
      return (Q*) m_element;
    }
    /// Access to an unrelated object type
    template <typename Q> Q& object() const {
      return *(Q*) m_element;
    }
    /// Checked object access. Throws invalid handle runtime exception if invalid handle.
    /** Very compact way to check the validity of a handle with exception thrown.  
     */
    T* access() const;
    /// Access the object name (or "" if not supported by the object)
    const char* name() const;
    /// Assign a new named object. Note: object references must be managed by the user
    void assign(Object* n, const std::string& nam, const std::string& title);
    /// Helper routine called when unrelated types are assigned.
    static void bad_assignment(const std::type_info& from, const std::type_info& to);
  };
  /// Default Ref_t definition describing named objects  \ingroup DD4HEP_CORE
  typedef Handle<NamedObject> Ref_t;
  namespace detail  {
    /// Helper to delete objects from heap and reset the handle  \ingroup DD4HEP_CORE
    template <typename T> inline void destroyHandle(T& handle) {
      deletePtr(handle.m_element);
    }
    /// Functor to destroy handles and delete the cached object  \ingroup DD4HEP_CORE
    template <typename T> class DestroyHandle {
    public:
      void operator()(T ptr) const {  destroyHandle(ptr);    }
    };
    /// map Functor to destroy handles and delete the cached object  \ingroup DD4HEP_CORE
    template <typename M> class DestroyHandles {
    public:
      /// Container reference
      M& object;
      /// Initializing constructor
      DestroyHandles(M& obj) : object(obj) {                 }
      /// Action operator
      void operator()(const std::pair<typename M::key_type, typename M::mapped_type>& arg) const
      {   DestroyHandle<typename M::mapped_type>()(arg.second);    }
    };
    /// Functional created of map destruction functors
    template <typename M> void destroyHandles(M& arg)  {
      for_each(arg.begin(), arg.end(), DestroyHandles<M>(arg));
      arg.clear();
    }

    /// Helper to delete objects from heap and reset the handle  \ingroup DD4HEP_CORE
    template <typename T> inline void releaseHandle(T& handle) {
      releasePtr(handle.m_element);
    }
    /// Functor to destroy handles and delete the cached object  \ingroup DD4HEP_CORE
    template <typename T> class ReleaseHandle {
    public:
      void operator()(T handle) const {  releaseHandle(handle);    }
    };
    /// map Functor to release handles  \ingroup DD4HEP_CORE
    template <typename M> class ReleaseHandles {
    public:
      /// Container reference
      M& object;
      /// Initializing constructor
      ReleaseHandles(M& obj) : object(obj) {                 }
      /// Action operator
      void operator()(const std::pair<typename M::key_type, typename M::mapped_type>& arg) const
      {   ReleaseHandle<typename M::mapped_type>()(arg.second);    }
    };
    /// Functional created of map destruction functors
    template <typename M> void releaseHandles(M& arg)  {
      for_each(arg.begin(), arg.end(), ReleaseHandles<M>(arg));
      arg.clear();
    }
  }
  
  /// String conversions: boolean value to string  \ingroup DD4HEP_CORE
  std::string _toString(bool value);
  /// String conversions: integer value to string  \ingroup DD4HEP_CORE
  std::string _toString(short value, const char* fmt = "%d");
  /// String conversions: integer value to string  \ingroup DD4HEP_CORE
  std::string _toString(int value, const char* fmt = "%d");
  /// String conversions: float value to string  \ingroup DD4HEP_CORE
  std::string _toString(float value, const char* fmt = "%.17e");
  /// String conversions: double value to string  \ingroup DD4HEP_CORE
  std::string _toString(double value, const char* fmt = "%.17e");
  /// Pointer to text conversion
  std::string _ptrToString(const void* p, const char* fmt = "%p");
  /// Format any pointer (64 bits) to string  \ingroup DD4HEP_XML
  template <typename T> std::string _toString(const T* p, const char* fmt = "%p")
  {      return _ptrToString((void*)p, fmt);       }

  /// String conversions: string to boolean value  \ingroup DD4HEP_CORE
  bool   _toBool(const std::string& value);
  /// String conversions: string to integer value  \ingroup DD4HEP_CORE
  short  _toShort(const std::string& value);
  /// String conversions: string to integer value  \ingroup DD4HEP_CORE
  int    _toInt(const std::string& value);
  /// String conversions: string to long integer value  \ingroup DD4HEP_CORE
  long   _toLong(const std::string& value);
  /// String conversions: string to float value  \ingroup DD4HEP_CORE
  float  _toFloat(const std::string& value);
  /// String conversions: string to double value  \ingroup DD4HEP_CORE
  double _toDouble(const std::string& value);

  /// Void helper function to support formalisms  \ingroup DD4HEP_CORE
  inline bool _toBool(bool value) {
    return value;
  }
  /// Void helper function to support formalisms  \ingroup DD4HEP_CORE
  inline short _toShort(short value) {
    return value;
  }
  /// Void helper function to support formalisms  \ingroup DD4HEP_CORE
  inline int _toInt(int value) {
    return value;
  }
  /// Void helper function to support formalisms  \ingroup DD4HEP_CORE
  inline long _toLong(long value) {
    return value;
  }
  /// Void helper function to support formalisms  \ingroup DD4HEP_CORE
  inline unsigned short _toUShort(unsigned short value) {
    return value;
  }
  /// Void helper function to support formalisms  \ingroup DD4HEP_CORE
  inline unsigned int _toUInt(unsigned int value) {
    return value;
  }
  /// Void helper function to support formalisms  \ingroup DD4HEP_CORE
  inline unsigned long _toULong(unsigned long value) {
    return value;
  }
  /// Void helper function to support formalisms  \ingroup DD4HEP_CORE
  inline float _toFloat(float value) {
    return value;
  }
  /// Void helper function to support formalisms  \ingroup DD4HEP_CORE
  inline double _toDouble(double value) {
    return value;
  }

  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <class T> T _multiply(const std::string& left, T right);
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <class T> T _multiply(T left, const std::string& right);
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <class T> T _multiply(const std::string& left, const std::string& right);

  /** Block for concrete overloads of type:  char */
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> char _multiply<char>(const std::string& left, const std::string& right);
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> inline char _multiply<char>(char left, const std::string& right) {
    return left * _toInt(right);
  }
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> inline char _multiply<char>(const std::string& left, char right) {
    return _toInt(left) * right;
  }

  /** Block for concrete overloads of type:  unsigned char */
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> unsigned char _multiply<unsigned char>(const std::string& left, const std::string& right);
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> inline unsigned char _multiply<unsigned char>(unsigned char left, const std::string& right) {
    return left * _toInt(right);
  }
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> inline unsigned char _multiply<unsigned char>(const std::string& left, unsigned char right) {
    return _toInt(left) * right;
  }

  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> short _multiply<short>(const std::string& left, const std::string& right);
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> inline short _multiply<short>(short left, const std::string& right) {
    return left * _toInt(right);
  }
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> inline short _multiply<short>(const std::string& left, short right) {
    return _toInt(left) * right;
  }

  /** Block for concrete overloads of type:  unsigned short */
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> unsigned short _multiply<unsigned short>(const std::string& left, const std::string& right);
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> inline unsigned short _multiply<unsigned short>(unsigned short left, const std::string& right) {
    return left * _toInt(right);
  }
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> inline unsigned short _multiply<unsigned short>(const std::string& left, unsigned short right) {
    return _toInt(left) * right;
  }

  /** Block for concrete overloads of type:  int */
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> int _multiply<int>(const std::string& left, const std::string& right);
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> inline int _multiply<int>(int left, const std::string& right) {
    return left * _toInt(right);
  }
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> inline int _multiply<int>(const std::string& left, int right) {
    return _toInt(left) * right;
  }

  /** Block for concrete overloads of type:  unsigned int */
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> unsigned int _multiply<unsigned int>(const std::string& left, const std::string& right);
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> inline unsigned int _multiply<unsigned int>(unsigned int left, const std::string& right) {
    return left * _toInt(right);
  }
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> inline unsigned int _multiply<unsigned int>(const std::string& left, unsigned int right) {
    return _toInt(left) * right;
  }

  /** Block for concrete overloads of type:  long */
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> long _multiply<long>(const std::string& left, const std::string& right);
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> inline long _multiply<long>(long left, const std::string& right) {
    return left * _toLong(right);
  }
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> inline long _multiply<long>(const std::string& left, long right) {
    return _toLong(left) * right;
  }

  /** Block for concrete overloads of type:  unsigned long */
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> unsigned long _multiply<unsigned long>(const std::string& left, const std::string& right);
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> inline unsigned long _multiply<unsigned long>(unsigned long left, const std::string& right) {
    return left * _toLong(right);
  }
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> inline unsigned long _multiply<unsigned long>(const std::string& left, unsigned long right) {
    return _toLong(left) * right;
  }

  /** Block for concrete overloads of type: float  */
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> float _multiply<float>(const std::string& left, const std::string& right);
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> inline float _multiply<float>(float left, const std::string& right) {
    return left * _toFloat(right);
  }
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> inline float _multiply<float>(const std::string& left, float right) {
    return _toFloat(left) * right;
  }

  /** Block for concrete overloads of type:  double */
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> double _multiply<double>(const std::string& left, const std::string& right);
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> inline double _multiply<double>(const std::string& left, double right) {
    return _toDouble(left) * right;
  }
  /// Generic multiplication using the evaluator: result = left * right  \ingroup DD4HEP_CORE
  template <> inline double _multiply<double>(double left, const std::string& right) {
    return left * _toDouble(right);
  }

  /// Enter name value pair to the dictionary. \"value\" must be a numerical expression, which is evaluated  \ingroup DD4HEP_CORE
  void _toDictionary(const std::string& name, const std::string& value);
  /// Enter name value pair to the dictionary.  \ingroup DD4HEP_CORE
  void _toDictionary(const std::string& name, const std::string& value, const std::string& typ);

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace detail {
    using dd4hep::Handle;
    using dd4hep::Ref_t;
  } /* End namespace detail  */
  
  // Forward declarations
  class Detector;
}         /* End namespace dd4hep    */
#endif    /* DD4HEP_HANDLE_H         */

