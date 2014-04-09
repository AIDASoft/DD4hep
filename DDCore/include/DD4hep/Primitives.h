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

  // Put here global basic type definitions derived from primitive types of the DD4hep namespace
  typedef DDSegmentation::CellID CellID;
  typedef DDSegmentation::VolumeID VolumeID;

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
  /*
   *   Geometry namespace declaration
   */
  namespace Geometry {

  // Put here global basic type defintiions derived from primitive types of the Geometry namespace

  }// End namespace Geometry
}      // End namespace DD4hep
#endif // DD4HEP_DD4HEP_PRIMITIVES_H
