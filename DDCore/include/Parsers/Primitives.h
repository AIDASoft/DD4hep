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
#ifndef DD4HEP_PARSERS_PRIMITIVES_H
#define DD4HEP_PARSERS_PRIMITIVES_H

// C/C++ include files
#include <map>
#include <list>
#include <vector>
#include <string>
#if __cplusplus >= 201703 || (__clang__ && __APPLE__)
#include <string_view>
#endif
#include <limits>

#include <typeinfo>
#include <algorithm>
#include <stdexcept>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  class DetElement;

  /// Namespace describing generic detector segmentations
  namespace DDSegmentation  {
    class BitFieldCoder;
    class BitFieldElement;
    /// Useful typedefs to differentiate cell IDs and volume IDs
    typedef long long int CellID;
    typedef long long int VolumeID;
  }


  // Put here global basic type definitions derived from primitive types of the dd4hep namespace
#ifdef __CINT__
  typedef DDSegmentation::CellID CellID;
  typedef DDSegmentation::VolumeID VolumeID;
  typedef DDSegmentation::BitFieldCoder BitFieldCoder;
  typedef DDSegmentation::BitFieldElement BitFieldElement;
#else
  using DDSegmentation::CellID;
  using DDSegmentation::VolumeID;
  using DDSegmentation::BitFieldCoder;
  using DDSegmentation::BitFieldElement;
#endif

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

  /// Macro for deprecated functions. Prints once only. useage: deprecatedCall(__PRETTY_FUNCTION__);
#define   DD4HEP_DEPRECATED_CALL(tag,replacement,func)                \
  { static bool __dd4hep_first=true;                                  \
    if ( __dd4hep_first ) { __dd4hep_first=false;                     \
      dd4hep::printout(dd4hep::WARNING,tag,                           \
                       "Deprecated function: '%s' use '%s' instead.", \
                       func,replacement);                             \
    }}
  
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

  /// Class to perform dynamic casts using unknown pointers.
  /** @class ComponentCast Primitives.h dd4hep/Primitives.h
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
    template <typename TYPE> static void _destroy(void* arg)  {
      TYPE* ptr = (TYPE*)arg;
      if (ptr)    delete ptr;
    }
    template <typename TYPE> static void* _cast(const void* arg)  {
      TYPE* ptr = (TYPE*)arg;
      ptr = dynamic_cast<TYPE*>(ptr);
      return (void*)ptr;
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

  /// Convert volumeID to string format (016X)
  std::string volumeID(VolumeID vid);

  /// DD4hep internal namespace declaration for utilities and implementation details
  namespace detail  {
    
    /// We need it so often: one-at-time 64 bit hash function
    unsigned long long int hash64(const char* key);
    unsigned long long int hash64(const std::string& key);
    template <typename T> unsigned long long int typeHash64()   {
      static unsigned long long int code = hash64(typeid(T).name());
      return code;
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

    template <typename T> T reverseBits(T num)     {
      static constexpr size_t NO_OF_BITS = sizeof(num) * 8 - 1; 
      static constexpr T _zero = 0;
      static constexpr T _one = 1;
      T reverse_num = _zero; 
      for (size_t i = 0; i <= NO_OF_BITS; i++)     { 
        if( (num & (_one << i)) )
          reverse_num |= (_one << (NO_OF_BITS - i));
      }
      return reverse_num; 
    }

    /// Convert date into epoch time (seconds since 1970)
    long int makeTime(int year, int month, int day,
                      int hour=0, int minutes=0, int seconds=0);
  
    /// Convert date into epoch time (seconds since 1970)
    long int makeTime(const std::string& date, const char* fmt="%d-%m-%Y %H:%M:%S");
  

    /// A bit of support for handling and printing primitives
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
      typedef std::vector<value_t>              vector_t;

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
      typedef std::map<short,value_t>           short_map_t;
      /// Definition of the unsigned short integer mapped type
      typedef std::map<unsigned short,value_t>  ushort_map_t;
      /// Definition of the integer mapped type
      typedef std::map<int,value_t>             int_map_t;
      /// Definition of the unsigned integer mapped type
      typedef std::map<unsigned int,value_t>    uint_map_t;
      /// Definition of the long integer mapped type
      typedef std::map<long,value_t>            long_map_t;
      /// Definition of the unsigned long integer mapped type
      typedef std::map<unsigned long,value_t>   ulong_map_t;
      /// Definition of the size_t mapped type
      typedef std::map<size_t,value_t>          size_map_t;
      /// Definition of the string mapped type
      typedef std::map<std::string,value_t>     string_map_t;
      /// Definition of the limits
      typedef std::numeric_limits<value_t>      limits;

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
      /// Get typed null pointer (for template selctions)
      static const value_t* null_pointer() { return (value_t*)0;          }
    };

    /// Safe cast mechanism using pre-linked conversions.
    /**
     *
     * \author  M.Frank
     * \version 1.0
     * \ingroup DD4HEP
     */
