// $Id: Primitives.h 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DD4HEP_PRIMITIVES_H
#define DD4HEP_DD4HEP_PRIMITIVES_H

#include "DDSegmentation/Segmentation.h"

// C/C++ include files
#include <algorithm>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  namespace DDSegmentation  {
    class BitField64;
    class BitFieldValue;
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
  /// ABI information about type names
  std::string typeName(const std::type_info& type);
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
      if (q)	delete q;
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
  template <typename T> struct DestroyObject {
    void operator()(T& p) const {
      destroyObject(p);
    }
  };

  /// map Functor to delete objects from heap
  template <typename M> struct DestroyObjects {
    M& object;
    DestroyObjects(M& m)
        : object(m) {
    }
    ~DestroyObjects() {
      object.clear();
    }
    void operator()(std::pair<typename M::key_type, typename M::mapped_type> p) const {
      DestroyObject<typename M::mapped_type>()(p.second);
    }
    void operator()() const {
      for_each(object.begin(),object.end(),(*this));
    }
  };
  template <typename M> DestroyObjects<M> destroyObjects(M& m) {
    return DestroyObjects<M>(m);
  }

  /// map Functor to delete objects from heap
  template <typename M> struct DestroyFirst {
    M& object;
    DestroyFirst(M& m)
        : object(m) {
    }
    ~DestroyFirst() {
      object.clear();
    }
    void operator()(std::pair<typename M::key_type, typename M::mapped_type> p) const {
      DestroyObject<typename M::key_type>()(p.first);
    }
    void operator()() const {
      for_each(object.begin(),object.end(),(*this));
    }
  };
  template <typename M> DestroyFirst<M> destroyFirst(M& m) {
    return DestroyFirst<M>(m);
  }

  /// Helper to delete objects from heap and reset the pointer. Saves many many lines of code
  template <typename T> inline void releasePtr(T*& p) {
    if (0 != p)
      p->release();
    p = 0;
  }

  /// Functor to delete objects from heap and reset the pointer
  template <typename T> struct ReleaseObject {
    void operator()(T& p) const {
      releasePtr(p);
    }
  };
  /// map Functor to delete objects from heap
  template <typename M> struct ReleaseObjects {
    M& object;
    ReleaseObjects(M& m)
        : object(m) {
    }
    ~ReleaseObjects() {
      object.clear();
    }
    void operator()(std::pair<typename M::key_type, typename M::mapped_type> p) const {
      ReleaseObject<typename M::mapped_type>()(p.second);
    }
    void operator()() const {
      for_each(object.begin(),object.end(),(*this));
    }
  };
  template <typename M> ReleaseObjects<M> releaseObjects(M& m) {
    return ReleaseObjects<M>(m);
  }


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
    bool isSet(const T& m)  const {
      return (mask&m) == m;
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
