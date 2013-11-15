// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_COMPONENTTUILS_H
#define DD4HEP_DDG4_COMPONENTTUILS_H

// C/C++ include files
#include <stdexcept>
#include <typeinfo>
#include <string>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /// ABI information about type names
  std::string typeinfoName(const std::type_info& type);
  void typeinfoCheck(const std::type_info& typ1, const std::type_info& typ2, const std::string& text = "");

  /** @class unrelated_type_error
   *
   *   @author  M.Frank
   *   @date    13.08.2013
   */
  struct unrelated_type_error : public std::runtime_error {
    static std::string msg(const std::type_info& typ1, const std::type_info& typ2, const std::string& text);
    unrelated_type_error(const std::type_info& typ1, const std::type_info& typ2, const std::string& text = "")
        : std::runtime_error(msg(typ1, typ2, text)) {
    }
  };

  /** @class
   *
   *   @author  M.Frank
   *   @date    13.08.2013
   */
  class ComponentCast {
  public:
    typedef void  (*destroy_t)(void*);
    typedef void* (*cast_t)(const void*);
    const std::type_info& type;
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

}      // End namespace DD4hep

#endif // DD4HEP_DDG4_COMPONENTTUILS_H
