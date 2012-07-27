// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4hep_IDDESCRIPTOR_H
#define DD4hep_IDDESCRIPTOR_H

// Framework include files
#include "DD4hep/Handle.h"

// C++ include files
#include <string>
#include <vector>
#include <map>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  namespace Geometry {

    struct IDDescriptor : public Ref_t  {
      public:
      typedef std::pair<int,int>          Field;
      typedef std::map<std::string,Field> FieldMap;
      typedef std::map<int,std::string>   FieldIDs;
      struct Object {
	FieldMap    fieldMap;
	FieldIDs    fieldIDs;
	int         maxBit;
	Object() : maxBit(0) {}
      };
      public:
      /// Default constructor
      IDDescriptor() : Ref_t() {}
      /// Constructor to be used when reading the already parsed object
      template <typename Q> IDDescriptor(const Handle<Q>& e) : Ref_t(e) {}
      
      /// Initializing constructor
      IDDescriptor(const std::string& description);
      int maxBit() const;
      const FieldIDs& ids() const;
      const FieldMap& fields() const;
    };
  }       /* End namespace Geometry    */
}         /* End namespace DD4hep      */
#endif    /* DD4hep_IDDESCRIPTOR_H     */
