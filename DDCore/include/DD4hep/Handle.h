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

#include <string>
#include <typeinfo>
#include <stdexcept>
#include <TNamed.h>

#ifdef DD4HEP_INSTANCE_COUNTS
#include "DD4hep/InstanceCount.h"
#endif

class TObject;
class TObjArray;
class TGeoManager;

// Conversion factor from radians to degree: 360/(2*PI)
#ifndef RAD_2_DEGREE
#define RAD_2_DEGREE 57.295779513082320876798154814105
#endif
#ifndef DEGREE_2_RAD 
#define DEGREE_2_RAD 0.017453292519943295769236907684886
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
  
  /*
   *   Geometry sub-namespace declaration
   */
  namespace Geometry  {
    struct  LCDD;
    
    std::string    _toString(bool value);
    std::string    _toString(int value);
    std::string    _toString(float value);
    std::string    _toString(double value);
    
    bool           _toBool  (const std::string& value);
    int            _toInt   (const std::string& value);
    long           _toLong  (const std::string& value);
    float          _toFloat (const std::string& value);
    double         _toDouble(const std::string& value);
    
    inline bool    _toBool(bool value)       {  return value; }
    inline int     _toInt(int value)         {  return value; }
    inline long    _toLong(long value)       {  return value; }
    inline float   _toFloat(float value)     {  return value; }
    inline double  _toDouble(double value)   {  return value; }

    template<class T> T _multiply(const std::string& left, T right);
    template<class T> T _multiply(T left, const std::string& right);
    template<class T> T _multiply(const std::string& left, const std::string& right);

    template <>        int     _multiply<int>(const std::string& left, const std::string& right);
    template <> inline int     _multiply<int>(int left, const std::string& right)
      { return left * _toInt(right);    }
    template <> inline int     _multiply<int>(const std::string& left, int right)
      { return _toInt(left) * right;    }

    template <>        long    _multiply<long>(const std::string& left, const std::string& right);
    template <> inline long    _multiply<long>(long left, const std::string& right)
      { return left * _toLong(right);   }
    template <> inline long    _multiply<long>(const std::string& left, long right)
      { return _toLong(left) * right;   }

    template <>        float   _multiply<float>(const std::string& left, const std::string& right);
    template <> inline float   _multiply<float>(float left, const std::string& right)
      { return left * _toFloat(right);  }
    template <> inline float   _multiply<float>(const std::string& left, float right)
      { return _toFloat(left) * right;  }

    template <>        double  _multiply<double>(const std::string& left, const std::string& right);
    template <> inline double  _multiply<double>(const std::string& left, double right)
      { return _toDouble(left) * right; }
    template <> inline double  _multiply<double>(double left, const std::string& right)
      { return left * _toDouble(right); }
    
    void _toDictionary(const std::string& name, const std::string& value);

    long num_object_validations();
    void increment_object_validations();
    std::string typeName(const std::type_info& type);
    
    inline unsigned long magic_word() { return 0xFEEDAFFEDEADFACEL; }
    
    /** @class Value Handle.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Counted  {
      /// Standard constructor
      Counted();
      /// Standard destructor
      virtual ~Counted();
    };

    /** @class Value Handle.h
     *  
     *  Class to simply combine to object types to one
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    template <typename Q, typename P> struct Value : public Q, public P
#ifdef DD4HEP_INSTANCE_COUNTS
      , public Counted
#endif
    {
      typedef  Q first_base;
      typedef  P second_base;
      /// Standard constructor
      Value();
      /// Standard destructor
      virtual ~Value();
    };

    template <typename Q, typename P> inline Value<Q,P>::Value()  {
#ifdef DD4HEP_INSTANCE_COUNTS
      InstanceCount::increment(this); 
#endif
    }
    template <typename Q, typename P> inline Value<Q,P>::~Value()  {
#ifdef DD4HEP_INSTANCE_COUNTS
      InstanceCount::decrement(this); 
#endif
    }
    
    /** @class Handle Handle.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    template <typename T=TNamed> struct Handle  {
      typedef T Implementation;
      typedef Handle<Implementation> handle_t;
      T* m_element;
      Handle() : m_element(0)                 {                                      }
      Handle(T* e) : m_element(e)             {                                      }
      Handle(const Handle<T>& e) : m_element(e.m_element) {                          }
      template<typename Q> Handle(Q* e)
      : m_element((T*)e)                      {  verifyObject();                     }
      template<typename Q> Handle(const Handle<Q>& e) 
      : m_element((T*)e.m_element)            {  verifyObject();                     }
      Handle<T>& operator=(const Handle<T>& e){ m_element=e.m_element; return *this; }
      bool isValid() const                    {  return 0 != m_element;              }
      bool operator!() const                  {  return 0 == m_element;              }
      void clear()                            {  m_element = 0;                      }
      T* operator->() const                   {  return  m_element;                  }
      operator T& ()  const                   {  return *m_element;                  }
      T& operator*()  const                   {  return *m_element;                  }
      T* ptr() const                          {  return m_element;                   }
      template <typename Q> Q* _ptr() const   {  return (Q*)m_element;               }
      template <typename Q> Q* data() const   {  return (Value<T,Q>*)m_element;      }
      void verifyObject() const {
        increment_object_validations();
        if ( m_element && dynamic_cast<T*>(m_element) == 0 )  {
          bad_assignment(typeid(*m_element),typeid(T));
        }
      }
      const char* name() const;
      static void bad_assignment(const std::type_info& from, const std::type_info& to);
      void  assign(Implementation* n, const std::string& nam, const std::string& title);
    };
    typedef Handle<>       Elt_t;
    typedef Handle<TNamed> Ref_t;
    
    /// Helper to delete objects from heap and reset the pointer
    template <typename T> inline void deletePtr(T*& p) {
      if(p) delete p;
      p = 0;
    }
    /// Helper to delete objects from heap and reset the pointer
    template <typename T> inline void destroyObject(T*& p) {
      deletePtr(p);
    }
    /// Helper to delete objects from heap and reset the handle
    template <typename T> inline void destroyHandle(T& h)   {
      deletePtr(h.m_element);
    }
    /// Functor to delete objects from heap and reset the pointer
    template <typename T> struct DestroyObject {
      void operator()(T& p) const { 
	destroyObject(p);
      }
    };
    /// Functor to destroy handles and delete the cached object
    template <typename T=Ref_t> struct DestroyHandle {
      void operator()(T p) const {
	destroyHandle(p);
      }
    };
    /// map Functor to delete objects from heap
    template <typename K=std::string,typename T=Ref_t> struct DestroyObjects {
      void operator()(std::pair<K,T> p) const {
	DestroyObject<T>()(p.second); 
      }
    };
    /// map Functor to destroy handles and delete the cached object
    template <typename K=std::string, typename T=Ref_t> struct DestroyHandles {
      void operator()(std::pair<K,T> p) const {
	DestroyHandle<T>()(p.second);
      }
    };
    

  }       /* End namespace Geometry  */
}         /* End namespace DD4hep    */
#endif    /* DD4HEP_ELEMENTS_H       */
