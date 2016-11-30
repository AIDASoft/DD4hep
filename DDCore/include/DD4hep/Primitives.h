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
#ifndef DD4HEP_DD4HEP_PRIMITIVES_H
#define DD4HEP_DD4HEP_PRIMITIVES_H

// C/C++ include files
#include <map>
#include <vector>
#include <string>
#include <limits>

#include <typeinfo>
#include <algorithm>
#include <stdexcept>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace describing generic detector segmentations
  namespace DDSegmentation  {
    class BitField64;
    class BitFieldValue;
    /// Useful typedefs to differentiate cell IDs and volume IDs
    typedef long long int CellID;
    typedef long long int VolumeID;
  }

  /// We need it so often: one-at-time 32 bit hash function
  inline unsigned int hash32(const char* key) {
    unsigned int hash = 0;
    const char* k = key;
    for (; *k; k++) {
      hash += *k;
      hash += (hash << 10);
      hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11); hash += (hash << 15);
    return hash;
  }
  inline unsigned int hash32(const std::string& key) {
    return hash32(key.c_str());
  }

  /// Convert date into epoch time (seconds since 1970)
  long int makeTime(int year, int month, int day,
                    int hour=0, int minutes=0, int seconds=0);
  
  /// Convert date into epoch time (seconds since 1970)
  long int makeTime(const std::string& date, const char* fmt="%d-%m-%Y %H:%M:%S");
  
  /// Specialized exception to be thrown if invalid handles are accessed
  class invalid_handle_exception : public std::runtime_error  {
  public:
    /// Initializing constructor
    invalid_handle_exception(const char* msg) : std::runtime_error(msg) {}
    /// Initializing constructor
    invalid_handle_exception(const std::string& msg) : std::runtime_error(msg) {}
    /// Generic copy constructor
    invalid_handle_exception(const std::exception& e) : std::runtime_error(e.what()) {}
    /// Generic copy constructor
    invalid_handle_exception(const invalid_handle_exception& e) : std::runtime_error(e.what()) {}
    /// Default destructor of specialized exception
    virtual ~invalid_handle_exception() = default;
  };

  /// ABI information about type names
  std::string typeName(const std::type_info& type);
  /// Check type infos for equivalence (dynamic casts) using ABI information
  void typeinfoCheck(const std::type_info& typ1, const std::type_info& typ2, const std::string& text = "");
  /// Throw exception when handles are check for validity
  void invalidHandleError(const std::type_info& type);
  /// Throw exception when handles are badly assigned
  void invalidHandleAssignmentError(const std::type_info& from, const std::type_info& to);

  /// Throw exception when handles are check for validity
  template <typename T> void invalidHandleError()  {
    invalidHandleError(typeid(T));
  }

  /// Throw exception when handles are check for validity
  void notImplemented(const std::string& msg);


  /// A bit of support for printing primitives
  /**
   *
   * \author  M.Frank
   * \version 1.0
   * \ingroup DD4HEP
   */
  template<typename T> struct Primitive {
  public:
    /// Type decribed by th class
    typedef T value_t;
    /// Definition of the vector type
    typedef std::vector<value_t>             vector_t;

    /// Definition of the short integer mapped type
    typedef std::pair<short,value_t>          short_pair_t;
    /// Definition of the unsigned short integer mapped type
    typedef std::pair<unsigned short,value_t> ushort_pair_t;
    /// Definition of the integer mapped type
    typedef std::pair<int,value_t>            int_pair_t;
    /// Definition of the unsigned integer mapped type
    typedef std::pair<unsigned int,value_t>   uint_pair_t;
    /// Definition of the long integer mapped type
    typedef std::pair<long,value_t>           long_pair_t;
    /// Definition of the unsigned long integer mapped type
    typedef std::pair<unsigned long,value_t>  ulong_pair_t;
    /// Definition of the size_t mapped type
    typedef std::pair<size_t,value_t>         size_pair_t;
    /// Definition of the string mapped type
    typedef std::pair<std::string,value_t>    string_pair_t;

    /// Definition of the short integer mapped type
    typedef std::map<short,value_t>          short_map_t;
    /// Definition of the unsigned short integer mapped type
    typedef std::map<unsigned short,value_t> ushort_map_t;
    /// Definition of the integer mapped type
    typedef std::map<int,value_t>            int_map_t;
    /// Definition of the unsigned integer mapped type
    typedef std::map<unsigned int,value_t>   uint_map_t;
    /// Definition of the long integer mapped type
    typedef std::map<long,value_t>           long_map_t;
    /// Definition of the unsigned long integer mapped type
    typedef std::map<unsigned long,value_t>  ulong_map_t;
    /// Definition of the size_t mapped type
    typedef std::map<size_t,value_t>         size_map_t;
    /// Definition of the string mapped type
    typedef std::map<std::string,value_t>    string_map_t;
    /// Definition of the limits
    typedef std::numeric_limits<value_t>     limits;

    /// Access to default printf format
    static const char* default_format(); 
    /// Access to the specific printf format. May be overloaded by users
    static const char* format()          { return default_format();     }
    /// Access to the RTTI data type
    static const std::type_info& type()  { return typeid(value_t);      }
    /// Access to the RTTI data type
    static std::string type_name()       { return typeName(type());     }
    /// Auto conversion to string using the default format
    static std::string toString(T value);
  };


  /// Class to perform dynamic casts using unknown pointers.
  /** @class ComponentCast Primitives.h DD4hep/Primitives.h
   *
   *  It is mandatory that the pointers referred do actually
   *  support the asked functionalty.
   *  Miracles also I cannot do.....
   *
   *   @author  M.Frank
   *   @date    13.08.2013
   */
  class ComponentCast {
  public:
    typedef void  (*destroy_t)(void*);
    typedef void* (*cast_t)(const void*);
#ifdef __CINT__
    const std::type_info* type;
#else
    const std::type_info& type;
#endif
    const void* abi_class;
    destroy_t   destroy;
    cast_t      cast;

  private:
    /// Initializing Constructor
    ComponentCast(const std::type_info& t, destroy_t d, cast_t c);
    /// Defautl destructor
    virtual ~ComponentCast();

  public:
    template <typename TYPE> static void _destroy(void* p)  {
      TYPE* q = (TYPE*)p;
      if (q)    delete q;
    }
    template <typename TYPE> static void* _cast(const void* p)  {
      TYPE* q = (TYPE*)p;
      q = dynamic_cast<TYPE*>(q);
      return (void*)q;
    }
    template <typename TYPE> static ComponentCast& instance() {
      static ComponentCast c(typeid(TYPE),_destroy<TYPE>,_cast<TYPE>);
      return c;
    }

    /// Apply cast using typeinfo instead of dynamic_cast
    void* apply_dynCast(const ComponentCast& to, const void* ptr) const;
    /// Apply cast using typeinfo instead of dynamic_cast
    void* apply_upCast(const ComponentCast& to, const void* ptr) const;
    /// Apply cast using typeinfo instead of dynamic_cast
    void* apply_downCast(const ComponentCast& to, const void* ptr) const;
  };


  // Put here global basic type definitions derived from primitive types of the DD4hep namespace
