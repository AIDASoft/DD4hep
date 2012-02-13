// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4hep_ELEMENTS_H
#define DD4hep_ELEMENTS_H

#include <string>
#include <typeinfo>
#include <stdexcept>
#include <TNamed.h>

class TObject;
class TObjArray;
class TGeoManager;

// Conversion factor from radians to degree: 360/(2*PI)
#ifndef RAD_2_DEGREE
#define RAD_2_DEGREE 57.295779513082320876798154814105
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
    float          _toFloat (const std::string& value);
    double         _toDouble(const std::string& value);
    
    inline bool    _toBool(bool value)       {  return value; }
    inline int     _toInt(int value)         {  return value; }
    inline float   _toFloat(float value)     {  return value; }
    inline double  _toDouble(double value)   {  return value; }
    
    void _toDictionary(const std::string& name, const std::string& value);
    
    template<typename T> inline void deletePtr(T*& p) { if(p) delete p; p=0; }
    
    /** @class Value Handle.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    template <typename Q, typename P> struct Value : public Q, public P  {
      typedef  Q first_base;
      typedef  P second_base;
      virtual ~Value() {}
    };
    
    long num_object_validations();
    void increment_object_validations();
    
    static unsigned long magic_word() { return 0xFEEDAFFEDEADFACEL; }
    
    /** @class Handle Handle.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    template <typename T=TNamed> struct Handle  {
      typedef T Implementation;
      typedef Handle<Implementation> handle_t;
      T* m_element;
      Handle() : m_element(0)                 {                         }
      Handle(T* e) : m_element(e)             {                         }
      Handle(const Handle<T>& e) : m_element(e.m_element) {             }
      template<typename Q> Handle(Q* e)
      : m_element((T*)e)                    {  verifyObject();        }
      template<typename Q> Handle(const Handle<Q>& e) 
      : m_element((T*)e.m_element)          {  verifyObject();        }
      
      T* ptr() const                          {  return m_element;      }
      template <typename Q> Q* _ptr() const   {  return (Q*)m_element;  }
      bool isValid() const                    {  return 0 != m_element; }
      bool operator!() const                  {  return 0 == m_element; }
      T* operator->() const                   {  return  m_element;     }
      operator T& ()  const                   {  return *m_element;     }
      T& operator*()  const                   {  return *m_element;     }
      
      template <typename Q> Q* data() const  {
	      return (Value<T,Q>*)m_element;
      }
      void verifyObject() const {
        increment_object_validations();
        if ( m_element && dynamic_cast<T*>(m_element) == 0 )  {
          bad_assignment(typeid(*m_element),typeid(T));
        }
      }
      static void bad_assignment(const std::type_info& from, const std::type_info& to);
      const char* name() const;
      void  assign(Implementation* n, const std::string& nam, const std::string& title);
    };
    typedef Handle<>       Elt_t;
    typedef Handle<TNamed> Ref_t;
    
  }       /* End namespace Geometry  */
}         /* End namespace DD4hep   */
#endif    /* DD4hep_ELEMENTS_H      */
