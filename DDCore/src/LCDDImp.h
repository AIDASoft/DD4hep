// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4hep_LCDDGEOIMP_H
#define DD4hep_LCDDGEOIMP_H
#include "DD4hep/LCDD.h"
//#include "XML/XMLElements.h"

class TGeoManager;

// C++ include files
#include <map>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {
  
  /*
   *   XML namespace declaration
   */
  namespace Geometry  {
    
    struct Materials {};
    
    class LCDDImp : public LCDD  {
    public:
      struct InvalidObjectError : public std::runtime_error {
        InvalidObjectError(const std::string& msg) : std::runtime_error(msg) {}
      };
      
      struct ObjectHandleMap : public HandleMap  {
        ObjectHandleMap() {}
        void append_noCheck(const Ref_t& e) { 
          if ( e.isValid() )  {
            std::string n = e.name();
            if ( this->find(n) != this->end() ) {
              throw InvalidObjectError("Object "+n+" is already present in map!");
            }
            this->insert(std::make_pair(n,e.ptr()));
          }
        }
        void append(const Ref_t& e) { 
          if ( e.isValid() )  {
            std::string n = e.name();
            this->insert(std::make_pair(n,e.ptr()));
            return;
          }
          throw InvalidObjectError("Attempt to add an invalid object object");
        }
        template <typename T> void append(const Ref_t& e) {
          T* obj = dynamic_cast<T*>(e.ptr());
          if ( obj )  {
            this->append(e);
            return;
          }
          throw InvalidObjectError("Attempt to add an object, which is of the wrong type.");
        }
      };
      
      ObjectHandleMap     m_readouts;
      ObjectHandleMap     m_header;
      ObjectHandleMap     m_idDict;
      ObjectHandleMap     m_limits;
      ObjectHandleMap     m_regions;
      ObjectHandleMap     m_detectors;
      ObjectHandleMap     m_alignments;

      ObjectHandleMap     m_sensitive;
      ObjectHandleMap     m_display;
      ObjectHandleMap     m_fields;
      
      // GDML fields
      ObjectHandleMap     m_gdml;
      ObjectHandleMap     m_define;
      ObjectHandleMap     m_structure;
      ObjectHandleMap     m_materials;
      ObjectHandleMap     m_solids;
      
      
      Volume              m_worldVol;
      Volume              m_trackingVol;
      
      Material            m_materialAir;
      Material            m_materialVacuum;
      
      Ref_t          m_setup;
      
      void convertMaterials(const std::string& uri);
      //void convertMaterials(XML::Handle_t doc_element);
      
      LCDDImp();

      /// Read compact geometry description or alignment file
      virtual void fromCompact(const std::string& fname);
      /// Apply & lock realigments
      virtual void applyAlignment();
      
      virtual void create();
      virtual void init();
      virtual void addStdMaterials();
      virtual void endDocument();
      
      void dump() const;
      
      virtual Handle<TObject>  getRefChild(const HandleMap& e, const std::string& name, bool throw_if_not=true)  const;
      virtual Volume         pickMotherVolume(const DetElement& sd) const;
      virtual Volume         worldVolume() const      { return m_worldVol;          }
      virtual Volume         trackingVolume() const   { return m_trackingVol;       }
      virtual Material       air() const              { return m_materialVacuum;    }
      virtual Material       vacuum() const           { return m_materialAir;       }
      
      virtual LimitSet limitSet(const std::string& name)  const
      {  return getRefChild(m_limits,name);                                         }  
      virtual VisAttr     visAttributes(const std::string& name) const
      {  return getRefChild(m_display,name,false);                                  }  
      virtual Material    material(const std::string& name)  const
      {  return getRefChild(m_materials,name);                                      }
      virtual Region      region(const std::string& name)  const
      {  return getRefChild(m_regions,name);                                        }
      virtual Ref_t  idSpec(const std::string& name)  const
      {  return getRefChild(m_idDict,name);                                         }
      virtual Volume      volume(const std::string& name)  const
      {  return getRefChild(m_structure,name);                                      }
      virtual Solid       solid(const std::string& name) const 
      {  return getRefChild(solids(),name);                                         }
      virtual Constant    constant(const std::string& name) const 
      {  return getRefChild(m_define,name);                                         }
      virtual Readout     readout(const std::string& name)  const
      {  return getRefChild(m_readouts,name);                                       }
      virtual DetElement detector(const std::string& name)  const
      {  return getRefChild(m_detectors,name);                                      }
      virtual AlignmentEntry alignment(const std::string& path) const 
      {  return getRefChild(alignments(),path);                                     }
      
      virtual const HandleMap& header()  const        { return m_header;            }
      virtual const HandleMap& constants() const      { return m_define;            }
      virtual const HandleMap& visAttributes() const  { return m_display;           }
      virtual const HandleMap& structure()  const     { return m_structure;         }
      virtual const HandleMap& solids()  const        { return m_solids;            }
      virtual const HandleMap& limitsets()  const     { return m_limits;            }
      virtual const HandleMap& regions() const        { return m_regions;           }
      virtual const HandleMap& materials()  const     { return m_materials;         }
      virtual const HandleMap& readouts() const       { return m_readouts;          }
      virtual const HandleMap& detectors()  const     { return m_detectors;         }
      virtual const HandleMap& alignments()  const    { return m_alignments;        }
      
      virtual LCDD& add(const Constant& x)            { return addConstant(x);      }
      virtual LCDD& add(const Solid& x)               { return addSolid(x);         }
      virtual LCDD& add(const Volume& x)              { return addVolume(x);        }
      virtual LCDD& add(const Material& x)            { return addMaterial(x);      }
      virtual LCDD& add(const LimitSet& x)            { return addLimitSet(x);      }
      virtual LCDD& add(const Region& x)              { return addRegion(x);        }
      virtual LCDD& add(const VisAttr& x)             { return addVisAttribute(x);  }
      virtual LCDD& add(const AlignmentEntry& x)      { return addAlignment(x);     }
      virtual LCDD& add(const Readout& x)             { return addReadout(x);       }
      virtual LCDD& add(const DetElement& x)          { return addDetector(x);      }
      
#define __R  return *this
      // These are manager by the TGeoManager
      virtual LCDD& addSolid(const Ref_t& x);       //  { m_solids.append(x);     __R;}
      virtual LCDD& addVolume(const Ref_t& x);      //  { m_structure.append(x);  __R;}
      
      // These not:
      virtual LCDD& addConstant(const Ref_t& x)         { m_define.append(x);     __R;}
      virtual LCDD& addMaterial(const Ref_t& x)         { m_materials.append(x);  __R;}
      virtual LCDD& addLimitSet(const Ref_t& x)         { m_limits.append(x);     __R;}
      virtual LCDD& addRegion(const Ref_t& x)           { m_regions.append(x);    __R;}
      virtual LCDD& addIDSpec(const Ref_t& x)           { m_idDict.append(x);     __R;}
      virtual LCDD& addReadout(const Ref_t& x)          { m_readouts.append(x);   __R;}
      virtual LCDD& addVisAttribute(const Ref_t& x)     { m_display.append(x);    __R;}
      virtual LCDD& addSensitiveDetector(const Ref_t& x){ m_sensitive.append(x);  __R;}
      virtual LCDD& addDetector(const Ref_t& x)         { m_detectors.append_noCheck(x);  __R;}

      virtual LCDD& addAlignment(const Ref_t& x)        { m_alignments.append(x); __R;}
#undef __R
      
    };
  }
}         /* End namespace DD4hep   */
#endif    /* DD4hep_LCDDGEOIMP_H    */