#ifdef __CINT__
  typedef DDSegmentation::CellID CellID;
  typedef DDSegmentation::VolumeID VolumeID;
  typedef DDSegmentation::BitField64 BitField64;
  typedef DDSegmentation::BitFieldValue BitFieldValue;
#else
  using DDSegmentation::CellID;
  using DDSegmentation::VolumeID;
  using DDSegmentation::BitField64;
  using DDSegmentation::BitFieldValue;
#endif

  template<typename C> struct ClearOnReturn {
    C& container;
    ClearOnReturn(C& c) : container(c) {  }
    ~ClearOnReturn() { container.clear(); }
  };

  /// Helper to copy objects.
  template <typename T> inline void copyObject(void* target,const void* source)  {
    const T* s = (const T*)source;
    ::new(target) T(*s);
  }
  /// Helper to destruct objects. Note: The memory is NOT released!
  template <typename T> inline void destructObject(T* p) {
    p->~T();
  }
  /// Helper to delete objects from heap and reset the pointer. Saves many many lines of code
  template <typename T> inline void deletePtr(T*& p) {
    if (0 != p)
      delete p;
    p = 0;
  }
  /// Helper to delete objects from heap and reset the pointer. Saves many many lines of code
  template <typename T> inline void deleteObject(T* p) {
    if (0 != p)
      delete p;
  }
  /// Helper to delete objects from heap and reset the pointer
  template <typename T> inline void destroyObject(T*& p) {
    deletePtr(p);
  }
  /// Functor to delete objects from heap and reset the pointer
  template <typename T> class DestroyObject {
  public:
    void operator()(T& p) const {
      destroyObject(p);
    }
  };

  /// Operator to select second element of a pair
  template <typename T> class Select2nd  {
  public:
    typedef T arg_t;
    typedef typename T::second_type result_t;
    /// Operator function
    const result_t& operator()(const arg_t &p) const { return p.second; }
  };
  /// Generator to create Operator to select value elements of a map
  template <typename T> Select2nd<typename T::value_type> select2nd(const T&)
  { return Select2nd<typename T::value_type>(); }

  /// Operator to select the first element of a pair
  template <typename T> class Select1st  {
  public:
    typedef T arg_t;
    typedef typename T::first_type result_t;
    /// Operator function
    const result_t& operator()(const arg_t &p) const { return p.first; }
  };
  /// Generator to create Operator to select key values of a map
  template <typename T> Select1st<typename T::value_type> select1st(const T&)
  { return Select1st<typename T::value_type>(); }


  /// map Functor to delete objects from heap
  template <typename M> class DestroyObjects {
  public:
    M& object;
    DestroyObjects(M& m) : object(m) {                    }
    void operator()(std::pair<typename M::key_type, typename M::mapped_type> p) const
    {  DestroyObject<typename M::mapped_type>()(p.second);    }
    void operator()() const {
      if ( !object.empty() ) for_each(object.begin(),object.end(),(*this));
      object.clear();
    }
  };
  template <typename M> void destroyObjects(M& m)
  {  DestroyObjects<M> del(m); del();                      }
  template <typename M> DestroyObjects<M> destroy2nd(M& m)
  {  DestroyObjects<M> del(m); del();                      }

  /// map Functor to delete objects from heap
  template <typename M> class DestroyFirst {
  public:
    M& object;
    DestroyFirst(M& m) : object(m) {   }
    void operator()(std::pair<typename M::key_type, typename M::mapped_type> p) const
    {  DestroyObject<typename M::key_type>()(p.first);     }
    void operator()() const {
      if ( !object.empty() ) for_each(object.begin(),object.end(),(*this));
      object.clear();
    }
  };
  template <typename M> void destroyFirst(M& m)
  {   DestroyFirst<M> del(m); del();           }
  template <typename M> void destroy1st(M& m)
  {   DestroyFirst<M> del(m); del();           }

  /// Helper to delete objects from heap and reset the pointer. Saves many many lines of code
  template <typename T> inline void releasePtr(T& p) {
    if (0 != p)
      p->release();
    p = 0;
  }

  /// Functor to release objects from heap and reset the pointer
  template <typename T> class ReleaseObject {
  public:
    void operator()(T& p) const {
      releasePtr(p);
    }
  };
  /// Map Functor to release objects from heap
  template <typename M> class ReleaseObjects {
  public:
    M& object;
    ReleaseObjects(M& m) : object(m) {    }
    void operator()(std::pair<typename M::key_type, typename M::mapped_type> p) const
    {  ReleaseObject<typename M::mapped_type>()(p.second);    }
    void operator()() const {
      if ( !object.empty() ) for_each(object.begin(),object.end(),(*this));
      object.clear();
    }
  };
  template <typename M> ReleaseObject<typename M::value_type> releaseObject(M&) {
    return ReleaseObject<typename M::value_type>();
  }
  template <typename M> void releaseObjects(M& m) {
    ReleaseObjects<M> rel(m); rel();
  }
  template <typename M> void release2nd(M& m) {
    ReleaseObjects<M> rel(m); rel();
  }

  /// Functor to delete objects from heap and reset the pointer
  template <typename T> class ReferenceObject {
  public:
    typedef T arg_t;
    T operator()(T p) const {
      if ( p ) p->addRef();
      return p;
    }
  };
  /// Functor to delete objects from heap and reset the pointer
  template <typename M> class ReferenceObjects {
  public:
    typedef typename M::second_type result_t;
    result_t operator()(const M& p) const {
      return ReferenceObject<result_t>()(p.second);
    }
  };
  template <typename M> ReferenceObject<M> referenceObject(M&) {
    return ReferenceObject<typename M::value_type>();
  }
  template <typename M> ReferenceObjects<typename M::value_type> reference2nd(M&) {
    return ReferenceObjects<typename M::value_type>();
  }

  /// Member function call-functor with no arguments
  template <typename R, typename T> struct ApplyMemFunc {
    typedef R (T::*memfunc_t)();
    memfunc_t func;
    ApplyMemFunc(memfunc_t f) : func(f)  {}
    void operator()(T* p)  const {  if (p) { (p->*func)(); } }
  };

  /// Member function call-functor with 1 argument
  template <typename R, typename T, typename A1> struct ApplyMemFunc1 {
    typedef R (T::*memfunc_t)(A1 a1);
    memfunc_t func;
    A1& arg1;
    ApplyMemFunc1(memfunc_t f, A1& a1) : func(f), arg1(a1)  {}
    void operator()(T* p)  const {  if ( p ) { (p->*func)(arg1); } }
  };

  /// Member function call-functor with 2 arguments
  template <typename R, typename T, typename A1, typename A2> struct ApplyMemFunc2 {
    typedef R (T::*memfunc_t)(A1 a1, A2 a2);
    memfunc_t func;
    A1& arg1;
    A2& arg2;
    ApplyMemFunc2(memfunc_t f, A1& a1, A2& a2) : func(f), arg1(a1), arg2(a2)  {}
    void operator()( T* p)  const {  if ( p ) { (p->*func)(arg1, arg2); } }
  };

  /// Member function call-functor with no arguments (const version)
  template <typename R, typename T> struct ApplyMemFuncConst {
    typedef R (T::*memfunc_t)() const;
    memfunc_t func;
    ApplyMemFuncConst(memfunc_t f) : func(f)  {}
    void operator()(const T* p)  const {  if ( p ) { (p->*func)(); } }
  };

  /// Member function call-functor with 1 argument (const version)
  template <typename R, typename T, typename A1> struct ApplyMemFuncConst1 {
    typedef R (T::*memfunc_t)(A1 a1) const;
    memfunc_t func;
    A1& arg1;
    ApplyMemFuncConst1(memfunc_t f, A1& a1) : func(f), arg1(a1)  {}
    void operator()(const T* p)  const {  if ( p ) { (p->*func)(arg1); } }
  };

  /// Member function call-functor with 2 arguments (const version)
  template <typename R, typename T, typename A1, typename A2> struct ApplyMemFuncConst2 {
    typedef R (T::*memfunc_t)(A1 a1, A2 a2) const;
    memfunc_t func;
    A1& arg1;
    A2& arg2;
    ApplyMemFuncConst2(memfunc_t f, A1& a1, A2& a2) : func(f), arg1(a1), arg2(a2)  {}
    void operator()(const T* p)  const {  if ( p ) { (p->*func)(arg1, arg2); } }
  };

  template <typename C, typename R, typename T>
  void call_member_func(C& object, R (T::*pmf)())
  {   std::for_each(object.begin(),object.end(),ApplyMemFunc<R,T>(pmf));    }

  template <typename C, typename R, typename T>
  void call_member_func(C& object, R (T::*pmf)() const)
  {   std::for_each(object.begin(),object.end(),ApplyMemFuncConst<R,T>(pmf));    }

  template <typename C, typename R, typename T, typename A1>
  void call_member_func(C& object, R (T::*pmf)(A1 a1), A1 a1)
  {   std::for_each(object.begin(),object.end(),ApplyMemFunc1<R,T,A1>(pmf,a1));    }
  
  template <typename C, typename R, typename T, typename A1>
  void call_member_func(C& object, R (T::*pmf)(A1 a1) const, A1 a1)
  {   std::for_each(object.begin(),object.end(),ApplyMemFuncConst1<R,T,A1>(pmf,a1));    }
  
  
  template <typename C, typename R, typename T, typename A1, typename A2>
  void call_member_func(C& object, R (T::*pmf)(A1 a1,A2 a2), A1 a1, A2 a2)
  {   std::for_each(object.begin(),object.end(),ApplyMemFunc2<R,T,A1,A2>(pmf,a1,a2));    }
  
  template <typename C, typename R, typename T, typename A1, typename A2>
  void call_member_func(C& object, R (T::*pmf)(A1 a1,A2 a2) const, A1 a1, A2 a2)
  {   std::for_each(object.begin(),object.end(),ApplyMemFuncConst2<R,T,A1,A2>(pmf,a1,a2));    }
  

  /// Generic map Functor to act on first element (key)
  template <typename M, typename FCN> class Apply1rst {
  public:
    const FCN& func;
    Apply1rst(const FCN& f) : func(f) {    }
    void operator()(std::pair<typename M::key_type const, typename M::mapped_type>& p) const
    {   (func)(p.first);    }
    void operator()(const std::pair<typename M::key_type const, typename M::mapped_type>& p) const
    {   (func)(p.first);    }
  };

  template <typename C, typename FCN> Apply1rst<C,FCN> apply__1rst_value(C&,const FCN& func)
  {  return Apply1rst<C,FCN>(func);  }

  template <typename C, typename FCN> void apply1rst(C& object,const FCN& func)
  {  std::for_each(object.begin(),object.end(),apply__1rst_value(object,func));  }

  template <typename C, typename R, typename T>
  void apply1rst(C& object, R (T::*pmf)())  
  {  std::for_each(object.begin(),object.end(),apply__1rst_value(object,ApplyMemFunc<R,T>(pmf)));  }

  template <typename C, typename R, typename T, typename A1>
  void apply1rst(C object, R (T::*pmf)(A1 a1), A1 a1)
  {  std::for_each(object.begin(),object.end(),apply__1rst_value(object,ApplyMemFunc1<R,T,A1>(pmf,a1)));  }

  template <typename C, typename R, typename T>
  void apply1rst(C& object, R (T::*pmf)() const)  
  {  std::for_each(object.begin(),object.end(),apply__1rst_value(object,ApplyMemFuncConst<R,T>(pmf)));  }

  template <typename C, typename R, typename T, typename A1>
  void apply1rst(C object, R (T::*pmf)(A1 a1) const, A1 a1)
  {  std::for_each(object.begin(),object.end(),apply__1rst_value(object,ApplyMemFuncConst1<R,T,A1>(pmf,a1)));  }

  /// Generic map Functor to act on second element (mapped type)
  template <typename M, typename FCN> class Apply2nd {
  public:
    const FCN& func;
    Apply2nd(const FCN& f) : func(f) {    }
    void operator()(std::pair<typename M::key_type const, typename M::mapped_type>& p) const
    {   (func)(p.second);    }
    void operator()(const std::pair<typename M::key_type const, typename M::mapped_type>& p) const
    {   (func)(p.second);    }
  };

  template <typename C, typename FCN> Apply2nd<C,FCN> apply__2nd_value(C&,const FCN& func)
  {  return Apply2nd<C,FCN>(func);  }

  template <typename C, typename FCN> void apply2nd(C& object,const FCN& func)
  {  std::for_each(object.begin(),object.end(),apply__2nd_value(object,func));  }

  template <typename C, typename R, typename T>
  void apply2nd(C& object, R (T::*pmf)())  
  {  std::for_each(object.begin(),object.end(),apply__2nd_value(object,ApplyMemFunc<R,T>(pmf)));  }

  template <typename C, typename R, typename T, typename A1>
  void apply2nd(C object, R (T::*pmf)(A1 a1), A1 a1)
  {  std::for_each(object.begin(),object.end(),apply__2nd_value(object,ApplyMemFunc1<R,T,A1>(pmf,a1)));  }

  template <typename C, typename R, typename T>
  void apply2nd(C& object, R (T::*pmf)() const)  
  {  std::for_each(object.begin(),object.end(),apply__2nd_value(object,ApplyMemFuncConst<R,T>(pmf)));  }

  template <typename C, typename R, typename T, typename A1>
  void apply2nd(C object, R (T::*pmf)(A1 a1) const, A1 a1)
  {  std::for_each(object.begin(),object.end(),apply__2nd_value(object,ApplyMemFuncConst1<R,T,A1>(pmf,a1)));  }

  /// Data structure to manipulate a bitmask held by reference and represented by an integer
  /**
   * @author  M.Frank
   * @version 1.0
   */
  template <typename T> class ReferenceBitMask  {
  public:
    /// Reference to the data
    T& mask;
    /// Standard constructor
    ReferenceBitMask(T& m);
    T value() const  {
      return mask;
    }
    void set(const T& m)   {
      mask |= m;
    }
    void clear(const T& m)   {
      mask &= ~m;
    }
    void clear()   {
      mask = 0;
    }
    bool isSet(const T& m)  const {
      return (mask&m) == m;
    }
    bool anySet(const T& m)  const {
      return (mask&m) != 0;
    }
    bool testBit(int bit) const  {
      T m = T(1)<<bit;
      return isSet(m);
    }
    bool isNull() const {
      return mask == 0;
    }
  };
  /// Standard constructor
  template <typename T>  ReferenceBitMask<T>::ReferenceBitMask(T& m) : mask(m) {}

  /*
   *   Geometry namespace declaration
   */
  namespace Geometry {

    // Put here global basic type defintiions derived from primitive types of the Geometry namespace

  }// End namespace Geometry
}      // End namespace DD4hep
#endif // DD4HEP_DD4HEP_PRIMITIVES_H
