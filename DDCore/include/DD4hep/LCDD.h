// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4HEP_LCDD_LCDD_H
#define DD4HEP_LCDD_LCDD_H

// Framework includes
#include "DD4hep/Handle.h"
#include "DD4hep/Fields.h"
#include "DD4hep/Objects.h"
#include "DD4hep/Shapes.h"
#include "DD4hep/Volumes.h"
#include "DD4hep/Readout.h"
#include "DD4hep/Detector.h"
#include "DD4hep/Segmentations.h"

// C/C++ include files
#include <map>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {
  
  /*
   *   Geometry namespace declaration
   */
  namespace Geometry  {

    /** @class LCDD LCDD.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct LCDD {
      typedef std::map<std::string,Handle<> > HandleMap;
      typedef std::map<std::string,std::string> PropertyValues;
      typedef std::map<std::string,PropertyValues>  Properties;

      /// Destructor
      virtual ~LCDD() {}

      /// Initialize geometry
      virtual void init() = 0;
      /// Finalize the geometry
      virtual void endDocument() = 0;

      /// Access to properties map
      virtual Properties& properties() const = 0;
      /// Return handle to material describing air
      virtual Material air() const = 0;
      /// Return handle to material describing vacuum
      virtual Material vacuum() const = 0;
      /// Return reference to the top-most (world) detector element
      virtual DetElement world() const = 0;
      /// Return reference to detector element with all tracker devices.
      virtual DetElement trackers() const = 0;

      /// Return handle to the world volume containing everything
      virtual Volume   worldVolume() const = 0;
      /// Return handle to the world volume containing the volume with the tracking devices
      virtual Volume   trackingVolume() const = 0;

      /// Return handle to the combined electromagentic field description.
      virtual OverlayedField field() const = 0;

      /// Accessor to the map of header entries
      virtual const HandleMap& header()  const = 0;
      /// Accessor to the map of constants
      virtual const HandleMap& constants()  const = 0;
      /// Accessor to the map of region settings
      virtual const HandleMap& regions() const = 0;
      /// Accessor to the map of materials
      virtual const HandleMap& materials()  const = 0;
      /// Accessor to the map of sub-detectors
      virtual const HandleMap& detectors()  const = 0;
      /// Accessor to the map of readout structures
      virtual const HandleMap& readouts() const = 0;
      /// Accessor to the map of visualisation attributes
      virtual const HandleMap& visAttributes() const = 0;
      /// Accessor to the map of limit settings
      virtual const HandleMap& limitsets()  const = 0;
      /// Accessor to the map of aligment entries
      virtual const HandleMap& alignments()  const = 0;
      /// Accessor to the map of field entries, which together form the global field
      virtual const HandleMap& fields()  const = 0;

      virtual Volume            pickMotherVolume(const DetElement& sd)     const = 0;

      /// Retrieve a constant by it's name from the detector description
      virtual Constant          constant(const std::string& name)          const = 0;
      /// Retrieve a matrial by it's name from the detector description
      virtual Material          material(const std::string& name)          const = 0;
      /// Retrieve a id descriptor by it's name from the detector description
      virtual IDDescriptor      idSpecification(const std::string& name)   const = 0;
      /// Retrieve a region object by it's name from the detector description
      virtual Region            region(const std::string& name)            const = 0;
      /// Retrieve a visualization attribute by it's name from the detector description
      virtual VisAttr           visAttributes(const std::string& name)     const = 0;
      /// Retrieve a limitset by it's name from the detector description
      virtual LimitSet          limitSet(const std::string& name)          const = 0;
      /// Retrieve a readout object by it's name from the detector description
      virtual Readout           readout(const std::string& name)           const = 0;
      /// Retrieve an alignment entry by it's name from the detector description
      virtual AlignmentEntry    alignment(const std::string& path)         const = 0;
      /// Retrieve a sensitive detector by it's name from the detector description
      virtual SensitiveDetector sensitiveDetector(const std::string& name) const = 0;
      /// Retrieve a subdetector element by it's name from the detector description
      virtual CartesianField    field(const std::string& name)             const = 0;
      /// Retrieve a field component by it's name from the detector description
      virtual DetElement        detector(const std::string& name)          const = 0;

      /// Add a new constant to the detector description
      virtual LCDD& add(Constant constant)    = 0;
      /// Add a new material to the detector description
      virtual LCDD& add(Material mat)         = 0;
      /// Add a new visualisation attribute to the detector description
      virtual LCDD& add(VisAttr attr)         = 0;
      /// Add a new limit set to the detector description
      virtual LCDD& add(LimitSet limitset)    = 0;
      /// Add a new detector region to the detector description
      virtual LCDD& add(Region region)        = 0;
      /// Add a new id descriptor to the detector description
      virtual LCDD& add(IDDescriptor spec)    = 0;
      /// Add a new detector readout to the detector description
      virtual LCDD& add(Readout readout)      = 0;
      /// Add a new subdetector to the detector description
      virtual LCDD& add(DetElement detector)  = 0;
      /// Add alignment entry to the detector description
      virtual LCDD& add(AlignmentEntry entry) = 0;
      /// Add a field component to the detector description
      virtual LCDD& add(CartesianField entry) = 0;

      /// Add a new constant by named reference to the detector description
      virtual LCDD& addConstant(const Ref_t& element) = 0;
      /// Add a new material by named reference to the detector description
      virtual LCDD& addMaterial(const Ref_t& element) = 0;
      /// Add a new visualisation attribute by named reference to the detector description
      virtual LCDD& addVisAttribute(const Ref_t& element) = 0;
      /// Add a new limit set by named reference to the detector description
      virtual LCDD& addLimitSet(const Ref_t& limset) = 0;
      /// Add a new id descriptor by named reference to the detector description
      virtual LCDD& addIDSpecification(const Ref_t& element) = 0;
      /// Add a new detector region by named reference to the detector description
      virtual LCDD& addRegion(const Ref_t& region) = 0;
      /// Add a new detector readout by named reference to the detector description
      virtual LCDD& addReadout(const Ref_t& readout) = 0;
      /// Add a new sensitive detector by named reference to the detector description
      virtual LCDD& addSensitiveDetector(const Ref_t& element) = 0;
      /// Add a new subdetector by named reference to the detector description
      virtual LCDD& addDetector(const Ref_t& detector) = 0;
      /// Add alignment entry by named reference to the detector description
      virtual LCDD& addAlignment(const Ref_t& alignment) = 0;
      /// Add a field component by named reference to the detector description
      virtual LCDD& addField(const Ref_t& field) = 0;

      /// Read compact geometry description or alignment file
      virtual void fromCompact(const std::string& fname) = 0;
      /// Read any geometry description or alignment file
      virtual void fromXML(const std::string& fname) = 0;
      /// 
      virtual void dump() const = 0;

      ///---Factory method-------
      static LCDD& getInstance(void);
    };
  }       /* End namespace Geometry  */
}         /* End namespace DD4hep   */
#endif    /* DD4HEP_LCDD_LCDD_H     */
