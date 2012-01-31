#ifndef DETDESC_LCDDGEO_H
#define DETDESC_LCDDGEO_H

// Framework includes
#include "Elements.h"
#include "Objects.h"
#include "Shapes.h"
#include "Readout.h"
#include "SubDetector.h"

// C/C++ include files
#include <map>

/*
 *   DetDesc namespace declaration
 */
namespace DetDesc {

  /*
  *   XML namespace declaration
  */
  namespace Geometry  {

    struct Subdetector;
    struct Readout;
    struct Document;
    struct Solid;
    struct Constant;
    struct Volume;
    struct Region;
    struct Material;
    struct Materials;
    struct Position;
    struct Rotation;
    struct LimitSet;
    struct VisAttr;
    struct Compact;
    struct LCDD;

    struct LCDD {
      typedef std::map<std::string,Handle_t> HandleMap;

      virtual ~LCDD() {}

      virtual Document document() const = 0;
      virtual Document create() = 0;
      virtual Document init() = 0;
      virtual void endDocument() = 0;
      virtual void addStdMaterials() = 0;

      virtual Volume   worldVolume() const = 0;
      virtual Volume   trackingVolume() const = 0;
      virtual Rotation reflection() const = 0;
      virtual Matrix   identity() const = 0;

      virtual const HandleMap& header()  const = 0;
      virtual const HandleMap& constants()  const = 0;
      virtual const HandleMap& regions() const = 0;
      virtual const HandleMap& structure()  const = 0;
      virtual const HandleMap& solids()  const = 0;
      virtual const HandleMap& positions() const = 0;
      virtual const HandleMap& rotations() const = 0;
      virtual const HandleMap& materials()  const = 0;
      virtual const HandleMap& detectors()  const = 0;
      virtual const HandleMap& readouts() const = 0;
      virtual const HandleMap& visAttributes() const = 0;
      virtual const HandleMap& limitsets()  const = 0;

      virtual Region      region(const std::string& name)  const = 0;
      virtual VisAttr     visAttributes(const std::string& name) const = 0;
      virtual LimitSet    limitSet(const std::string& name)  const = 0;
      virtual Material    material(const std::string& name)  const = 0;
      virtual Readout     readout(const std::string& name)  const = 0;
      virtual RefElement  idSpec(const std::string& name)  const = 0;
      virtual Volume      pickMotherVolume(const Subdetector& sd) const = 0;
      virtual Constant    constant(const std::string& name) const = 0;
      virtual Position    position(const std::string& name) const = 0;
      virtual Rotation    rotation(const std::string& name) const = 0;
      virtual Solid       solid(const std::string& name) const = 0;
      virtual Subdetector detector(const std::string& name) const = 0;

      virtual LCDD& add(const Constant& constant) = 0;
      virtual LCDD& add(const Solid& solid) = 0;
      virtual LCDD& add(const Region& region) = 0;
      virtual LCDD& add(const Volume& vol) = 0;
      virtual LCDD& add(const Material& mat) = 0;
      virtual LCDD& add(const VisAttr& attr) = 0;
      virtual LCDD& add(const Position& pos) = 0;
      virtual LCDD& add(const Rotation& rot) = 0;
      virtual LCDD& add(const LimitSet& limset) = 0;
      virtual LCDD& add(const Subdetector& detector) = 0;

      virtual LCDD& addIDSpec(const RefElement& element) = 0;
      virtual LCDD& addConstant(const RefElement& element) = 0;
      virtual LCDD& addMaterial(const RefElement& element) = 0;
      virtual LCDD& addVisAttribute(const RefElement& element) = 0;
      virtual LCDD& addSensitiveDetector(const RefElement& e) = 0;
      virtual LCDD& addLimitSet(const RefElement& limset) = 0;
      virtual LCDD& addRegion(const RefElement& region) = 0;
      virtual LCDD& addReadout(const RefElement& readout) = 0;
      virtual LCDD& addDetector(const RefElement& detector) = 0;
      //---Factory method-------
      static LCDD& getInstance(void);
      virtual void fromCompact(const std::string& xmlfile = "") = 0;
      virtual void dump() const = 0;
      
    };
  }       /* End namespace Geometry  */
}         /* End namespace DetDesc   */
#endif    /* DETDESC_LCDDGEO_H       */
