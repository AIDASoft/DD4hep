// $Id: Detector.h 1087 2014-04-09 12:25:51Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEOMETRY_OBJECTSINTERNA_H
#define DD4HEP_GEOMETRY_OBJECTSINTERNA_H

// Framework include files
#include "DD4hep/Volumes.h"
#include "DD4hep/NamedObject.h"
#include "DD4hep/Segmentations.h"
#include "DDSegmentation/BitField64.h"

// C/C++ include files
#include <set>

// Forward declarations
class TColor;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geometry namespace declaration
   */
  namespace Geometry {

    /** @class HeaderObject ObjectsInterna.h DD4hep/objects/ObjectsInterna.h
     *
     *  
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    class HeaderObject: public NamedObject {
    public:
      std::string url;
      std::string author;
      std::string status;
      std::string version;
      std::string comment;
      /// Standard constructor
      HeaderObject();
      /// Default destructor
      virtual ~HeaderObject();
    private:
      /// Private copy constructor
      HeaderObject(const HeaderObject&) : NamedObject() {}
      /// Private assignment operator
      HeaderObject& operator=(const HeaderObject&) { return *this; }
    };

    /** @class VisAttrObject ObjectsInterna.h DD4hep/objects/ObjectsInterna.h
     *
     *  
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    class VisAttrObject: public NamedObject {
    public:
      unsigned long magic;
      TColor* col;
      int color;
      float alpha;
      unsigned char drawingStyle, lineStyle, showDaughters, visible;
      /// Standard constructor
      VisAttrObject();
      /// Default destructor
      virtual ~VisAttrObject();
    };

    /** @class RegionObject ObjectsInterna.h DD4hep/objects/ObjectsInterna.h
     *
     *  
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    class RegionObject: public NamedObject {
    public:
      unsigned long magic;
      double threshold;
      double cut;
      bool store_secondaries;
      std::string lunit, eunit;
      std::vector<std::string> user_limits;
      /// Standard constructor
      RegionObject();
      /// Default destructor
      virtual ~RegionObject();
    };

    /** @class LimitSetObject ObjectsInterna.h DD4hep/objects/ObjectsInterna.h
     *
     *  
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    class LimitSetObject: public NamedObject, public std::set<Limit> {
    public:
      /// Standard constructor
      LimitSetObject();
      /// Default destructor
      virtual ~LimitSetObject();
    };

    /** @class Readout::Object  Readout.h DD4hep/Readout.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct ReadoutObject: public NamedObject {
      /// Handle to the readout segmentation
      Segmentation segmentation;
      /// Handle to the volume
      Volume readoutWorld;
      /// Handle to the field descriptor
      IDDescriptor id;
      /// Standard constructor
      ReadoutObject();
      /// Default destructor
      virtual ~ReadoutObject();
    };

    /** @class IDDescriptor::Object IDDescriptor.h DDCore/IDDescriptor.h
     *
     *  @author  M.Frank
     *  @version 1.0
     *  @date    2012/07/31
     */
    class IDDescriptorObject: public NamedObject, public BitField64 {
    public:
      typedef BitFieldValue* Field;
      typedef std::vector<std::pair<std::string, Field> > FieldMap;
      typedef std::vector<std::pair<size_t, std::string> > FieldIDs;
      FieldMap fieldMap; //! not ROOT-persistent
      FieldIDs fieldIDs; //! not ROOT-persistent
      std::string description;
      /// Default constructor
      IDDescriptorObject();
      /// Standard constructor
      IDDescriptorObject(const std::string& initString);
      /// Default destructor
      virtual ~IDDescriptorObject();
#ifndef __CINT__
      /// Access to the field container of the BitField64
      const std::vector<BitFieldValue*> fields() const {
	return _fields;
      }
#endif
    };


  } /* End namespace Geometry      */
} /* End namespace DD4hep        */
#endif /* DD4HEP_GEOMETRY_OBJECTSINTERNA_H  */
