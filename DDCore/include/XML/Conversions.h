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
  namespace XML  {    struct Handle_t;  };

    /** @class Converter Conversions.h  DD4hep/compact/Conversions.h
      *
      *  Basic conversion objects for handling DD4hep XML files.
      *
      *  @author   M.Frank
      *  @version  1.0
      */
    template <typename T> struct Converter {
      typedef T        to_type;
      typedef void*    user_param;
      /// Reference to the detector description object
      Geometry::LCDD&  lcdd;
      /// Reference to optional user defined parameter
      user_param       param;
      /// Initializing constructor of the functor
      Converter(Geometry::LCDD& l) : lcdd(l), param(0) {}
      /// Initializing constructor of the functor with initialization of the user parameter
      Converter(Geometry::LCDD& l, user_param p) : lcdd(l), param(p) {}
      /// Callback operator to be specialized depending on the element type
      void operator()(const XML::Handle_t& xml) const;
    };

    /** @class PrintMap Conversions.h  DD4hep/compact/Conversions.h
      *
      *  Small helper class to print maps of objects
      *
      *  @author   M.Frank
      *  @version  1.0
      */
    template <typename T> struct PrintMap {
      typedef T item_type;
      typedef const Geometry::LCDD::HandleMap cont_type;

      /// Reference to the detector description object
      const Geometry::LCDD&   lcdd;
      /// Reference to the output stream object, the Printer object should write
      std::ostream&           os;
      /// Optional text prefix when formatting the output
      std::string             text;
      /// Reference to the container data of the map.
      cont_type&              cont;
      /// Initializing constructor of the functor
      PrintMap(const Geometry::LCDD& l, std::ostream& stream, cont_type& c, const std::string& t="") 
	: lcdd(l), os(stream), text(t), cont(c)  {}
      /// Callback operator to be specialized depending on the element type
      void operator()() const;
    };

    /** @class Printer Conversions.h  DD4hep/compact/Conversions.h
      *
      *  Small helper class to print objects
      *
      *  @author   M.Frank
      *  @version  1.0
      */
    template <typename T> struct Printer  {
      /// Reference to the detector description object
      const Geometry::LCDD&   lcdd;
      /// Reference to the output stream object, the Printer object should write
      std::ostream&           os;
      /// Optional text prefix when formatting the output
      std::string             prefix;
      /// Initializing constructor of the functor
      Printer(const Geometry::LCDD& l, std::ostream& stream, const std::string& p="") 
	: lcdd(l), os(stream), prefix(p) {}
      /// Callback operator to be specialized depending on the element type
      void operator()(const T& value) const;
    };

    /// Helper function to print booleans in format YES/NO
    inline const char* yes_no(bool value)     { return value ? "YES"   : "NO ";   }
    /// Helper function to print booleans in format true/false
    inline const char* true_false(bool value) { return value ? "true " : "false"; }

}         /* End namespace DD4hep   */
#endif    /* DD4hep_COMPACT_CONVERSION_H    */
