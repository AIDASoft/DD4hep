// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4hep_GEOMETRY_VOLUMES_H
#define DD4hep_GEOMETRY_VOLUMES_H

// Framework include files
#include "DD4hep/Handle.h"
#include "DD4hep/Shapes.h"
#include "DD4hep/Objects.h"

// C/C++ include files
#include <map>

// ROOT include file (includes TGeoVolume + TGeoShape)
#include "TGeoNode.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {
  
  /*
   *   Geometry namespace declaration
   */
  namespace Geometry  {
    
    // Forward declarations
    struct LCDD;
    struct Region;
    struct LimitSet;
    struct Material;
    struct Volume;
    struct PlacedVolume;
    struct Position;
    struct Rotation;
    struct VisAttr;
    struct SensitiveDetector;
    
    /** @class PlacedVolume Volume.h  DD4hep/lcdd/Volume.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct PlacedVolume : Handle<TGeoNodeMatrix> {
      typedef std::map<std::string,int> VolIDs;
      struct Object  {
        unsigned long magic;
        VolIDs        volIDs;
        Ref_t         detector;
        Object() : volIDs(), detector() {}
      };
      /// Constructor to be used when reading the already parsed DOM tree
      PlacedVolume(const TGeoNode* e) : Handle<TGeoNodeMatrix>(e) {}
      /// Default constructor
      PlacedVolume() : Handle<TGeoNodeMatrix>() {}
      /// Copy assignment
      PlacedVolume(const PlacedVolume& e) : Handle<TGeoNodeMatrix>(e) {}
      /// Copy assignment from other handle type
      template <typename T> PlacedVolume(const Handle<T>& e) : Handle<TGeoNodeMatrix>(e) {}
      /// Add identifier
      PlacedVolume& addPhysVolID(const std::string& name, int value);
      /// Volume material
      Material material() const;
      /// Logical volume of this placement
      Volume   volume() const;
      /// Parent volume (envelope)
      Volume motherVol() const;
      /// Access to the volume IDs
      const VolIDs& volIDs() const;
      /// Set the detector handle
      void setDetElement(Ref_t detector)  const;
      /// Access to the corresponding detector element (maybe invalid)
      Ref_t detElement() const;
      /// String dump
      std::string toString() const;
    };
    
    /** @class Volume Volume.h  DD4hep/lcdd/Volume.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Volume : public Handle<TGeoVolume>  {
      typedef Handle<TGeoVolume> Base;
      struct Object  {
        unsigned long magic;
        Region        region;
        LimitSet      limits;
        VisAttr       vis;
        Ref_t         sens_det;
        Object() : region(), limits(), vis(), sens_det() {}
      };
      /// Default constructor
      Volume() : Base(0) {}
      
      /// Copy from handle
      Volume(const Volume& v) : Base(v) {}
      
      /// Copy from arbitrary Element
      template <typename T> Volume(const Handle<T>& v) : Base(v) {}
      
      /// Constructor to be used when creating a new geometry tree.
      Volume(LCDD& lcdd, const std::string& name);
      
      /// Constructor to be used when creating a new geometry tree. Also sets materuial and solid attributes
      Volume(LCDD& lcddument, const std::string& name, const Solid& s, const Material& m);
      
      /// Place daughter volume. The position and rotation are the identity
      PlacedVolume placeVolume(const Volume& vol)  const  
      { return placeVolume(vol,IdentityPos());                        }
      /// Place un-rotated daughter volume at the given position.
      PlacedVolume placeVolume(const Volume& vol, const Position& pos)  const;
      /// Place rotated daughter volume. The position is automatically the identity position
      PlacedVolume placeVolume(const Volume& vol, const Rotation& rot)  const;
      /// Place translated and rotated daughter volume
      PlacedVolume placeVolume(const Volume& vol, const Position& pos, const Rotation& rot)  const;
      
      /// Place daughter volume. The position and rotation are the identity
      PlacedVolume placeVolume(const Volume& vol, const IdentityPos& pos)  const;
      /// Place daughter volume. The position and rotation are the identity
      PlacedVolume placeVolume(const Volume& vol, const IdentityRot& pos)  const;
      
      /// Attach attributes to the volume
      void setAttributes(const LCDD& lcdd,
                         const std::string& region, 
                         const std::string& limits, 
                         const std::string& vis) const;
      
      /// Set the regional attributes to the volume
      void setRegion(const Region& obj)  const;
      /// Access to the handle to the region structure
      Region region() const;
      
      /// Set the limits to the volume
      void setLimitSet(const LimitSet& obj)  const;
      /// Access to the limit set
      LimitSet limitSet() const;
      
      /// Set Visualization attributes to the volume
      void setVisAttributes(const VisAttr& obj) const;
      /// Set Visualization attributes to the volume
      void setVisAttributes(const LCDD& lcdd, const std::string& name) const;
      /// Access the visualisation attributes
      VisAttr  visAttributes() const;
      
      /// Assign the sensitive detector structure
      void setSensitiveDetector(const SensitiveDetector& obj) const;
      /// Access to the handle to the sensitive detector
      Ref_t sensitiveDetector() const;
      
      /// Set the volume's solid shape
      void setSolid(const Solid& s)  const;
      /// Access to Solid (Shape)
      Solid solid() const;
      
      /// Set the volume's material
      void setMaterial(const Material& m)  const;
      /// Access to the Volume material
      Material material() const;
      
      /// Auto conversion to underlying ROOT object
      operator TGeoVolume*() const     { return m_element; }
    };
    
  }       /* End namespace Geometry           */
}         /* End namespace DD4hep            */
#endif    /* DD4hep_GEOMETRY_VOLUMES_H       */
