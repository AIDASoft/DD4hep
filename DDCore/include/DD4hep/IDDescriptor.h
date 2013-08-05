// $Id$
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
#include "DD4hep/BitField64.h"

// C++ include files
#include <string>
#include <vector>
#include <map>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geometry sub-namespace declaration
   */
  namespace Geometry {

    /** @class IDDescriptor IDDescriptor.h DDCore/IDDescriptor.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     *  @date    2012/07/31
     */
    struct IDDescriptor : public Ref_t  {
    public:
      typedef unsigned long long int VolumeID;
#if 0
      //typedef std::pair<int,int>          Field;
      struct Field  {
	int first, second;
	VolumeID mask;
	VolumeID encode(int value)  const  {
	  VolumeID v = value;
	  return  ( (  v  << first )  | mask  ) ;
	  //xx	  return mask|((v<<(64-second))>>first);

	}
	int decode(VolumeID value)  const  {
	  return  ( ~mask & value ) >> first  ;
	  //xx return (~mask&value)>>(64-second-first);
	}
      };
#endif
      typedef std::pair<std::string,int> VolID;
      typedef BitFieldValue* Field;
      typedef std::vector<std::pair<std::string,Field> >  FieldMap;
      typedef std::vector<std::pair<size_t,std::string> > FieldIDs;

      /** @class IDDescriptor::Object IDDescriptor.h DDCore/IDDescriptor.h
       *  
       *  @author  M.Frank
       *  @version 1.0
       *  @date    2012/07/31
       */
      struct Object : public TNamed, public BitField64 {
	FieldMap      fieldMap;
	FieldIDs      fieldIDs;
	std::string   description;
	//unsigned      maxBit;
	/// Standard constructor
	Object(const std::string& initString);
	/// Default destructor
	virtual ~Object();
      };
      public:
      /// Default constructor
      IDDescriptor() : Ref_t() {}
      /// Constructor to be used when reading the already parsed object
      template <typename Q> IDDescriptor(const Handle<Q>& e) : Ref_t(e) {}
      /// Initializing constructor
      IDDescriptor(const std::string& description);
      // the string description of all fields
      std::string fieldDescription() const  ;
      /// The total number of encoding bits for this descriptor
      unsigned maxBit() const;
      /// Access the field-id container 
      const FieldIDs& ids() const;
      /// Access the fieldmap container 
      const FieldMap& fields() const;
      /// Get the field descriptor of one field by name
      Field field(const std::string& field_name)  const;
      /// Get the field identifier of one field by name
      size_t fieldID(const std::string& field_name)  const;
      /// Get the field descriptor of one field by its identifier
      Field field(size_t identifier)  const;
      /// Encoede a set of volume identifiers (corresponding to this description of course!) to a volumeID.
      VolumeID encode(const std::vector<VolID>& ids)  const;
      /// Acces string representation
      std::string toString() const;
    };
  }       /* End namespace Geometry    */
}         /* End namespace DD4hep      */
#endif    /* DD4hep_IDDESCRIPTOR_H     */
