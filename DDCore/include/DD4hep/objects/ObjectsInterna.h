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

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    /// Concrete object implementation for the Header handle
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
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

    /// Concrete object implementation for the Constant handle
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     */
    class ConstantObject: public NamedObject {
    public:
      /// Constant type
      std::string m_type;
      /// Standard constructor
      ConstantObject(const std::string& nam, const std::string& val, const std::string& typ);
      /// Default destructor
      virtual ~ConstantObject();
    private:
      /// Private copy constructor
      ConstantObject(const ConstantObject&) : NamedObject() {}
      /// Private assignment operator
      ConstantObject& operator=(const ConstantObject&) { return *this; }
    };

    /// Concrete object implementation of the VisAttr Handle
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
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

    /// Concrete object implementation of the Region Handle
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     */
    class RegionObject: public NamedObject {
    public:
      unsigned long magic;
      double threshold;
      double cut;
      bool store_secondaries;
      std::vector<std::string> user_limits;
      /// Standard constructor
      RegionObject();
      /// Default destructor
      virtual ~RegionObject();
    };

    /// Concrete object implementation of the LimitSet Handle
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     */
    class LimitSetObject: public NamedObject, public std::set<Limit> {
    public:
      /// Standard constructor
      LimitSetObject();
      /// Default destructor
      virtual ~LimitSetObject();
    };

    /// Concrete object implementation of the Readout Handle
    /**
     *
     * \author  M.Frank
     * \version 1.0
     * \ingroup DD4HEP_GEOMETRY
     */
    class ReadoutObject: public NamedObject {
    public:
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

    /// Concrete object implementation of the IDDescriptorObject Handle
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \date    2012/07/31
     *  \ingroup DD4HEP_GEOMETRY
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