#ifdef DD4HEP_USE_SAFE_CAST
    template <typename TO> class safe_cast {
    public:
      template <typename FROM> static TO* cast(FROM* from);
      template <typename FROM> static TO* cast(const FROM* from);
      template <typename FROM> static TO* cast_non_null(FROM* from);
      template <typename FROM> static TO* cast_non_null(const FROM* from);
    };
#else
    template <typename TO> class safe_cast {
    public:
      static TO* cast(TO* from);
      static TO* cast_non_null(TO* from);
      template <typename FROM> static TO* cast(FROM* from)                 { return cast((TO*)from); }
      template <typename FROM> static TO* cast(const FROM* from)           { return cast((TO*)from); }
      template <typename FROM> static TO* cast_non_null(FROM* from)        { return cast_non_null((TO*)from); }
      template <typename FROM> static TO* cast_non_null(const FROM* from)  { return cast_non_null((TO*)from); }
    };
#endif
    
    /// Operator to clear containers when out of scope
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CORE
     */
    template<typename C> struct ClearOnReturn {
      C& container;
      ClearOnReturn(C& c) : container(c) {  }
      ~ClearOnReturn() { container.clear(); }
    };

    /// Helper to copy objects.
    template <typename T> inline void copyObject(void* target,const void* source)  {
      const T* src = (const T*)source;
      ::new(target) T(*src);
    }
    /// Helper to destruct objects. Note: The memory is NOT released!
    template <typename T> inline void destructObject(T* ptr) {
      ptr->~T();
    }
    /// Helper to delete objects from heap and reset the pointer. Saves many many lines of code
    template <typename T> inline void deletePtr(T*& ptr) {
      if (0 != ptr)
        delete ptr;
      ptr = 0;
    }
    /// Helper to delete objects from heap and reset the pointer. Saves many many lines of code
    template <typename T> inline void deleteObject(T* ptr) {
      if (0 != ptr)
        delete ptr;
    }
    /// Helper to delete objects from heap and reset the pointer
    template <typename T> inline void destroyObject(T*& ptr) {
      deletePtr(ptr);
    }
    /// Functor to delete objects from heap and reset the pointer
    template <typename T> class DestroyObject {
    public:
      void operator()(T& ptr) const {
        destroyObject(ptr);
      }
    };

    /// Operator to select second element of a pair
    template <typename T> class Select2nd  {
    public:
      typedef T arg_t;
      typedef typename T::second_type result_t;
      /// Operator function
      const result_t& operator()(const arg_t &arg) const { return arg.second; }
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
      const result_t& operator()(const arg_t &arg) const { return arg.first; }
    };
    /// Generator to create Operator to select key values of a map
    template <typename T> Select1st<typename T::value_type> select1st(const T&)
    { return Select1st<typename T::value_type>(); }


    /// map Functor to delete objects from heap
    template <typename M> class DestroyObjects {
    public:
      M& object;
      DestroyObjects(M& obj) : object(obj) {                    }
      void operator()(std::pair<typename M::key_type, typename M::mapped_type> arg) const
      {  DestroyObject<typename M::mapped_type>()(arg.second);    }
      void operator()() const {
        if ( !object.empty() ) for_each(object.begin(),object.end(),(*this));
        object.clear();
      }
    };
    template <typename M> void destroyObjects(M& obj)
    {  DestroyObjects<M> del(obj); del();                      }
    template <typename M> DestroyObjects<M> destroy2nd(M& obj)
    {  DestroyObjects<M> del(obj); del();                      }

    /// map Functor to delete objects from heap
    template <typename M> class DestroyFirst {
    public:
      M& object;
      DestroyFirst(M& obj) : object(obj) {   }
      void operator()(std::pair<typename M::key_type, typename M::mapped_type> arg) const
      {  DestroyObject<typename M::key_type>()(arg.first);     }
      void operator()() const {
        if ( !object.empty() ) for_each(object.begin(),object.end(),(*this));
        object.clear();
      }
    };
    template <typename M> void destroyFirst(M& arg)
    {   DestroyFirst<M> del(arg); del();           }
    template <typename M> void destroy1st(M& arg)
    {   DestroyFirst<M> del(arg); del();           }

    /// Helper to delete objects from heap and reset the pointer. Saves many many lines of code
    template <typename T> inline void releasePtr(T& arg) {
      if (0 != arg)
        arg->release();
      arg = 0;
    }

    /// Functor to release objects from heap and reset the pointer
    template <typename T> class ReleaseObject {
    public:
      void operator()(T& arg) const {
        releasePtr(arg);
      }
    };
    /// Map Functor to release objects from heap
    template <typename M> class ReleaseObjects {
    public:
      M& object;
      ReleaseObjects(M& arg) : object(arg) {    }
      void operator()(std::pair<typename M::key_type, typename M::mapped_type> arg) const
      {  ReleaseObject<typename M::mapped_type>()(arg.second);    }
      void operator()() const {
        if ( !object.empty() ) for_each(object.begin(),object.end(),(*this));
        object.clear();
      }
    };
    template <typename M> ReleaseObject<typename M::value_type> releaseObject(M&) {
      return ReleaseObject<typename M::value_type>();
    }
    template <typename M> void releaseObjects(M& arg) {
      ReleaseObjects<M> rel(arg); rel();
    }
    template <typename M> void release2nd(M& arg) {
      ReleaseObjects<M> rel(arg); rel();
    }

    /// Functor to delete objects from heap and reset the pointer
    template <typename T> class ReferenceObject {
    public:
      typedef T arg_t;
      T operator()(T arg) const {
        if ( arg ) arg->addRef();
        return arg;
      }
    };
    /// Functor to delete objects from heap and reset the pointer
    template <typename M> class ReferenceObjects {
    public:
      typedef typename M::second_type result_t;
      result_t operator()(const M& arg) const {
        return ReferenceObject<result_t>()(arg.second);
      }
    };
    template <typename M> ReferenceObject<M> referenceObject(M&) {
      return ReferenceObject<typename M::value_type>();
    }
    template <typename M> ReferenceObjects<typename M::value_type> reference2nd(M&) {
      return ReferenceObjects<typename M::value_type>();
    }
    /// Helper to pass reference counted objects
    template <typename T> class RefCountHandle {
    public:
      T* ptr;
      RefCountHandle() : ptr(0) {}
      RefCountHandle(T* p) : ptr(p)     { if ( ptr ) ptr->addRef(); }
      RefCountHandle(const RefCountHandle& c) : ptr(c.ptr) { if ( ptr ) ptr->addRef(); }
      RefCountHandle(const RefCountHandle& c, const DetElement& ) : ptr(c.ptr) { if ( ptr ) ptr->addRef(); }
      virtual ~RefCountHandle()     { if ( ptr ) ptr->release(); }
      RefCountHandle& operator=(const RefCountHandle& c) {
        if ( &c != this )   {
          if ( ptr ) ptr->release();
          ptr = c.ptr;
          if ( ptr ) ptr->addRef();
        }
        return *this;
      }
    };
    
    /// Member function call-functor with no arguments
    template <typename R, typename T> struct ApplyMemFunc {
      typedef R (T::*memfunc_t)();
      memfunc_t func;
      ApplyMemFunc(memfunc_t f) : func(f)  {}
      void operator()(T* arg)  const {  if (arg) { (arg->*func)(); } }
    };

    /// Member function call-functor with 1 argument
    template <typename R, typename T, typename A1> struct ApplyMemFunc1 {
      typedef R (T::*memfunc_t)(A1 a1);
      memfunc_t func;
      A1& arg1;
      ApplyMemFunc1(memfunc_t f, A1& a1) : func(f), arg1(a1)  {}
      void operator()(T* arg)  const {  if ( arg ) { (arg->*func)(arg1); } }
    };

    /// Member function call-functor with 2 arguments
    template <typename R, typename T, typename A1, typename A2> struct ApplyMemFunc2 {
      typedef R (T::*memfunc_t)(A1 a1, A2 a2);
      memfunc_t func;
      A1& arg1;
      A2& arg2;
      ApplyMemFunc2(memfunc_t f, A1& a1, A2& a2) : func(f), arg1(a1), arg2(a2)  {}
      void operator()( T* arg)  const {  if ( arg ) { (arg->*func)(arg1, arg2); } }
    };

    /// Member function call-functor with no arguments (const version)
    template <typename R, typename T> struct ApplyMemFuncConst {
      typedef R (T::*memfunc_t)() const;
      memfunc_t func;
      ApplyMemFuncConst(memfunc_t f) : func(f)  {}
      void operator()(const T* arg)  const {  if ( arg ) { (arg->*func)(); } }
    };

    /// Member function call-functor with 1 argument (const version)
    template <typename R, typename T, typename A1> struct ApplyMemFuncConst1 {
      typedef R (T::*memfunc_t)(A1 a1) const;
      memfunc_t func;
      A1& arg1;
      ApplyMemFuncConst1(memfunc_t f, A1& a1) : func(f), arg1(a1)  {}
      void operator()(const T* arg)  const {  if ( arg ) { (arg->*func)(arg1); } }
    };

    /// Member function call-functor with 2 arguments (const version)
    template <typename R, typename T, typename A1, typename A2> struct ApplyMemFuncConst2 {
      typedef R (T::*memfunc_t)(A1 a1, A2 a2) const;
      memfunc_t func;
      A1& arg1;
      A2& arg2;
      ApplyMemFuncConst2(memfunc_t f, A1& a1, A2& a2) : func(f), arg1(a1), arg2(a2)  {}
      void operator()(const T* arg)  const {  if ( arg ) { (arg->*func)(arg1, arg2); } }
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
      void operator()(std::pair<typename M::key_type const, typename M::mapped_type>& arg) const
      {   (func)(arg.first);    }
      void operator()(const std::pair<typename M::key_type const, typename M::mapped_type>& arg) const
      {   (func)(arg.first);    }
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
      void operator()(std::pair<typename M::key_type const, typename M::mapped_type>& arg) const
      {   (func)(arg.second);    }
      void operator()(const std::pair<typename M::key_type const, typename M::mapped_type>& arg) const
      {   (func)(arg.second);    }
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

    /// Helper class to select the value from a mapped type
    /** 
     *  \author  M.Frank
     *  \version 1.0
     */
    template <typename C> struct get_2nd {
      const typename C::mapped_type& operator()( const typename C::value_type& v) const
      { return v.second; }
    };


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
      ReferenceBitMask(T& arg);
      T value() const  {
        return mask;
      }
      void set(const T& arg)   {
        mask |= arg;
      }
      void clear(const T& arg)   {
        mask &= ~arg;
      }
      void clear()   {
        mask = 0;
      }
      bool isSet(const T& arg)  const {
        return (mask&arg) == arg;
      }
      bool anySet(const T& arg)  const {
        return (mask&arg) != 0;
      }
      bool testBit(int bit) const  {
        T arg = T(1)<<bit;
        return isSet(arg);
      }
      bool isNull() const {
        return mask == 0;
      }
    };
    /// Standard constructor
    template <typename T>  ReferenceBitMask<T>::ReferenceBitMask(T& arg) : mask(arg) {}

  }    // End namespace detail
}      // End namespace dd4hep
#endif // DD4HEP_PARSERS_PRIMITIVES_H
