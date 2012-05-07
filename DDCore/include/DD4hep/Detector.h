// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4hep_LCDD_DETECTOR_H
#define DD4hep_LCDD_DETECTOR_H

// Framework include files
#include "DD4hep/Handle.h"
#include "DD4hep/Objects.h"
#include "DD4hep/Volumes.h"
#include "DD4hep/Readout.h"
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
    
    struct SensitiveDetector;
    struct Detector;
    struct LCDD;
    
    /** @class SensitiveDetector Detector.h DD4hep/lcdd/Detector.h
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    struct SensitiveDetector : public Ref_t  {
      struct Object  {
        unsigned int magic;
        int          verbose;
        int          combine_hits;
        double       ecut;
        std::string  eunit;
        std::string  hits_collection;
        Segmentation segmentation;
        Ref_t  id;
        Object() : magic(magic_word()), verbose(0), segmentation() {}
      };
      
      /// Default constructor
      SensitiveDetector() : Ref_t() {}
      /// Templated constructor for handle conversions
      template <typename Q>
      SensitiveDetector(const Handle<Q>& e) : Ref_t(e) {}
      /// Constructor for a new sensitive detector element
      SensitiveDetector(const LCDD& lcdd, const std::string& type, const std::string& name);
      
      /// Additional data accessor
      Object& _data()   const {  return *data<Object>();  }
      /// Access the type of the sensitive detector
      std::string type() const;
      /// Set flag to handle hits collection
      SensitiveDetector& setCombineHits(bool value);
      /// Assign the name of the hits collection
      SensitiveDetector& setHitsCollection(const std::string& spec);
      /// Assign the IDDescriptor reference
      SensitiveDetector& setIDSpec(const Ref_t& spec);
      /// Assign the readout segmentation reference
      SensitiveDetector& setSegmentation(const Segmentation& seg);
      
    };
    
    /** @class SubDetector Detector.h DD4hep/lcdd/Detector.h
     *
     *  Please note: 
     *  Though nowhere enforced, it is obvious that only placements
     *  of the **SAME** logical volume should be added.
     *
     *  We explicitly rely here on the common sense of the user
     *  of this class. People without a brain can always screw 
     *  things completely - nothing one can do about!
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    struct DetElement : public Ref_t   {
      typedef Ref_t                            Parent;
      typedef std::map<std::string,DetElement> Children;
      typedef std::vector<PlacedVolume>        Placements;
      
      enum {
	COPY_NONE      = 0,
	COPY_PLACEMENT = 1<<0,
	COPY_PARENT    = 1<<1,
	COPY_ALIGNMENT = 1<<2,
	LAST            
      } CopyParameters;
      struct Object  {
        unsigned int      magic;
        int               id;
	std::string       path;
        int               combine_hits;
        Volume            volume;
        Readout           readout;
        Alignment         alignment;
        Conditions        conditions;
	PlacedVolume      placement;
        Placements        placements;
	Parent            parent;
        Children          children;
	/// Default constructor
        Object();
	/// Construct new empty object
	virtual Value<TNamed,Object>* construct(int new_id, int flag) const;
	/// Deep object copy to replicate DetElement trees e.g. for reflection
	virtual void deepCopy(const Object& source, int new_id, int flag);
      };

      /// Additional data accessor
      Object& _data()   const {  return *data<Object>();  }
      
      void check(bool condition, const std::string& msg) const;
      
      /// Default constructor
      DetElement() : Ref_t()  {}

      /// Default constructor
      template<typename Q> DetElement(Q* data, const std::string& name, const std::string& type) : Ref_t(data)  
      {  this->assign(data, name, type);                   }
      /// Templated constructor for handle conversions
      template<typename Q> DetElement(const Handle<Q>& e) : Ref_t(e)  {}
      /// DEPRECATED: Constructor for a new subdetector element. First argument lcdd is unused!
      DetElement(const LCDD& lcdd, const std::string& name, const std::string& type, int id);

      /// Constructor for a new subdetector element
      DetElement(const std::string& name, const std::string& type, int id);

      /// Clone (Deep copy) the DetElement structure with a new name
      DetElement clone(const std::string& new_name) const;
      /// Clone (Deep copy) the DetElement structure with a new name and new identifier
      DetElement clone(const std::string& new_name, int new_id) const;
      
      DetElement&     setCombineHits(bool value, SensitiveDetector& sens);
      DetElement&     add(const DetElement& sub_element);
      int             id() const;
      std::string     type() const;
      std::string     path() const;
      bool            isTracker() const;
      bool            isCalorimeter() const;
      //bool            isInsideTrackingVolume() const;
      bool            combineHits() const;
      
      /// Set all attributes in one go
      DetElement& setAttributes(const LCDD& lcdd, const Volume& volume,
                                const std::string& region, 
                                const std::string& limits, 
                                const std::string& vis);
      
      /// Set Visualization attributes to the detector element
      DetElement& setVisAttributes(const LCDD& lcdd, const std::string& name, const Volume& volume);
      /// Set the regional attributes to the detector element
      DetElement& setRegion(const LCDD& lcdd, const std::string& name, const Volume& volume);
      /// Set the limits to the detector element
      DetElement& setLimitSet(const LCDD& lcdd, const std::string& name, const Volume& volume);
      
      /// Access the readout structure 
      Readout         readout() const;
      /// Assign readout definition
      DetElement&     setReadout(const Readout& readout);
      
      /// Access to the physical volume of this detector element
      PlacedVolume    placement() const;
      /// Set the physical volumes of the detector element
      DetElement&     setPlacement(const PlacedVolume& volume);

      /// Access the physical volumes of the detector element
      Placements      placements() const;
      /// Set the physical volumes of the detector element
      DetElement&     addPlacement(const PlacedVolume& volume);

      /// Access to the logical volume of the placements (all daughters have the same!)
      Volume          volume() const;
      
      /// Access to the list of children
      const Children& children() const;
      /// Access to individual children by name
      DetElement      child(const std::string& name) const;
    };
    
  }       /* End namespace Geometry      */
}         /* End namespace DD4hep       */
#endif    /* DD4hep_LCDD_DETECTOR_H     */
