#ifndef DETDESC_LCDD_DETECTOR_H
#define DETDESC_LCDD_DETECTOR_H

// Framework include files
#include "Elements.h"
#include "Objects.h"
#include "Volumes.h"
#include "Readout.h"

// C/C++ include files
#include <map>


/*
 *   DetDesc namespace declaration
 */
namespace DetDesc {

  /*
  *   Geometry namespace declaration
  */
  namespace Geometry  {

    struct SensitiveDetector;
    struct Detector;
    struct PhysVol;
    struct Volume;
    struct Solid;
    struct LCDD;

    struct SensitiveDetector : public RefElement  {
      struct Object  {
        int         Attr_verbose;
        int         Attr_combine_hits;
        double      Attr_ecut;
        std::string Attr_eunit;
        std::string Attr_hits_collection;
        Element     Attr_segmentation;
        RefElement  Attr_id;
        Object() : Attr_verbose(0), Attr_segmentation(0) {}
      };
      SensitiveDetector() : RefElement(Handle_t(0)) {}
      SensitiveDetector(Handle_t e) : RefElement(e) {}
      SensitiveDetector(const RefElement& e) : RefElement(e) {}
      SensitiveDetector(const Document& doc, const std::string& type, const std::string& name);
      /// Access the type of the sensitive detector
      std::string type() const;
      SensitiveDetector& setCombineHits(bool value);
      /// Assign the name of the hits collection
      SensitiveDetector& setHitsCollection(const std::string& spec);
      /// Assign the IDDescriptor reference
      SensitiveDetector& setIDSpec(const RefElement& spec);
      /// Assign the readout segmentation reference
      SensitiveDetector& setSegmentation(Element seg);
    };

    struct Subdetector : public RefElement  {
      typedef std::map<std::string,Subdetector> Children;
      struct Object  {
        int               Attr_id;
        int               Attr_combine_hits;
        Solid             Attr_envelope;
        Volume            Attr_volume;
        Material          Attr_material;
        VisAttr           Attr_visualization;
        Readout           Attr_readout;
        Children          Attr_children;
        Object();
      };
      void check(bool condition, const std::string& msg) const;

      Subdetector(Handle_t e) : RefElement(e)  {}
      Subdetector(const RefElement& e) : RefElement(e)  {}
      /// Constructor for a new subdetector element
      Subdetector(const Document& doc, const std::string& name, const std::string& type, int id);
      Subdetector& setVisAttributes(const LCDD& lcdd, const std::string& solid, const Volume& volume);
      Subdetector& setRegion(const LCDD& lcdd, const std::string& name, const Volume& volume);
      Subdetector& setLimitSet(const LCDD& lcdd, const std::string& name, const Volume& volume);
      Subdetector& setAttributes(const LCDD& lcdd, const Volume& volume,
        const std::string& region, const std::string& limits, const std::string& vis);

      Subdetector& setCombineHits(bool value, SensitiveDetector& sens);
      Subdetector& add(const Subdetector& sub_element);
      int             id() const;
      std::string     type() const;
      bool            isTracker() const;
      bool            isCalorimeter() const;
      bool            isInsideTrackingVolume() const;
      bool            combineHits() const;
      Material        material() const;
      VisAttr         visAttr() const;
      Readout         readout() const;
      Subdetector&    setReadout(const Readout& readout);
      Volume          volume() const;
      Subdetector&    setVolume(const Volume& volume);
      Solid           envelope() const;
      Subdetector&    setEnvelope(const Solid& solid);
      const Children& children() const;
    };

  }       /* End namespace Geometry      */
}         /* End namespace DetDesc       */
#endif    /* DETDESC_LCDD_DETECTOR_H     */
