// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4HEP_ELEMENTS_H
#define DD4HEP_ELEMENTS_H
#include "DD4hep/config.h"
#include "DD4hep/Primitives.h"

#include <string>
#include <typeinfo>
#include <stdexcept>

class TNamed;

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

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /// Access to the CXX abi name
  std::string typeName(const std::type_info& type);

  /*
   *   Geometry sub-namespace declaration
   */
  namespace Geometry {
    struct LCDD;

    std::string _toString(bool value);
    std::string _toString(int value);
    std::string _toString(float value);
    std::string _toString(double value);

    bool _toBool(const std::string& value);
    int _toInt(const std::string& value);
    long _toLong(const std::string& value);
    float _toFloat(const std::string& value);
    double _toDouble(const std::string& value);

    inline bool _toBool(bool value) {
      return value;
    }
    inline int _toInt(int value) {
      return value;
    }
    inline long _toLong(long value) {
      return value;
    }
    inline float _toFloat(float value) {
      return value;
    }
    inline double _toDouble(double value) {
      return value;
    }

    template <class T> T _multiply(const std::string& left, T right);
    template <class T> T _multiply(T left, const std::string& right);
    template <class T> T _multiply(const std::string& left, const std::string& right);

    template <> int _multiply<int>(const std::string& left, const std::string& right);
    template <> inline int _multiply<int>(int left, const std::string& right) {
      return left * _toInt(right);
    }
    template <> inline int _multiply<int>(const std::string& left, int right) {
      return _toInt(left) * right;
    }

    template <> long _multiply<long>(const std::string& left, const std::string& right);
    template <> inline long _multiply<long>(long left, const std::string& right) {
      return left * _toLong(right);
    }
    template <> inline long _multiply<long>(const std::string& left, long right) {
      return _toLong(left) * right;
    }

    template <> float _multiply<float>(const std::string& left, const std::string& right);
    template <> inline float _multiply<float>(float left, const std::string& right) {
      return left * _toFloat(right);
    }
    template <> inline float _multiply<float>(const std::string& left, float right) {
      return _toFloat(left) * right;
    }

    template <> double _multiply<double>(const std::string& left, const std::string& right);
    template <> inline double _multiply<double>(const std::string& left, double right) {
      return _toDouble(left) * right;
    }
    template <> inline double _multiply<double>(double left, const std::string& right) {
      return left * _toDouble(right);
    }

    void _toDictionary(const std::string& name, const std::string& value);

    long num_object_validations();
    void increment_object_validations();

    inline unsigned long long int magic_word() {
      return 0xFEEDAFFEDEADFACEULL;
    }

    /** @class Handle Handle.h
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    template <typename T = TNamed> struct Handle {
      typedef T Implementation;
      typedef Handle<Implementation> handle_t;
      T* m_element;
      Handle()
          : m_element(0) {
      }
      Handle(T* e)
          : m_element(e) {
      }
      Handle(const Handle<T>& e)
          : m_element(e.m_element) {
      }
      template <typename Q> Handle(Q* e)
          : m_element((T*) e) {
        verifyObject();
      }
      template <typename Q> Handle(const Handle<Q>& e)
          : m_element((T*) e.m_element) {
        verifyObject();
      }
      Handle<T>& operator=(const Handle<T>& e) {
        m_element = e.m_element;
        return *this;
      }
      bool isValid() const {
        return 0 != m_element;
      }
      bool operator!() const {
        return 0 == m_element;
      }
      void clear() {
        m_element = 0;
      }
      T* operator->() const {
        return m_element;
      }
      operator T&() const {
        return *m_element;
      }
      T& operator*() const {
        return *m_element;
      }
      T* ptr() const {
        return m_element;
      }
      template <typename Q> Q* _ptr() const {
        return (Q*) m_element;
      }
      template <typename Q> Q* data() const {
        return (Q*) m_element;
      }
      template <typename Q> Q& object() const {
        return *(Q*) m_element;
      }
      void verifyObject() const;
      const char* name() const;
      static void bad_assignment(const std::type_info& from, const std::type_info& to);
      void assign(Implementation* n, const std::string& nam, const std::string& title);
    };
    typedef Handle<TNamed> Ref_t;

    /// Helper to delete objects from heap and reset the handle
    template <typename T> inline void destroyHandle(T& h) {
      deletePtr(h.m_element);
    }
    /// Helper to delete objects from heap and reset the handle
    template <typename T> inline void releaseHandle(T& h) {
      releasePtr(h.m_element);
    }
    /// Functor to destroy handles and delete the cached object
    template <typename T> struct DestroyHandle {
      void operator()(T p) const {
        destroyHandle(p);
      }
    };
    /// Functor to destroy handles and delete the cached object
    template <typename T> struct ReleaseHandle {
      void operator()(T p) const {
        releaseHandle(p);
      }
    };
    /// map Functor to destroy handles and delete the cached object
    template <typename M> struct DestroyHandles {
      M& object;
      DestroyHandles(M& m)
          : object(m) {
      }
      ~DestroyHandles() {
        object.clear();
      }
      void operator()(std::pair<typename M::key_type, typename M::mapped_type> p) const {
        DestroyHandle<typename M::mapped_type>()(p.second);
      }
    };
    template <typename M> DestroyHandles<M> destroyHandles(M& m) {
      return DestroyHandles<M>(m);
    }

  } /* End namespace Geometry  */
} /* End namespace DD4hep    */
#endif    /* DD4HEP_ELEMENTS_H       */

