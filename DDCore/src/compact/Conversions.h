// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4hep_COMPACT_CONVERSION_H
#define DD4hep_COMPACT_CONVERSION_H

// C/C++ include files
#include <map>
#include <iostream>
#include "DD4hep/LCDD.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  // Forward declarations
  namespace XML  {
    struct Handle_t;
  };

  /*
   *   XML namespace declaration
   */
  namespace Geometry  {

    // Function prototypes used for object conversions
    template <typename T> Ref_t    toRefObject(LCDD& lcdd, const XML::Handle_t& xml);

    /** @class Converter Conversions.h  DD4hep/compact/Conversions.h
      *
      *  @author   M.Frank
      *  @version  1.0
      */
    template <typename T> struct Converter {
      typedef T to_type;
      LCDD&  lcdd;
      Converter(LCDD& l) : lcdd(l) {}
      void operator()(const XML::Handle_t& xml) const;
    };

    /** @class PrintMap Conversions.h  DD4hep/compact/Conversions.h
      *
      *  @author   M.Frank
      *  @version  1.0
      */
    template <typename T> struct PrintMap {
      typedef T item_type;
      typedef const LCDD::HandleMap cont_type;

      const LCDD&   lcdd;
      std::ostream& os;
      std::string   text;
      cont_type&    cont;
      PrintMap(const LCDD& l, std::ostream& stream, cont_type& c, const std::string& t="") 
	: lcdd(l), os(stream), text(t), cont(c)  {}
      void operator()() const;
    };

    /** @class Printer Conversions.h  DD4hep/compact/Conversions.h
      *
      *  @author   M.Frank
      *  @version  1.0
      */
    template <typename T> struct Printer  {
      const LCDD&   lcdd;
      std::ostream& os;
      std::string   prefix;
      Printer(const LCDD& l, std::ostream& stream, const std::string& p="") 
	: lcdd(l), os(stream), prefix(p) {}
      void operator()(const T& value) const;
    };
    inline const char* yes_no(bool value) { return value ? "YES" : "NO "; }

  }       /* End namespace Geometry  */
}         /* End namespace DD4hep   */
#endif    /* DD4hep_COMPACT_CONVERSION_H    */
