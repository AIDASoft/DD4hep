#ifndef DETDESC_LCDDGEOIMP_H
#define DETDESC_LCDDGEOIMP_H
#include "LCDD.h"
#include "TGeoManager.h"

// C++ include files
#include <map>

/*
 *   DetDesc namespace declaration
 */
namespace DetDesc {

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

      struct ObjectHandleMap : public std::map<std::string,Handle_t>  {
        ObjectHandleMap() {}
        void append_noCheck(const RefElement& e) { 
          if ( e.isValid() )  {
            std::string n = e.name();
            this->insert(std::make_pair(n,e.ptr()));
          }
        }
        void append(const RefElement& e) { 
          if ( e.isValid() )  {
            std::string n = e.name();
            this->insert(std::make_pair(n,e.ptr()));
            return;
          }
          throw InvalidObjectError("Attempt to add an invalid object object");
        }
        template <class T> void append(const RefElement& e) {
          T* obj = dynamic_cast<T*>(e.ptr());
          if ( obj )  {
            this->append(e);
            return;
          }
          throw InvalidObjectError("Attempt to add an object, which is of the wrong type.");
        }
      };


      Document_t   *m_doc;
      ObjectHandleMap     m_readouts;
      //HandleMap           m_root;
      ObjectHandleMap     m_header;
      ObjectHandleMap     m_idDict;
      ObjectHandleMap     m_limits;
      ObjectHandleMap     m_regions;
      ObjectHandleMap     m_detectors;
      ObjectHandleMap     m_sensitive;
      ObjectHandleMap     m_display;
      ObjectHandleMap     m_fields;

      // GDML fields
      ObjectHandleMap     m_gdml;
      ObjectHandleMap     m_define;
      ObjectHandleMap     m_rotations;
      ObjectHandleMap     m_positions;
      ObjectHandleMap     m_structure;
      ObjectHandleMap     m_materials;
      ObjectHandleMap     m_solids;


      Volume              m_worldVol;
      Volume              m_trackingVol;
      Rotation            m_reflect;
      Matrix              m_identity;

      RefElement          m_setup;

      template<typename T> LCDD& __add(const RefElement& x, ObjectHandleMap& m,Int_t (TGeoManager::*func)(T*));

      void convertMaterials(const std::string& uri);
      //void convertMaterials(XML::Handle_t doc_element);

      LCDDImp();
      //void fromCompact(XML::Handle_t doc_element);
      //void fromCompact(const std::string& fname);
      
      virtual void fromCompact(const std::string& xmlfile);

      virtual Document create();
      virtual Document init();
      virtual void addStdMaterials();
      virtual void endDocument();

      void dump() const;

      virtual Handle_t getRefChild(const HandleMap& e, const std::string& name, bool throw_if_not=true)  const;
      virtual Volume   pickMotherVolume(const Subdetector& sd) const;
      virtual Volume   worldVolume() const            { return m_worldVol;          }
      virtual Volume   trackingVolume() const         { return m_trackingVol;       }
      virtual Rotation reflection() const             { return m_reflect;           }
      virtual Matrix   identity() const               { return m_identity;          }

      virtual LimitSet limitSet(const std::string& name)  const
      {  return getRefChild(m_limits,name);                                         }  
      virtual VisAttr     visAttributes(const std::string& name) const
      {  return getRefChild(m_display,name,false);                                  }  
      virtual Material    material(const std::string& name)  const
      {  return getRefChild(m_materials,name);                                      }
      virtual Region      region(const std::string& name)  const
      {  return getRefChild(m_regions,name);                                        }
      virtual RefElement  idSpec(const std::string& name)  const
      {  return getRefChild(m_idDict,name);                                         }
      virtual Volume      volume(const std::string& name)  const
      {  return getRefChild(m_structure,name);                                      }
      virtual Rotation    rotation(const std::string& name) const 
      {  return getRefChild(m_rotations,name);                                      }
      virtual Position    position(const std::string& name) const 
      {  return getRefChild(m_positions,name);                                      }
      virtual Solid       solid(const std::string& name) const 
      {  return getRefChild(solids(),name);                                         }
      virtual Constant    constant(const std::string& name) const 
      {  return getRefChild(m_define,name);                                         }
      virtual Readout     readout(const std::string& name)  const
      {  return getRefChild(m_readouts,name);                                       }
      virtual Subdetector detector(const std::string& name)  const
      {  return getRefChild(m_detectors,name);                                      }

