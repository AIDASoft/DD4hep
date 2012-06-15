// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4hep_LCDD_LCDD_H
#define DD4hep_LCDD_LCDD_H

// Framework includes
#include "DD4hep/Handle.h"
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

      virtual ~LCDD() {}

      virtual void create() = 0;
      virtual void init() = 0;
      virtual void endDocument() = 0;
      virtual void addStdMaterials() = 0;

      virtual Material air() const = 0;
      virtual Material vacuum() const = 0;

      virtual DetElement world() const = 0;
      virtual DetElement trackers() const = 0;

      virtual Volume   worldVolume() const = 0;
      virtual Volume   trackingVolume() const = 0;

      virtual const HandleMap& header()  const = 0;
      virtual const HandleMap& constants()  const = 0;
      virtual const HandleMap& regions() const = 0;
      virtual const HandleMap& materials()  const = 0;
      virtual const HandleMap& detectors()  const = 0;
      virtual const HandleMap& readouts() const = 0;
      virtual const HandleMap& visAttributes() const = 0;
      virtual const HandleMap& limitsets()  const = 0;
      virtual const HandleMap& alignments()  const = 0;

      virtual Volume         pickMotherVolume(const DetElement& sd) const = 0;
      virtual Region         region(const std::string& name)  const = 0;
      virtual VisAttr        visAttributes(const std::string& name) const = 0;
      virtual LimitSet       limitSet(const std::string& name)  const = 0;
      virtual Material       material(const std::string& name)  const = 0;
      virtual Readout        readout(const std::string& name)  const = 0;
      virtual Ref_t          idSpec(const std::string& name)  const = 0;
      virtual Constant       constant(const std::string& name) const = 0;
      virtual AlignmentEntry alignment(const std::string& path) const = 0;
      virtual DetElement     detector(const std::string& name) const = 0;

      virtual LCDD& add(const Constant& constant) = 0;
      virtual LCDD& add(const Region& region) = 0;
      virtual LCDD& add(const Material& mat) = 0;
      virtual LCDD& add(const VisAttr& attr) = 0;
      virtual LCDD& add(const LimitSet& limset) = 0;
      virtual LCDD& add(const DetElement& detector) = 0;
      virtual LCDD& add(const AlignmentEntry& x) = 0;

      virtual LCDD& addIDSpec(const Ref_t& element) = 0;
      virtual LCDD& addConstant(const Ref_t& element) = 0;
      virtual LCDD& addMaterial(const Ref_t& element) = 0;
      virtual LCDD& addVisAttribute(const Ref_t& element) = 0;
      virtual LCDD& addSensitiveDetector(const Ref_t& e) = 0;
      virtual LCDD& addLimitSet(const Ref_t& limset) = 0;
      virtual LCDD& addRegion(const Ref_t& region) = 0;
      virtual LCDD& addReadout(const Ref_t& readout) = 0;
      virtual LCDD& addDetector(const Ref_t& detector) = 0;
      /// Add alignment entry
      virtual LCDD& addAlignment(const Ref_t& alignment) = 0;

      /// Read compact geometry description or alignment file
      virtual void fromCompact(const std::string& fname) = 0;
      /// Apply & lock realigments
      virtual void applyAlignment() = 0;

      /// Dump geometry description
      virtual void dump() const = 0;
      
      ///---Factory method-------
      static LCDD& getInstance(void);
    };
  }       /* End namespace Geometry  */
}         /* End namespace DD4hep   */
#endif    /* DD4hep_LCDD_LCDD_H     */
