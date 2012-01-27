#ifndef DETDESC_ELEMENTS_H
#define DETDESC_ELEMENTS_H

#include <string>
#include <stdexcept>

class TObject;
class TObjArray;
class TGeoManager;

/*
 *   Gaudi namespace declaration
 */
namespace DetDesc {

  namespace Geometry  {
    struct  LCDD;
    struct  Element;
    struct  RefElement;
    struct  Document;
    struct  Handle_t;
    struct  Collection_t;

    typedef TObject     Element_t;
    typedef TObjArray   NodeList_t;
    typedef TGeoManager Document_t;

    bool           _toBool  (const std::string& value);
    int            _toInt   (const std::string& value);
    float          _toFloat (const std::string& value);
    double         _toDouble(const std::string& value);

    inline bool    _toBool(bool value)       {  return value; }
    inline int     _toInt(int value)         {  return value; }
    inline float   _toFloat(float value)     {  return value; }
    inline double  _toDouble(double value)   {  return value; }

    std::string    _toString(bool value);
    std::string    _toString(int value);
    std::string    _toString(float value);
    std::string    _toString(double value);

    void _toDictionary(const std::string& name, const std::string& value);

    template<class T> inline void deletePtr(T*& p) { if(p) delete p; p=0; }

    struct Handle_t  {
      Element_t* m_node;
      Handle_t(const Handle_t& c) : m_node(c.m_node) {}
      Handle_t(Element_t* e=0) : m_node(e)        {                                        }
      ~Handle_t()                                 {                                        }
      //Element_t* operator->() const               { return m_node;                         }
      //operator Element_t* () const                { return m_node;                         }
      bool isValid() const                        { return 0 != m_node;                    }
      Element_t* ptr() const                      { return m_node;                         }
      template <typename T> T* _ptr() const       { return (T*)m_node;                     }
    };

    struct Document  {
      Document_t* m_doc;
      Document(Document_t* d) : m_doc(d) {}
      operator Document_t*() const   {  return m_doc; }
      Document_t* operator->() const {  return m_doc; }
      Element_t*  createElt(const std::string& tag) const;
    };

    struct Element  {
      Handle_t m_element;
      Element(const Handle_t&  e) : m_element(e) {                            }
      Element(const Document& document, const std::string& type);
      Element_t* ptr() const                  {  return m_element.ptr();      }
      bool isValid() const                    {  return 0 != m_element.ptr(); }
      bool operator!() const                  {  return 0 == m_element.ptr(); }
      //operator Handle_t () const              {  return m_element;            }
      //operator Element_t*() const             {  return m_element;            }
    };

    struct RefElement : public Element  {
      RefElement() : Element(0) {}
      RefElement(const Handle_t& e);
      RefElement(const Document& d, const std::string& type, const std::string& name);
      const char* name() const;
      const char* refName() const;
      void setName(const std::string& new_name);
    };

  }       /* End namespace Geometry  */
}         /* End namespace DetDesc   */
#endif    /* DETDESC_ELEMENTS_H      */