      virtual Document document() const               { return Document(m_doc);     }
      virtual const HandleMap& header()  const        { return m_header;            }
      virtual const HandleMap& constants() const      { return m_define;            }
      virtual const HandleMap& visAttributes() const  { return m_display;           }
      virtual const HandleMap& positions() const      { return m_positions;         }
      virtual const HandleMap& rotations() const      { return m_rotations;         }
      virtual const HandleMap& structure()  const     { return m_structure;         }
      virtual const HandleMap& solids()  const        { return m_solids;            }
      virtual const HandleMap& limitsets()  const     { return m_limits;            }
      virtual const HandleMap& regions() const        { return m_regions;           }
      virtual const HandleMap& materials()  const     { return m_materials;         }
      virtual const HandleMap& readouts() const       { return m_readouts;          }
      virtual const HandleMap& detectors()  const     { return m_detectors;         }

      virtual LCDD& add(const Constant& x)            { return addConstant(x);      }
      virtual LCDD& add(const Solid& x)               { return addSolid(x);         }
      virtual LCDD& add(const Volume& x)              { return addVolume(x);        }
      virtual LCDD& add(const Material& x)            { return addMaterial(x);      }
      virtual LCDD& add(const Position& x)            { return addPosition(x);      }
      virtual LCDD& add(const Rotation& x)            { return addRotation(x);      }
      virtual LCDD& add(const LimitSet& x)            { return addLimitSet(x);      }
      virtual LCDD& add(const Region& x)              { return addRegion(x);        }
      virtual LCDD& add(const VisAttr& x)             { return addVisAttribute(x);  }
      virtual LCDD& add(const Readout& x)             { return addReadout(x);       }
      virtual LCDD& add(const Subdetector& x)         { return addDetector(x);      }

#define __R  return *this
      // These are manager by the TGeoManager
      virtual LCDD& addSolid(const RefElement& x);       //  { m_solids.append(x);     __R;}
      virtual LCDD& addVolume(const RefElement& x);      //  { m_structure.append(x);  __R;}
      virtual LCDD& addRotation(const RefElement& x);    //  { m_rotations.append(x);  __R;}
      virtual LCDD& addPosition(const RefElement& x);    //  { m_positions.append(x);  __R;}

      // These not:
      virtual LCDD& addConstant(const RefElement& x)         { m_define.append(x);     __R;}
      virtual LCDD& addMaterial(const RefElement& x)         { m_materials.append(x);  __R;}
      virtual LCDD& addLimitSet(const RefElement& x)         { m_limits.append(x);     __R;}
      virtual LCDD& addRegion(const RefElement& x)           { m_regions.append(x);    __R;}
      virtual LCDD& addIDSpec(const RefElement& x)           { m_idDict.append(x);     __R;}
      virtual LCDD& addReadout(const RefElement& x)          { m_readouts.append(x);   __R;}
      virtual LCDD& addVisAttribute(const RefElement& x)     { m_display.append(x);    __R;}
      virtual LCDD& addSensitiveDetector(const RefElement& x){ m_sensitive.append(x);  __R;}
      virtual LCDD& addDetector(const RefElement& x)         { m_detectors.append_noCheck(x);  __R;}
#undef __R

    };
  }
}         /* End namespace DetDesc   */
#endif    /* DETDESC_LCDDGEOIMP_H    */
